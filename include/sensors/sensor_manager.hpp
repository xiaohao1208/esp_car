#pragma once

#include <MPU6050_light.h>
#include <Wire.h>
#include "common/car_types.hpp"

namespace car {

    class SensorManager {
    public:
        /**
         * @brief 保存传感器配置并初始化未就绪状态
         *
         * @param config 全局只读配置，生命周期必须长于本管理器
         */
        explicit SensorManager(const Config& config);

        /**
         * @brief 初始化 I2C、MPU6050 和超声波 GPIO
         *
         * MPU6050 初始化成功后在小车静止状态完成偏置校准
         */
        void initialize();

        /**
         * @brief 读取三轴线加速度和角速度
         *
         * @return 包含 SI 单位测量值和有效标志的 IMU 快照
         */
        IMU read_imu();

        /**
         * @brief 触发一次超声波测距并更新双阈值障碍锁存
         *
         * @return 包含米制距离、测量有效性和前方通行状态的快照
         */
        Ultrasonic read_ultrasonic();

    private:
        // 编译器硬件参数引用，包含I2C和超声波GPIO
        const Config& config_;
        // MPU驱动实例，初始时完成静止偏置校准
        MPU6050 mpu_;
        // IMU初始化是否成功，失败时/imu/data不发布有效数据
        bool imu_ok_;
        // 超声波是否遇到障碍，双阈值避免临界距离抖动
        bool obstacle_ok_;
    };

}
