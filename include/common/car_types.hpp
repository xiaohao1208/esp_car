#ifndef CAR_TYPES_HPP_
#define CAR_TYPES_HPP_

#include <stdint.h>
#include <IPAddress.h>
#include "config/car_config.hpp"

namespace car {
    // /car/mcu_status.fault_bits 的稳定通信契约。bit7 只是单次测距无回波
    // 警告；bit8 是必须由运动安全链保留的障碍物锁存。
    constexpr uint32_t MCU_FAULT_WIFI_DISCONNECTED = (1u << 0);
    constexpr uint32_t MCU_FAULT_AGENT_DISCONNECTED = (1u << 1);
    constexpr uint32_t MCU_FAULT_IMU_INVALID = (1u << 2);
    constexpr uint32_t MCU_FAULT_ENCODER_INVALID = (1u << 3);
    constexpr uint32_t MCU_FAULT_COMMAND_TIMEOUT = (1u << 4);
    constexpr uint32_t MCU_FAULT_MOTOR_DRIVER_INVALID = (1u << 5);
    constexpr uint32_t MCU_WARNING_ULTRASONIC_NO_ECHO = (1u << 7);
    constexpr uint32_t MCU_FAULT_OBSTACLE_LATCHED = (1u << 8);

    // Config使用全局默认配置
    struct Config : public ConfigDefaults {};

    struct IMU
    {
        // 本次IMU读数是否有效
        bool ok = false;
        // X轴线加速度
        float linear_x = 0.0f;
        // Y轴线加速度
        float linear_y = 0.0f;
        // Z轴线加速度
        float linear_z = 0.0f;
        // X轴角速度
        float angular_x = 0.0f;
        // Y轴角速度
        float angular_y = 0.0f;
        // Z轴角速度
        float angular_z = 0.0f;
    };
    struct Ultrasonic
    {
        // 本次超声波测距是否成功
        bool ok = false;
        // 是否检测到前方近障
        bool obstacle_ok = false;
        // 测得的距离，单位米
        float distance = 0.0f;
    };
    struct MotionState
    {
        // 目标线速度，单位米/秒，来自/cmd_vel_move
        float target_linear = 0.0f;
        // 目标角速度，单位弧度/秒，来自/cmd_vel_move
        float target_angular = 0.0f;
        // 差速运动学计算得到的左轮目标速度
        float target_left = 0.0f;
        // 差速运动学计算得到的右轮目标速度
        float target_right = 0.0f;
        // 编码器估算的左轮实际速度
        float actual_left = 0.0f;
        // 编码器估算的右轮实际速度
        float actual_right = 0.0f;
        // 左电机最终输出占空比百分比，范围[-100, 100]
        float output_left = 0.0f;
        // 右电机最终输出占空比百分比，范围[-100, 100]
        float output_right = 0.0f;
        // 左轮编码器累计tick数
        int32_t left_ticks = 0;
        // 右轮编码器累计tick数
        int32_t right_ticks = 0;
        // 是否因为长时间未收到/cmd_vel_move而进入安全停机状态
        bool cmd_timeout_ok = true;
        // 是否因为前方近障而进入安全停机状态
        bool obstacle_ok = false;
        // 编码器初始化或连续采样是否可信
        bool encoder_ok = false;
        // 电机驱动对象是否初始化成功
        bool motor_driver_ok = false;
    };
    struct McuStatus
    {
        // 用来复制到car_interfaces/CarMcuStatus，并发布给jetson
        // wifi是否连接到jetson热点
        bool wifi_connect_ok;
        // micro-ros的agent是否连接
        bool agent_connect_ok;
        // imu是否完成初始化且当前读数有效
        bool imu_ok;
        // 编码器是否完成初始化且当前轮速计算有效
        bool encoder_ok;
        // 电机驱动是否完成初始化
        bool motor_driver_ok;
        // esp32是否因为长时间没有收到/cmd_vel_move而停车（/cmd_vel_move是esp32接收唯一移动话题）
        bool cmd_timeout_ok;
        // 左轮编码器估算速度，单位m/s
        float left_wheel_speed;
        // 右轮编码器估算速度，单位m/s
        float right_wheel_speed;
        // 左轮累计编码器tick值，可以使用相邻消息差值计算位移（不空转）
        int32_t left_encoder_ticks;
        // 右轮累计编码器tick值，可以使用相邻消息差值计算位移（不空转）
        int32_t right_encoder_ticks;
        // 超声波本次测距是否有效；无回波可能只是超出量程，不等同硬件故障
        bool ultrasonic_ok;
        // 前方是否有障碍物
        bool obstacle_ok;
        // 前方障碍物距离，单位m
        float obstacle_distance;
        // 状态位图：bit7 是无回波警告，bit8 是障碍物锁存，其余低位为故障
        uint32_t fault_bits;
    };


}

#endif
