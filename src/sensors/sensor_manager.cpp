#include "sensors/sensor_manager.hpp"
#include <Arduino.h>

namespace car {

    namespace {
        // MPU的g值转换成标准线加速度所用的重力常数
        constexpr float kGravity = 9.80665f;
        // MPU的角速度单位是deg/s，需要转换成ros标准rad/s
        const float kDegreesToRadians = DEG_TO_RAD;
    }

    SensorManager::SensorManager(const Config & config)
    : config_(config),
      mpu_(Wire),
      imu_ok_(false),
      obstacle_ok_(false){}

    void SensorManager::initialize() {
        // MPU安装在ESP32板上，I2C引脚显式来自配置，避免依赖Arduino默认引脚
        Wire.begin(config_.imu_sda_pin, config_.imu_scl_pin);
        pinMode(config_.ultrasonic_trig_pin, OUTPUT);
        pinMode(config_.ultrasonic_echo_pin, INPUT);
        digitalWrite(config_.ultrasonic_trig_pin, LOW);
        // MPU返回的硬件初始化状态码
        const byte status = mpu_.begin();
        imu_ok_ = status == 0;
        if (imu_ok_) {
            // 上电校准要求小车静止，同时校准陀螺仪和加速度计偏置
            mpu_.calcOffsets(true, true);
        }
    }

    IMU SensorManager::read_imu() {
        // 默认无效的IMU样本，初始化失败时直接返回
        IMU imu;
        if (!imu_ok_) {
            return imu;
        }
        // 更新MPU传感器数据
        mpu_.update();
        imu.ok = true;
        // MPU返回deg/s和g，ROS的IMU需要rad/s和m/s^2
        imu.angular_x = mpu_.getGyroX() * kDegreesToRadians;
        imu.angular_y = mpu_.getGyroY() * kDegreesToRadians;
        imu.angular_z = mpu_.getGyroZ() * kDegreesToRadians;
        imu.linear_x = mpu_.getAccX() * kGravity;
        imu.linear_y = mpu_.getAccY() * kGravity;
        imu.linear_z = mpu_.getAccZ() * kGravity;
        return imu;
    }

    Ultrasonic SensorManager::read_ultrasonic() {
        // 默认无效的超声波样本，回波超时时保留安全锁存状态
        Ultrasonic ultrasonic;
        // 标准触发：先拉低稳定，再给10us高电平，然后测echo脉宽
        digitalWrite(config_.ultrasonic_trig_pin, LOW);
        delayMicroseconds(2);
        digitalWrite(config_.ultrasonic_trig_pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(config_.ultrasonic_trig_pin, LOW);
        // Echo高电平持续时间，单位微秒
        const unsigned long echo_duration =
            pulseIn(config_.ultrasonic_echo_pin, HIGH, config_.ultrasonic_timeout);
        if (echo_duration == 0) {
            // 超时时保留上一次近障锁存状态，比直接清除障碍更安全
            ultrasonic.obstacle_ok = obstacle_ok_;
            return ultrasonic;
        }
        ultrasonic.ok = true;
        ultrasonic.distance = (static_cast<float>(echo_duration) * 0.000343f) * 0.5f;
        // 带滞回的近障判断：低于停车距离锁存，高于解除距离才释放，避免边界抖动
        if (ultrasonic.distance <= config_.obstacle_stop_distance) {
            obstacle_ok_ = true;
        } else if (ultrasonic.distance >= config_.obstacle_clear_distance) {
            obstacle_ok_ = false;
        }
        ultrasonic.obstacle_ok = obstacle_ok_;
        return ultrasonic;
    }

}