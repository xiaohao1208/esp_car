#pragma once

namespace car {

    class WheelPidController {
        public:
            // 创建参数和历史状态均为零的轮速 PID 控制器
            WheelPidController();
            // 配置归一化轮速误差到归一化电机修正量的 PID 参数和限幅
            void configure(float kp,
                           float ki,
                           float kd,
                           float integral_output_limit,
                           float output_limit);
            // 重置积分项和历史误差
            void reset();
            // 根据目标、反馈、周期和可用输出区间计算带抗积分饱和的 PID 修正量
            float update(float target_speed,
                         float actual_speed,
                         float elapsed,
                         float lower_output,
                         float upper_output);

        private:
            // 归一化轮速误差比例系数
            float kp_;
            // 归一化轮速误差积分系数
            float ki_;
            // 归一化轮速误差微分系数
            float kd_;
            // 积分项对应的归一化输出上限
            float integral_output_limit_;
            // PID归一化修正输出上限
            float output_limit_;
            // 当前轮速误差积分
            float integral_;
            // 上一次轮速误差
            float previous_error_;
            // 第一帧不计算微分项
            bool have_previous_error_;
    };

}
