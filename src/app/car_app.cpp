#include "app/car_app.hpp"
#include <Arduino.h>

namespace car {

    CarApp::CarApp()
    : connect_manager_(config_),
      motion_controller_(config_),
      sensor_manager_(config_),
      display_manager_(config_),
      last_mcu_publish_tick_(0),
      last_imu_publish_tick_(0),
      last_ultrasonic_read_tick_(0),
      last_ros_init_attempt_tick_(millis() - config_.ros_init_retry_period),
      previous_agent_connected_(false),
      ros_failure_count_(0) {}

    void CarApp::setup() {
        // 启动顺序固定为：硬件 -> WiFi/micro-ROS传输 -> ROS实体
        init_hardware();
        init_connect();
        init_ros_entities();
    }

    void CarApp::init_hardware() {
        // 串口先启动，后续WiFi、IMU、电机和ROS初始化问题都从这里输出
        Serial.begin(115200);
        delay(200);
        Serial.println("ESP32小车开始启动");
        sensor_manager_.initialize();
        // OLED初始化放在传感器之后，因为二者共用I2C引脚配置
        display_manager_.initialize();
        // 电机初始化最后执行，并立即stop_motors，防止上电残留PWM
        motion_controller_.initialize_hardware();
    }

    void CarApp::init_connect() {
        // WiFi连接先启动，micro-ROS连接要等WiFi稳定后在loop中注册
        connect_manager_.connect_wifi();
        // /car/display_message由RosEntityManager接收，再回调给OLED管理器显示
        ros_manager_.set_display_message_callback(
            &CarApp::handle_display_message,
            this);
    }

    bool CarApp::init_ros_entities() {
        // 当前Arduino毫秒tick，用于ROS初始化失败退避
        const unsigned long now_tick = millis();
        // ROS实体创建失败时按周期重试，避免loop()高频构建/销毁造成阻塞
        if (now_tick - last_ros_init_attempt_tick_ < config_.ros_init_retry_period) {
            return false;
        }
        last_ros_init_attempt_tick_ = now_tick;
        if (!connect_manager_.wifi_connected()) {
            // WiFi未连上时不要尝试rclc初始化，先让OLED回到本地状态
            display_manager_.clear_remote_message();
            return false;
        }
        if (!connect_manager_.agent_connected()) {
            // Agent未稳定时也不创建实体，避免刚建好就被publish failure销毁
            display_manager_.clear_remote_message();
            return false;
        }
        if (!ros_manager_.init(&motion_controller_)) {
            // 初始化失败后立即destroy，确保半初始化的rcl句柄不会影响下一次重试
            ros_manager_.destroy();
            display_manager_.clear_remote_message();
            ++ros_failure_count_;
            Serial.println("micro-ROS实体创建失败，等待重试");
            return false;
        }
        ros_failure_count_ = 0;
        Serial.println("micro-ROS实体已创建");
        return true;
    }

    void CarApp::reset_ros_entities() {
        // reset_ros_entities是Agent断开、发布失败和手动恢复共用的清理入口
        if (ros_manager_.is_init()) {
            ros_manager_.destroy();
            Serial.println("micro-ROS实体已销毁");
        }
        // ROS 通道失效后清掉Web文本，OLED不再显示可能过期的小车在线
        display_manager_.clear_remote_message();
        ros_failure_count_ = 0;
    }

    void CarApp::maintain_ros_entities() {
        // 当前经过连续ping和宽限窗口处理后的Agent状态
        const bool agent_connected = connect_manager_.agent_connected();
        // 只在从已连接变为断开时销毁实体，短暂ping丢失由WiFi连接管理容错
        if (previous_agent_connected_ && !agent_connected) {
            reset_ros_entities();
        }
        previous_agent_connected_ = agent_connected;
        if (!ros_manager_.is_init()) {
            // 未初始化时每轮尝试，但init_ros_entities内部会按时间节流
            init_ros_entities();
        }
    }

    McuStatus CarApp::build_status(const IMU& imu, const Ultrasonic ultrasonic) const {
        // 准备发布给Jetson的底盘信息
        McuStatus mcu_status{};
        // 电机控制器当前目标、反馈和健康状态副本
        const MotionState motion_state = motion_controller_.get_motion_state();
        mcu_status.wifi_connect_ok = connect_manager_.wifi_connected();
        mcu_status.agent_connect_ok = connect_manager_.agent_connected();
        mcu_status.imu_ok = imu.ok;
        mcu_status.encoder_ok = motion_state.encoder_ok;
        mcu_status.motor_driver_ok = motion_state.motor_driver_ok;
        mcu_status.cmd_timeout_ok = motion_state.cmd_timeout_ok;
        mcu_status.left_wheel_speed = motion_state.actual_left;
        mcu_status.right_wheel_speed = motion_state.actual_right;
        mcu_status.left_encoder_ticks = motion_state.left_ticks;
        mcu_status.right_encoder_ticks = motion_state.right_ticks;
        mcu_status.ultrasonic_ok = ultrasonic.ok;
        mcu_status.obstacle_ok = ultrasonic.obstacle_ok;
        mcu_status.obstacle_distance = ultrasonic.distance;
        // fault_bits是给Jetson快速定位问题用的位图
        if (!mcu_status.wifi_connect_ok) {
            mcu_status.fault_bits |= MCU_FAULT_WIFI_DISCONNECTED;
        }
        if (!mcu_status.agent_connect_ok) {
            mcu_status.fault_bits |= MCU_FAULT_AGENT_DISCONNECTED;
        }
        if (!mcu_status.imu_ok) {
            mcu_status.fault_bits |= MCU_FAULT_IMU_INVALID;
        }
        if (!mcu_status.encoder_ok) {
            mcu_status.fault_bits |= MCU_FAULT_ENCODER_INVALID;
        }
        if (mcu_status.cmd_timeout_ok) {
            mcu_status.fault_bits |= MCU_FAULT_COMMAND_TIMEOUT;
        }
        if (!mcu_status.motor_driver_ok) {
            mcu_status.fault_bits |= MCU_FAULT_MOTOR_DRIVER_INVALID;
        }
        if (!mcu_status.ultrasonic_ok) {
            mcu_status.fault_bits |= MCU_WARNING_ULTRASONIC_NO_ECHO;
        }
        if (mcu_status.obstacle_ok) {
            mcu_status.fault_bits |= MCU_FAULT_OBSTACLE_LATCHED;
        }
        return mcu_status;
    }

    void CarApp::handle_display_message(void * context, const char * message) {
        // context在init_connect中传入this，避免静态回调直接依赖全局对象
        static_cast<CarApp *>(context)->display_manager_.set_remote_message(message);
    }

    void CarApp::update_display() {
        // OLED每轮调用，实际刷新周期由OledDisplayManager内部节流
        display_manager_.update(
            connect_manager_,
            ros_manager_.is_init(),
            last_mcu_status_);
    }

    void CarApp::publish_periodic_topics() {
        // 当前Arduino毫秒tick，用于所有周期任务调度
        const unsigned long now_tick = millis();
        // IMU读取较快，每轮先更新
        last_imu_ = sensor_manager_.read_imu();
        // 先发布快速传感器和上一次已完成的超声波结果，避免最长约 25 ms 的
        // pulseIn 阻塞持续推迟 IMU 与 MCU 心跳。
        last_mcu_status_= build_status(last_imu_, last_ultrasonic_);
        if (ros_manager_.is_init() &&
            now_tick - last_imu_publish_tick_ >= config_.imu_publish_period) {
            // 发布失败计数由外层统一处理，达到阈值后重建ROS实体
            if (!ros_manager_.publish_imu(last_imu_)) {
                ++ros_failure_count_;
            }
            last_imu_publish_tick_ = now_tick;
        }
        if (ros_manager_.is_init() &&
            now_tick - last_mcu_publish_tick_ >= config_.mcu_publish_period) {
            // MCU状态是Jetson判断在线和运动准入的核心心跳，成功发布后清失败计数
            if (!ros_manager_.publish_mcu_status(last_mcu_status_))
            {
                ++ros_failure_count_;
            } else {
                ros_failure_count_ = 0;
            }
            last_mcu_publish_tick_ = now_tick;
        }
        // 慢速测距放在周期发布之后。本地障碍锁在测量完成后立即送入电机控制器，
        // Jetson 最迟在下一次 MCU 状态发布时看到同一结果。
        if (now_tick - last_ultrasonic_read_tick_ >= config_.ultrasonic_read_period) {
            last_ultrasonic_ = sensor_manager_.read_ultrasonic();
            motion_controller_.set_obstacle_state(last_ultrasonic_.obstacle_ok);
            last_ultrasonic_read_tick_ = millis();
        }
        if (ros_failure_count_ >= config_.ros_publish_failure_limit) {
            // 连续失败通常说明Agent和连接状态异常，重建实体比继续发布更可靠
            reset_ros_entities();
        }
    }

    void CarApp::loop() {
        // Arduino主循环是协作式调度：每个模块都要尽快返回，保证电机超时和状态发布及时执行
        connect_manager_.maintain_wifi();
        connect_manager_.update_agent_state();
        maintain_ros_entities();
        if (ros_manager_.is_init() && !ros_manager_.spin_some()) {
            // executor失败同样计入ROS故障，和发布失败走同一套恢复流程
            ++ros_failure_count_;
        }
        motion_controller_.update();
        publish_periodic_topics();
        update_display();
        delay(5);
    }

}
