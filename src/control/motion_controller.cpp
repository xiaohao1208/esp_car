#include "control/motion_controller.hpp"

#include <Arduino.h>
#include <algorithm>
#include <cmath>
#include <driver/mcpwm.h>

namespace {

float clamp_float(float value, float minimum, float maximum) {
    return std::min(std::max(value, minimum), maximum);
}

bool pwm_frequency_matches(uint32_t requested, uint32_t actual) {
    const uint32_t tolerance = std::max<uint32_t>(1U, requested / 100U);
    const uint32_t difference = requested > actual
        ? requested - actual
        : actual - requested;
    return difference <= tolerance;
}

bool configure_motor_pwm_frequency(uint32_t frequency_hz) {
    // FishBot二驱板使用TB6612；限制仍保守地保持在芯片100kHz规格以内。
    if (frequency_hz < 1000U || frequency_hz > 100000U) {
        return false;
    }
    const esp_err_t left_result = mcpwm_set_frequency(
        MCPWM_UNIT_0, MCPWM_TIMER_0, frequency_hz);
    const esp_err_t right_result = mcpwm_set_frequency(
        MCPWM_UNIT_0, MCPWM_TIMER_1, frequency_hz);
    if (left_result != ESP_OK || right_result != ESP_OK) {
        return false;
    }
    return pwm_frequency_matches(
        frequency_hz,
        mcpwm_get_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0)) &&
        pwm_frequency_matches(
        frequency_hz,
        mcpwm_get_frequency(MCPWM_UNIT_0, MCPWM_TIMER_1));
}

}

namespace car
{

    MotionController::MotionController(const Config &config)
    : config_(config),
      motor_soft_start_(config.motor_soft_start_duration_ms),
      last_update_tick_(0),
      last_cmd_tick_(0),
      last_control_interval_(0.0f),
      encoders_initialized_(false),
      wheel_filter_initialized_{false, false},
      encoder_invalid_nums_(0),
      encoder_valid_nums_(0) {}

    void MotionController::initialize_hardware() {
        // 固件约定电机通道0为左轮，通道1为右轮
        motor_.attachMotor(0, config_.motor_left_pin_a, config_.motor_left_pin_b);
        motor_.attachMotor(1, config_.motor_right_pin_a, config_.motor_right_pin_b);
        const bool motor_pwm_initialized = configure_motor_pwm_frequency(
            config_.motor_pwm_frequency_hz);
        const bool left_encoder_initialized =
            encoders_[0].init(
                0,
                config_.left_encoder_pin_a,
                config_.left_encoder_pin_b);
        const bool right_encoder_initialized =
            encoders_[1].init(
                1,
                config_.right_encoder_pin_a,
                config_.right_encoder_pin_b);
        encoders_initialized_ =
            left_encoder_initialized && right_encoder_initialized;
        motion_state_.encoder_ok = encoders_initialized_;
        motion_state_.motor_driver_ok = motor_pwm_initialized;
        if (encoders_initialized_) {
            motion_state_.left_ticks = encoders_[0].getTicks();
            motion_state_.right_ticks = encoders_[1].getTicks();
        }
        configure_pid_controllers();
        stop_motors();
        if (!motor_pwm_initialized) {
            Serial.printf(
                "电机PWM频率初始化失败，请检查配置值：%lu Hz\n",
                static_cast<unsigned long>(config_.motor_pwm_frequency_hz));
        } else {
            Serial.printf(
                "电机PWM频率已设置为%lu Hz\n",
                static_cast<unsigned long>(config_.motor_pwm_frequency_hz));
        }
        last_update_tick_ = millis();
        last_cmd_tick_ = millis();
    }

    void MotionController::set_target_twist(
     float linear_speed,
     float angular_speed) {
        const float previous_left = motion_state_.target_left;
        const float previous_right = motion_state_.target_right;
        const float finite_linear = isfinite(linear_speed) ? linear_speed : 0.0f;
        const float finite_angular =
            isfinite(angular_speed) ? angular_speed : 0.0f;
        motion_state_.target_linear = clamp_float(
            finite_linear,
            -config_.max_linear_speed,
            config_.max_linear_speed);
        const float limited_angular = clamp_float(
            finite_angular,
            -config_.max_angular_speed,
            config_.max_angular_speed);
        const bool in_place =
            fabsf(motion_state_.target_linear) <=
            std::max(0.0f, config_.in_place_linear_threshold);
        motion_state_.target_angular =
            limited_angular == 0.0f || !in_place
            ? limited_angular
            : copysignf(
                std::max(
                    fabsf(limited_angular),
                    std::min(
                        config_.min_angular_speed,
                        config_.max_angular_speed)),
                limited_angular);
        last_cmd_tick_ = millis();
        motion_state_.cmd_timeout_ok = false;
        compute_wheel_targets();

        if (fabsf(motion_state_.target_left) < 0.001f &&
            fabsf(motion_state_.target_right) < 0.001f) {
            stop_motors();
            return;
        }

        // 停止后起步或任一车轮换向时，先立即归零，再让双轮目标按同一比例爬升。
        // 连续收到同方向非零命令不会重启斜坡。
        if (motor_soft_start_.update_target(
                previous_left,
                previous_right,
                motion_state_.target_left,
                motion_state_.target_right,
                last_cmd_tick_)) {
            reset_pid_controllers();
            zero_motor_outputs();
        }
    }

    void MotionController::set_obstacle_state(bool obstacle_ok) {
        // ESP32本地近障判断不依赖Jetson网络时延
        motion_state_.obstacle_ok = obstacle_ok;
        if (obstacle_ok && motion_state_.target_linear > 0.0f) {
            stop_motors();
        }
    }

    void MotionController::update() {
        handle_cmd_timeout();
        if (motion_state_.cmd_timeout_ok) {
            return;
        }
        if (!encoders_initialized_) {
            motion_state_.encoder_ok = false;
            stop_motors();
            return;
        }
        if (!motion_state_.motor_driver_ok) {
            stop_motors();
            return;
        }

        const unsigned long now_tick = millis();
        if (now_tick - last_update_tick_ < config_.wheel_control_period) {
            return;
        }
        if (!update_encoder_state(now_tick)) {
            if (!motion_state_.encoder_ok) {
                stop_motors();
            }
            return;
        }
        apply_motor_output();
    }

    void MotionController::stop_motors() {
        // 清除目标、输出和PID状态，确保任意安全入口都能完全停车
        motion_state_.target_linear = 0.0f;
        motion_state_.target_angular = 0.0f;
        motion_state_.target_left = 0.0f;
        motion_state_.target_right = 0.0f;
        motor_soft_start_.reset();
        reset_pid_controllers();
        zero_motor_outputs();
    }

    void MotionController::zero_motor_outputs() {
        motion_state_.output_left = 0.0f;
        motion_state_.output_right = 0.0f;
        motor_.updateMotorSpeed(0, 0);
        motor_.updateMotorSpeed(1, 0);
    }

    MotionState MotionController::get_motion_state() const {
        return motion_state_;
    }

    void MotionController::compute_wheel_targets() {
        // 差速逆运动学，left=v-omega*track/2，right=v+omega*track/2
        motion_state_.target_left =
            motion_state_.target_linear -
            motion_state_.target_angular * config_.wheel_distance * 0.5f;
        motion_state_.target_right =
            motion_state_.target_linear +
            motion_state_.target_angular * config_.wheel_distance * 0.5f;

        // 同比例缩放双轮目标，限制轮速同时保持原有曲率
        const float largest = std::max(
            fabsf(motion_state_.target_left),
            fabsf(motion_state_.target_right));
        if (largest > config_.max_target_wheel_speed) {
            const float scale = config_.max_target_wheel_speed / largest;
            motion_state_.target_left *= scale;
            motion_state_.target_right *= scale;
        }
    }

    void MotionController::configure_pid_controllers() {
        for (auto &pid_controller : wheel_pid_) {
            pid_controller.configure(
                config_.pid_kp,
                config_.pid_ki,
                config_.pid_kd,
                config_.pid_integral_output_limit,
                config_.pid_output_limit);
        }
    }

    void MotionController::reset_pid_controllers() {
        for (auto &pid_controller : wheel_pid_) {
            pid_controller.reset();
        }
    }

    void MotionController::handle_cmd_timeout() {
        if(millis() - last_cmd_tick_ <= config_.command_timeout ||
           motion_state_.cmd_timeout_ok) {
            return;
        }
        stop_motors();
        motion_state_.cmd_timeout_ok = true;
    }

    bool MotionController::update_encoder_state(unsigned long now_tick) {
        const float control_interval =
            static_cast<float>(now_tick - last_update_tick_) / 1000.0f;
        const int32_t left_ticks = encoders_[0].getTicks();
        const int32_t right_ticks = encoders_[1].getTicks();
        const int32_t delta_left =
            left_ticks - motion_state_.left_ticks;
        const int32_t delta_right =
            right_ticks - motion_state_.right_ticks;
        motion_state_.left_ticks = left_ticks;
        motion_state_.right_ticks = right_ticks;
        last_control_interval_ = control_interval;
        last_update_tick_ = now_tick;

        const float measured_left =
            delta_left * config_.left_wheel_per_tick / control_interval;
        const float measured_right =
            delta_right * config_.right_wheel_per_tick / control_interval;
        const bool plausible =
            isfinite(measured_left) &&
            isfinite(measured_right) &&
            fabsf(measured_left) <= config_.max_valid_wheel_speed &&
            fabsf(measured_right) <= config_.max_valid_wheel_speed;
        if (!plausible) {
            encoder_valid_nums_ = 0;
            if (encoder_invalid_nums_ < UINT8_MAX) {
                ++encoder_invalid_nums_;
            }
            if (encoder_invalid_nums_ >= config_.encoder_invalid_num_limit) {
                motion_state_.encoder_ok = false;
            }
            return false;
        }

        const float alpha = clamp_float(
            config_.wheel_speed_filter_alpha,
            0.0f,
            1.0f);
        if (!wheel_filter_initialized_[0]) {
            motion_state_.actual_left = measured_left;
            wheel_filter_initialized_[0] = true;
        } else {
            motion_state_.actual_left +=
                alpha * (measured_left - motion_state_.actual_left);
        }
        if (!wheel_filter_initialized_[1]) {
            motion_state_.actual_right = measured_right;
            wheel_filter_initialized_[1] = true;
        } else {
            motion_state_.actual_right +=
                alpha * (measured_right - motion_state_.actual_right);
        }

        encoder_invalid_nums_ = 0;
        if (encoder_valid_nums_ < UINT8_MAX) {
            ++encoder_valid_nums_;
        }
        if (encoder_valid_nums_ >= config_.encoder_recovery_num_limit) {
            motion_state_.encoder_ok = true;
        }
        return true;
    }

    float MotionController::wheel_speed_to_motor_output(
     float target_speed,
     float actual_speed,
     float static_feedforward,
     float velocity_feedforward,
     WheelPidController &pid_controller,
     float static_feedforward_scale) {
        constexpr float epsilon = 0.001f;
        if (fabsf(target_speed) < epsilon) {
            pid_controller.reset();
            return 0.0f;
        }

        const float direction = target_speed > 0.0f ? 1.0f : -1.0f;
        const float feedforward =
            direction * fabsf(static_feedforward) *
                clamp_float(static_feedforward_scale, 0.0f, 1.0f) +
            target_speed * fabsf(velocity_feedforward);
        const float limited_feedforward =
            clamp_float(feedforward, -1.0f, 1.0f);
        float correction = 0.0f;
        if (config_.pid_enabled) {
            const float speed_scale = std::max(
                config_.max_target_wheel_speed, 0.001f);
            correction = pid_controller.update(
                target_speed / speed_scale,
                actual_speed / speed_scale,
                last_control_interval_,
                -1.0f - limited_feedforward,
                1.0f - limited_feedforward);
        } else {
            pid_controller.reset();
        }
        return clamp_float(
            limited_feedforward + correction,
            -1.0f,
            1.0f);
    }

    void MotionController::apply_motor_output() {
        if (motion_state_.obstacle_ok &&
            motion_state_.target_linear > 0.0f) {
            stop_motors();
            return;
        }

        const float ramp_progress = motor_soft_start_.scale(millis());
        // 目标轮速、静摩擦前馈和速度前馈同步爬升；PID面对的是逐步增加的
        // 轮速目标，因此积分也会平缓建立，不会等斜坡结束后才开始起转。
        const float ramped_left_target = MotorSoftStart::scale_target(
            motion_state_.target_left, ramp_progress);
        const float ramped_right_target = MotorSoftStart::scale_target(
            motion_state_.target_right, ramp_progress);
        const float left_output = wheel_speed_to_motor_output(
            ramped_left_target,
            motion_state_.actual_left,
            config_.left_feedforward_static,
            config_.left_feedforward_velocity,
            wheel_pid_[0],
            ramp_progress);
        const float right_output = wheel_speed_to_motor_output(
            ramped_right_target,
            motion_state_.actual_right,
            config_.right_feedforward_static,
            config_.right_feedforward_velocity,
            wheel_pid_[1],
            ramp_progress);
        motion_state_.output_left = left_output * 100.0f;
        motion_state_.output_right = right_output * 100.0f;
        motor_.updateMotorSpeed(
            0,
            static_cast<int16_t>(lroundf(motion_state_.output_left)));
        motor_.updateMotorSpeed(
            1,
            static_cast<int16_t>(lroundf(motion_state_.output_right)));
    }

}
