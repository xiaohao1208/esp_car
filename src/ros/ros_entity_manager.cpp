#include "ros/ros_entity_manager.hpp"
#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rosidl_runtime_c/string_functions.h>
#include "control/motion_controller.hpp"

namespace car {

    // rclc静态回调当前转发到的管理器实例，固件只创建一个RosEntityManager
    RosEntityManager * RosEntityManager::instance_ = nullptr;

    RosEntityManager::RosEntityManager()
    : motion_controller_(nullptr),
      display_message_callback_(nullptr),
      display_message_context_(nullptr),
      all_init_(false),
      support_init_(false),
      node_init_(false),
      executor_init_(false),
      cmd_vel_move_sub_init_(false),
      display_message_sub_init_(false),
      imu_pub_init_(false),
      mcu_status_pub_init_(false) {
        // rclc的C回调不能直接捕获this，所以用单例指针转发到当前对象
        instance_ = this;
        // 构造时先把所有rcl/rclc句柄置为zero initialized，后续destroy可安全判断
        allocator_ = rcl_get_default_allocator();
        support_ = rclc_support_t();
        node_ = rcl_get_zero_initialized_node();
        executor_ = rclc_executor_get_zero_initialized_executor();
        cmd_vel_move_sub_ = rcl_get_zero_initialized_subscription();
        display_message_sub_ = rcl_get_zero_initialized_subscription();
        imu_pub_ = rcl_get_zero_initialized_publisher();
        mcu_status_pub_ = rcl_get_zero_initialized_publisher();
        geometry_msgs__msg__Twist__init(&cmd_vel_move_msg_);
        std_msgs__msg__String__init(&display_message_msg_);
        sensor_msgs__msg__Imu__init(&imu_msg_);
        car_interfaces__msg__CarMcuStatus__init(&mcu_status_msg_);
    }

    RosEntityManager::~RosEntityManager() {
        destroy();
    }

    bool RosEntityManager::init(MotionController * motion_controller) {
        // 重复初始化直接成功，避免Agent状态抖动时重复创建同一批ROS实体
        if (all_init_) {
            return true;
        }
        motion_controller_ = motion_controller;
        allocator_ = rcl_get_default_allocator();
        support_ = rclc_support_t();
        node_ = rcl_get_zero_initialized_node();
        executor_ = rclc_executor_get_zero_initialized_executor();
        cmd_vel_move_sub_ = rcl_get_zero_initialized_subscription();
        display_message_sub_ = rcl_get_zero_initialized_subscription();
        imu_pub_ = rcl_get_zero_initialized_publisher();
        mcu_status_pub_ = rcl_get_zero_initialized_publisher();
        if (!prepare_display_message_buffer()) {
            return false;
        }
        // 支持对象、节点、订阅、发布器、executor必须按rclc规定顺序创建
        if (rclc_support_init(&support_, 0, nullptr, &allocator_) != RCL_RET_OK) {
            destroy();
            return false;
        }
        support_init_ = true;
        if (rclc_node_init_default(&node_, "esp32_car", "", &support_) != RCL_RET_OK) {
            destroy();
            return false;
        }
        node_init_ = true;
        // MCU只接受/cmd_vel_move
        if (rclc_subscription_init_best_effort(
            &cmd_vel_move_sub_,
            &node_,
            ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
            "/cmd_vel_move") != RCL_RET_OK) {
            destroy();
            return false;
        }
        cmd_vel_move_sub_init_ = true;
        // Web/OLED状态文本通过String下发，ESP32只显示，不参与控制
        if (rclc_subscription_init_best_effort(
            &display_message_sub_,
            &node_,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
            "/car/display_message") != RCL_RET_OK) {
            destroy();
            return false;
        }
        display_message_sub_init_ = true;
        if (rclc_publisher_init_best_effort(
            &imu_pub_,
            &node_,
            ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
            "/imu/data") != RCL_RET_OK) {
            destroy();
            return false;
        }
        imu_pub_init_ = true;
        if (rclc_publisher_init_best_effort(
            &mcu_status_pub_,
            &node_,
            ROSIDL_GET_MSG_TYPE_SUPPORT(car_interfaces, msg, CarMcuStatus),
            "/car/mcu_status") != RCL_RET_OK) {
            destroy();
            return false;
        }
        mcu_status_pub_init_ = true;
        if (rclc_executor_init(&executor_, &support_.context, 2, &allocator_) != RCL_RET_OK) {
            destroy();
            return false;
        }
        executor_init_ = true;
        // executor容量为2，对应速度订阅和OLED文本订阅
        if (rclc_executor_add_subscription(
            &executor_,
            &cmd_vel_move_sub_,
            &cmd_vel_move_msg_,
            &RosEntityManager::twist_callback,
            ON_NEW_DATA) != RCL_RET_OK) {
            destroy();
            return false;
        }
        if (rclc_executor_add_subscription(
            &executor_,
            &display_message_sub_,
            &display_message_msg_,
            &RosEntityManager::display_message_callback,
            ON_NEW_DATA) != RCL_RET_OK) {
            destroy();
            return false;
        }
        // IMU的frame_id固定为URDF中的imu_link，Jetson端无需再猜测坐标系
        rosidl_runtime_c__String__assign(&imu_msg_.header.frame_id, "imu_link");
        // MPU当前只发布加速度和角速度，不提供绝对姿态，ROS约定协方差首项为-1表示忽略orientation
        imu_msg_.orientation.w = 1.0;
        imu_msg_.orientation_covariance[0] = -1.0;
        // 非零协方差避免上层把原始MEMS测量误认为完全无噪声，数值可在实车静止采样后继续标定
        imu_msg_.angular_velocity_covariance[0] = 0.0025;
        imu_msg_.angular_velocity_covariance[4] = 0.0025;
        imu_msg_.angular_velocity_covariance[8] = 0.0025;
        imu_msg_.linear_acceleration_covariance[0] = 0.04;
        imu_msg_.linear_acceleration_covariance[4] = 0.04;
        imu_msg_.linear_acceleration_covariance[8] = 0.04;
        sync_time();
        all_init_ = true;
        return true;
    }

    bool RosEntityManager::prepare_display_message_buffer() {
        if (display_message_msg_.data.capacity >= kDisplayMessageBufferCapacity) {
            display_message_msg_.data.size = 0;
            display_message_msg_.data.data[0] = '\0';
            return true;
        }
        // 用于一次性扩展rosidl字符串容量的全零占位缓冲区
        char placeholder[kDisplayMessageBufferCapacity] = {};
        // rosidl字符串容量预分配是否成功
        const bool ok = rosidl_runtime_c__String__assignn(
            &display_message_msg_.data,
            placeholder,
            kDisplayMessageBufferCapacity - 1);
        if (!ok) {
            return false;
        }
        // micro-ROS订阅可变长字符串前必须有足够capacity，size仍保持0，等待Agent写入真实文本
        display_message_msg_.data.size = 0;
        display_message_msg_.data.data[0] = '\0';
        return true;
    }

    void RosEntityManager::set_display_message_callback(DisplayMessageCallback callback, void * context) {
        display_message_callback_ = callback;
        display_message_context_ = context;
    }

    void RosEntityManager::destroy() {
        // 销毁ROS实体前先停电机，防止Agent断开后旧速度继续生效
        if (motion_controller_ != nullptr) {
            motion_controller_->stop_motors();
        }
        // rcl_*_fini失败也继续清理其它对象，保证下次初始化可以重新开始
        if (imu_pub_init_) {
            // IMU发布器销毁结果，清理阶段不因单个对象失败而中断
            const rcl_ret_t ret = rcl_publisher_fini(&imu_pub_, &node_);
            (void)ret;
        }
        if (mcu_status_pub_init_) {
            // MCU状态发布器销毁结果，仅用于完整执行rcl清理调用
            const rcl_ret_t ret = rcl_publisher_fini(&mcu_status_pub_, &node_);
            (void)ret;
        }
        if (cmd_vel_move_sub_init_) {
            // 运动命令订阅器销毁结果,失败时仍继续释放其余实体
            const rcl_ret_t ret = rcl_subscription_fini(&cmd_vel_move_sub_, &node_);
            (void)ret;
        }
        if (display_message_sub_init_) {
            // OLED文本订阅器销毁结果，失败时仍继续释放其余实体
            const rcl_ret_t ret = rcl_subscription_fini(&display_message_sub_, &node_);
            (void)ret;
        }
        if (executor_init_) {
            (void)rclc_executor_fini(&executor_);
        }
        if (node_init_) {
            // ESP32 ROS节点销毁结果，清理路径不向上抛出该错误
            const rcl_ret_t ret = rcl_node_fini(&node_);
            (void)ret;
        }
        if (support_init_) {
            (void)rclc_support_fini(&support_);
        }
        // 标志位全部清零后，重新赋zero initialized句柄，避免二次fini旧句柄
        all_init_ = false;
        support_init_ = false;
        node_init_ = false;
        executor_init_ = false;
        cmd_vel_move_sub_init_ = false;
        display_message_sub_init_ = false;
        imu_pub_init_ = false;
        mcu_status_pub_init_ = false;
        support_ = rclc_support_t();
        node_ = rcl_get_zero_initialized_node();
        executor_ = rclc_executor_get_zero_initialized_executor();
        cmd_vel_move_sub_ = rcl_get_zero_initialized_subscription();
        display_message_sub_ = rcl_get_zero_initialized_subscription();
        imu_pub_ = rcl_get_zero_initialized_publisher();
        mcu_status_pub_ = rcl_get_zero_initialized_publisher();
    }

    bool RosEntityManager::is_init() const {
        return all_init_;
    }

    bool RosEntityManager::spin_some() {
        if (!all_init_) {
            return false;
        }
        // 每次最多处理5ms，避免micro-ROS executor长时间阻塞电机安全更新
        return rclc_executor_spin_some(&executor_, RCL_MS_TO_NS(5)) == RCL_RET_OK;
    }

    void RosEntityManager::sync_time() {
        // 时间同步失败不阻止运行，发布时会回退到millis()
        // 当前时间同步尝试序号，最多执行十次短同步
        for (int attempt = 0; attempt < 10 && !rmw_uros_epoch_synchronized(); ++attempt) {
            rmw_uros_sync_session(1000);
            delay(10);
        }
    }

    int64_t RosEntityManager::epoch_time() const {
        // 同步成功时使用Agent/ROS时间，否则使用本地开机毫秒，至少保持单调递增
        if (rmw_uros_epoch_synchronized()) {
            return rmw_uros_epoch_millis();
        }
        return static_cast<int64_t>(millis());
    }

    void RosEntityManager::twist_callback(const void * msg_in) {
        // C回调入口只做类型转换和转发，实际运动逻辑留在MotionController
        if (instance_ == nullptr) {
            return;
        }
        // executor传入的只读Twist消息指针
        const auto * msg = static_cast<const geometry_msgs__msg__Twist *>(msg_in);
        instance_->handle_twist(*msg);
    }

    void RosEntityManager::display_message_callback(const void * msg_in) {
        // OLED文本订阅和速度订阅共用executor，但互不影响
        if (instance_ == nullptr) {
            return;
        }
        // executor传入的只读OLED状态字符串消息指针
        const auto * msg = static_cast<const std_msgs__msg__String *>(msg_in);
        instance_->handle_display_message(*msg);
    }

    void RosEntityManager::handle_twist(const geometry_msgs__msg__Twist & msg) {
        // 只取差速小车需要的linear.x和angular.z，其余Twist分量忽略
        if (motion_controller_ == nullptr) {
            return;
        }
        motion_controller_->set_target_twist(msg.linear.x, msg.angular.z);
    }

    void RosEntityManager::handle_display_message(const std_msgs__msg__String & msg) {
        // String缓冲为空时忽略，避免OLED显示未初始化内存
        if (display_message_callback_ == nullptr || msg.data.data == nullptr) {
            return;
        }
        display_message_callback_(display_message_context_, msg.data.data);
    }

    bool RosEntityManager::publish_imu(const IMU& imu) {
        if (!all_init_) {
            return false;
        }
        imu_msg_.linear_acceleration.x = imu.linear_x;
        imu_msg_.linear_acceleration.y = imu.linear_y;
        imu_msg_.linear_acceleration.z = imu.linear_z;
        imu_msg_.angular_velocity.x = imu.angular_x;
        imu_msg_.angular_velocity.y = imu.angular_y;
        imu_msg_.angular_velocity.z = imu.angular_z;
        // 本帧IMU消息使用的同步时间，未同步时为本机开机时间
        const int64_t stamp_tick = epoch_time();
        // 优先使用micro-ROS同步后的时间，方便Jetson融合IMU与里程计数据
        imu_msg_.header.stamp.sec = static_cast<int32_t>(stamp_tick / 1000);
        imu_msg_.header.stamp.nanosec = static_cast<uint32_t>((stamp_tick % 1000) * 1000000);
        return rcl_publish(&imu_pub_, &imu_msg_, nullptr) == RCL_RET_OK;
    }

    bool RosEntityManager::publish_mcu_status(const McuStatus& mcu_status) {
        if (!all_init_) {
            return false;
        }
        // 本帧 MCU 状态消息使用的同步时间，未同步时为本机开机时间
        const int64_t stamp_tick = epoch_time();
        // CarMcuStatus 是 ESP32 到 Jetson 的唯一原始底盘状态通道
        mcu_status_msg_.stamp.sec = static_cast<int32_t>(stamp_tick / 1000);
        mcu_status_msg_.stamp.nanosec = static_cast<uint32_t>((stamp_tick % 1000) * 1000000);
        // 下面字段保持和 car_interfaces/CarMcuStatus.msg 完全一致，Jetson 直接按该契约解析
        mcu_status_msg_.wifi_connect_ok = mcu_status.wifi_connect_ok;
        mcu_status_msg_.agent_connect_ok = mcu_status.agent_connect_ok;
        mcu_status_msg_.imu_ok = mcu_status.imu_ok;
        mcu_status_msg_.encoder_ok = mcu_status.encoder_ok;
        mcu_status_msg_.motor_driver_ok = mcu_status.motor_driver_ok;
        mcu_status_msg_.cmd_timeout_ok = mcu_status.cmd_timeout_ok;
        mcu_status_msg_.left_wheel_speed = mcu_status.left_wheel_speed;
        mcu_status_msg_.right_wheel_speed = mcu_status.right_wheel_speed;
        mcu_status_msg_.left_encoder_ticks = mcu_status.left_encoder_ticks;
        mcu_status_msg_.right_encoder_ticks = mcu_status.right_encoder_ticks;
        mcu_status_msg_.ultrasonic_ok = mcu_status.ultrasonic_ok;
        mcu_status_msg_.obstacle_ok = mcu_status.obstacle_ok;
        mcu_status_msg_.obstacle_distance = mcu_status.obstacle_distance;
        mcu_status_msg_.fault_bits = mcu_status.fault_bits;
        return rcl_publish(&mcu_status_pub_, &mcu_status_msg_, nullptr) == RCL_RET_OK;
    }

}