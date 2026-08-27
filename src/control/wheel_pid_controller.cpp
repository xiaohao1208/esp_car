#include "control/wheel_pid_controller.hpp"

#include <algorithm>
#include <cmath>

namespace {

float clamp_float(float value, float minimum, float maximum) {
    return std::min(std::max(value, minimum), maximum);
}

}

namespace car {

    WheelPidController::WheelPidController()
    : kp_(0.0f),
      ki_(0.0f),
      kd_(0.0f),
      integral_output_limit_(0.0f),
      output_limit_(0.0f),
      integral_(0.0f),
      previous_error_(0.0f),
      have_previous_error_(false) {}

    void WheelPidController::configure(
     float kp,
     float ki,
     float kd,
     float integral_output_limit,
     float output_limit) {
        kp_ = kp;
        ki_ = ki;
        kd_ = kd;
        // 参数全部按绝对值限制，负号只由轮速误差决定
        integral_output_limit_ = fabsf(integral_output_limit);
        output_limit_ = fabsf(output_limit);
        reset();
    }

    void WheelPidController::reset() {
        // 停车或换向时清除历史误差，避免积分残留
        integral_ = 0.0f;
        previous_error_ = 0.0f;
        have_previous_error_ = false;
    }

    float WheelPidController::update(
     float target_speed,
     float actual_speed,
     float elapsed,
     float lower_output,
     float upper_output) {
        constexpr float epsilon = 0.001f;
        if(fabsf(target_speed) < epsilon || output_limit_ <= 0.0f) {
            reset();
            return 0.0f;
        }

        // loop可能被网络维护拖慢，限制参与控制计算的时间间隔
        const float interval = clamp_float(elapsed, 0.001f, 0.1f);
        const float error = target_speed - actual_speed;
        float derivative = 0.0f;
        if(have_previous_error_) {
            derivative = (error - previous_error_) / interval;
        }
        previous_error_ = error;
        have_previous_error_ = true;

        const float minimum = std::max(-output_limit_, lower_output);
        const float maximum = std::min(output_limit_, upper_output);
        const float candidate_integral = integral_ + error * interval;
        const float integral_output = clamp_float(
            ki_ * candidate_integral,
            -integral_output_limit_,
            integral_output_limit_);
        const float unsaturated =
            kp_ * error + integral_output + kd_ * derivative;
        const float output = clamp_float(unsaturated, minimum, maximum);

        // 未饱和或误差推动输出离开饱和区时才接受本次积分
        const bool can_integrate =
            unsaturated == output ||
            (unsaturated > maximum && error < 0.0f) ||
            (unsaturated < minimum && error > 0.0f);
        if (can_integrate) {
            if (fabsf(ki_) > epsilon) {
                integral_ = integral_output / ki_;
            } else {
                integral_ = 0.0f;
            }
        }
        return output;
    }

}
