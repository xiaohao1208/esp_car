#pragma once

#include <cmath>

namespace car {

    /**
     * @brief 跟踪停止到运动及换向事件，并生成公共目标轮速斜坡
     *
     * 左右轮共用同一个比例，因而不会改变差速目标原有的曲率。unsigned long
     * 的减法可正确处理 Arduino millis() 的回绕。
     */
    class MotorSoftStart {
    public:
        explicit MotorSoftStart(unsigned long duration_ms)
        : duration_ms_(duration_ms),
          started_tick_(0),
          active_(false) {}

        /**
         * @return true 表示本次需要先把实际电机输出立即归零
         */
        bool update_target(float previous_left,
                           float previous_right,
                           float target_left,
                           float target_right,
                           unsigned long now_tick) {
            if (is_stopped(target_left, target_right)) {
                reset();
                return false;
            }

            const bool starting = is_stopped(previous_left, previous_right);
            const bool reversing =
                direction_reversed(previous_left, target_left) ||
                direction_reversed(previous_right, target_right);
            if (!starting && !reversing) {
                return false;
            }

            started_tick_ = now_tick;
            active_ = duration_ms_ > 0;
            return true;
        }

        float scale(unsigned long now_tick) {
            if (!active_) {
                return 1.0f;
            }
            const unsigned long elapsed = now_tick - started_tick_;
            if (elapsed >= duration_ms_) {
                active_ = false;
                return 1.0f;
            }
            return static_cast<float>(elapsed) /
                static_cast<float>(duration_ms_);
        }

        void reset() {
            started_tick_ = 0;
            active_ = false;
        }

        bool active() const {
            return active_;
        }

        static float scale_target(float target, float progress) {
            const float limited_progress = progress < 0.0f
                ? 0.0f
                : (progress > 1.0f ? 1.0f : progress);
            return target * limited_progress;
        }

    private:
        static bool is_stopped(float left, float right) {
            constexpr float epsilon = 0.001f;
            return std::fabs(left) < epsilon && std::fabs(right) < epsilon;
        }

        static bool direction_reversed(float previous, float target) {
            return previous * target < 0.0f;
        }

        unsigned long duration_ms_;
        unsigned long started_tick_;
        bool active_;
    };

}
