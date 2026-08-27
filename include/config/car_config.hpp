#ifndef CAR_CONFIG_HPP_
#define CAR_CONFIG_HPP_

#include <stdint.h>
#include <IPAddress.h>

namespace car {

    // 所有默认硬件参数集中在car_config.hpp编译期配置文件中
    struct ConfigDefaults
    {
        // esp32连接固定2.4GHz热点名称
        const char* wifi_ssid = "jetson";
        // 固定热点密码
        const char* wifi_password = "88888888";
        // 固定热点的IP地址192.168.4.1，micro-ros agent所在的主机地址
        IPAddress agent_ip = IPAddress(192, 168, 4, 1);
        // micro-ros agent udp端口
        uint16_t agent_port = 8888;
        // WiFi掉线后重新调用WiFi.begin()的最小毫秒间隔
        uint32_t wifi_reconnect_period = 5000;
        // 两次Agent ping之间的最小毫秒间隔
        uint32_t agent_ping_period = 1000;
        // 单次Agent ping的毫秒超时
        uint32_t agent_ping_timeout = 250;
        // 每轮Agent检查执行的ping次数
        uint8_t agent_ping_attempts = 1;
        // WiFi连上后等待链路稳定的毫秒时长
        uint32_t wifi_settle_delay = 1500;
        // 创建ROS实体前所需的连续ping成功次数
        uint8_t agent_required_successes = 2;
        // 最近成功ping后仍暂时保留Agent在线状态的毫秒时长
        uint32_t agent_lost_timeout = 3500;
        // ROS实体创建失败后的毫秒重试间隔
        uint32_t ros_init_retry_period = 2000;
        // 触发ROS实体重建的连续发布失败次数上限
        uint8_t ros_publish_failure_limit = 50;

        // 左电机方向/PWM A引脚
        int motor_left_pin_a = 22;
        // 左电机方向/PWM B引脚
        int motor_left_pin_b = 23;
        // 右电机方向/PWM A引脚
        int motor_right_pin_a = 12;
        // 右电机方向/PWM B引脚
        int motor_right_pin_b = 13;
        // TB6612电机载波频率；20kHz高于主要可听频段且低于芯片100kHz上限
        uint32_t motor_pwm_frequency_hz = 20000;

        // 左编码器A相引脚
        int left_encoder_pin_a = 32;
        // 左编码器B相引脚
        int left_encoder_pin_b = 33;
        // 右编码器A相引脚
        int right_encoder_pin_a = 26;
        // 右编码器B相引脚
        int right_encoder_pin_b = 25;

        // MPU6050 I2C SDA引脚
        int imu_sda_pin = 18;
        // MPU6050 I2C SCL引脚
        int imu_scl_pin = 19;

        // 是否启用OLED本机状态显示
        bool oled_enabled = true;
        // OLED I2C SDA引脚
        int oled_sda_pin = 18;
        // OLED I2C SCL引脚
        int oled_scl_pin = 19;
        // OLED I2C地址
        uint8_t oled_i2c_address = 0x3C;
        // OLED两次刷新之间的最小毫秒间隔
        uint32_t oled_update_period = 500;
        // Web文本在OLED上保持有效的毫秒时长
        uint32_t oled_message_timeout = 1500;

        // HC-SR04触发引脚
        int ultrasonic_trig_pin = 27;
        // HC-SR04回波引脚
        int ultrasonic_echo_pin = 21;
        // pulseIn等待回波的微秒超时
        uint32_t ultrasonic_timeout = 25000;
        // 进入前方近障锁存的距离
        float obstacle_stop_distance = 0.10f;
        // 解除前方近障锁存的距离
        float obstacle_clear_distance = 0.15f;

        // 左右驱动轮中心距离
        float wheel_distance = 0.175f;
        // 左轮编码器tick对应的轮端行驶距离
        float left_wheel_per_tick = 0.0001039203f;
        // 右轮编码器tick对应的轮端行驶距离
        float right_wheel_per_tick = 0.0001033942f;

        // ESP32接收的最大线速度，单位米每秒
        float max_linear_speed = 0.4f;
        // 线速度不大于该值时视为原地转向，单位米每秒
        float in_place_linear_threshold = 0.005f;
        // ESP32接收的最小非零原地角速度，单位弧度每秒
        float min_angular_speed = 0.523598776f;
        // ESP32接收的最大角速度，单位弧度每秒
        float max_angular_speed = 2.094395102f;
        // 差速换算后的最大单轮目标速度，单位米每秒
        float max_target_wheel_speed = 0.4f;
        // 左轮归一化静摩擦前馈
        float left_feedforward_static = 0.40f;
        // 右轮归一化静摩擦前馈
        float right_feedforward_static = 0.40f;
        // 左轮归一化速度前馈，单位秒每米
        float left_feedforward_velocity = 1.50f;
        // 右轮归一化速度前馈，单位秒每米
        float right_feedforward_velocity = 1.50f;
        // 是否在轮速前馈基础上启用双轮PID修正
        bool pid_enabled = true;
        // 归一化轮速误差的比例系数
        float pid_kp = 0.45f;
        // 归一化轮速误差的积分系数
        float pid_ki = 1.20f;
        // 编码器轮速不适合直接微分，默认关闭D项
        float pid_kd = 0.0f;
        // PID积分项允许贡献的最大归一化输出
        float pid_integral_output_limit = 0.40f;
        // PID对前馈输出的最大归一化修正
        float pid_output_limit = 0.40f;
        // 停止到运动或换向后的公共目标轮速爬升时长；0表示关闭
        uint32_t motor_soft_start_duration_ms = 100;
        // 固定轮速控制周期，单位毫秒
        uint32_t wheel_control_period = 50;
        // 编码器轮速低通滤波系数，范围0到1
        float wheel_speed_filter_alpha = 0.35f;
        // 可接受的最大瞬时编码器轮速
        float max_valid_wheel_speed = 0.8f;
        // 连续无效编码器样本达到该值后报告故障
        uint8_t encoder_invalid_num_limit = 3;
        // 故障后恢复编码器健康状态所需的连续有效数
        uint8_t encoder_recovery_num_limit = 20;

        // 未收到运动命令后本地停车的毫秒超时
        uint32_t command_timeout = 750;
        // MCU状态发布的毫秒周期
        uint32_t mcu_publish_period = 50;
        // IMU消息发布的毫秒周期
        uint32_t imu_publish_period = 20;
        // 超声波读取的毫秒周期
        uint32_t ultrasonic_read_period = 100;

    };

}


#endif
