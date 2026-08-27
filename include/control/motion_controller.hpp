#pragma once
#include <Esp32McpwmMotor.h>
#include <Esp32PcntEncoder.h>
#include "common/car_types.hpp"
#include "control/motor_soft_start.hpp"
#include "control/wheel_pid_controller.hpp"

namespace car {

    class MotionController {
    public:
        /**
         * @brief 保存底盘配置并初始化尚未启用的控制状态
         *
         * @param config 全局只读配置，生命周期必须长于本控制器
         */
        explicit MotionController(const Config &config);

        /**
         * @brief 初始化双路电机、编码器和轮速 PID
         *
         * 初始化结束后立即保持零输出，避免 ESP32 上电时电机误动作
         */
        void initialize_hardware();

        /**
         * @brief 保存底盘目标线速度和角速度
         *
         * 输入会按配置限幅，角速度单位为弧度每秒，随后由差速运动学换算为左右轮目标速度
         *
         * @param linear_speed 目标线速度，单位米每秒
         * @param angular_speed 目标角速度，单位弧度每秒
         */
        void set_target_twist(float linear_speed, float angular_speed);

        /**
         * @brief 更新前方通行状态
         *
         * true 表示前方存在近障，控制器立即将前进目标和输出归零
         *
         * @param obstacle_ok true 表示检测到近障，false 表示前方未锁存近障
         */
        void set_obstacle_state(bool obstacle_ok);

        /**
         * @brief 执行一次非阻塞轮速控制更新
         *
         * 依次检查命令超时和障碍状态，再按固定周期读取编码器、滤波轮速并输出前馈加 PID 修正
         */
        void update();

        /**
         * @brief 将目标、PID 状态和电机输出全部归零
         */
        void stop_motors();

        /**
         * @brief 返回当前目标速度、轮速、编码器和故障状态快照
         *
         * @return 当前 MotionState 的值副本
         */
        MotionState get_motion_state() const;

    private:
        // update顺序固定，检查安全状态后按固定周期读取编码器和输出电机
        // 根据差速逆运动学把目标线速度和角速度转化为左右轮目标轮速
        void compute_wheel_targets();
        // 根据固定周期内的编码器增量更新滤波后的实际轮速
        bool update_encoder_state(unsigned long now_tick);
        // 计算得到输出百分比并应用到电机上
        void apply_motor_output();
        // 使用全局配置初始化左右轮 PID 参数
        void configure_pid_controllers();
        // 同时重置左右轮 PID 积分和历史误差
        void reset_pid_controllers();
        // 只把驱动输出归零，不改变当前速度目标
        void zero_motor_outputs();
        // 根据目标轮速、实际轮速、前馈和 PID 得到归一化电机输出
        float wheel_speed_to_motor_output(float target_speed,
                                          float actual_speed,
                                          float static_feedforward,
                                          float velocity_feedforward,
                                          WheelPidController &pid_controller,
                                          float static_feedforward_scale);
        // 处理命令超时，长时间未收到速度命令时将目标速度归零
        void handle_cmd_timeout();

        // 编译期配置参数
        const Config &config_;
        // 电机驱动对象，输入范围为[-100, 100]占空比百分比
        Esp32McpwmMotor motor_;
        // 两个编码器
        Esp32PcntEncoder encoders_[2];
        // 两个pid控制器
        WheelPidController wheel_pid_[2];
        // 左右轮共用的停止起步/换向目标轮速斜坡
        MotorSoftStart motor_soft_start_;
        // 当前目标速度、实际速度、编码器tick值等
        MotionState motion_state_;
        // 最近一次控制循环的arduino毫秒tick
        unsigned long last_update_tick_;
        // 最近一次收到/cmd_vel_move的arduino毫秒tick
        unsigned long last_cmd_tick_;
        // 最近一次控制循环的时间间隔
        float last_control_interval_;
        // 两路编码器是否都成功完成硬件初始化
        bool encoders_initialized_;
        // 左右轮滤波状态是否已经取得第一帧有效样本
        bool wheel_filter_initialized_[2];
        // 当前连续无效编码器数量
        uint8_t encoder_invalid_nums_;
        // 编码器故障后的连续有效恢复数量
        uint8_t encoder_valid_nums_;
    };

}
