#pragma once

#include "common/car_types.hpp"
#include "control/motion_controller.hpp"
#include "display/oled_display_manager.hpp"
#include "ros/ros_entity_manager.hpp"
#include "sensors/sensor_manager.hpp"
#include "connect/wifi_connect_manager.hpp"

namespace car {

    class CarApp {
    public:
        /**
         * @brief 构造 CarApp 应用对象
         *
         * 初始化底部的各个子管理器（网络、电机控制、传感器、OLED显示等），
         * 并重置运行时间戳、Agent 状态历史记录及 ROS 失败计数器
         */
        CarApp();

        /**
         * @brief 应用总初始化入口
         *
         * 在 Arduino 的 setup() 中调用。按照“硬件传感器/电机 -> WiFi/网络传输 -> ROS实体”的严格顺序执行初始化
         */
        void setup();

        /**
         * @brief Arduino 主循环主控函数
         *
         * 在 Arduino 的 loop() 中持续调用。采用协作式非阻塞调度，依次执行网络维持、Agent状态更新、
         * ROS实体状态维护与 spin、电机闭环控制更新、周期性话题发布（IMU/MCU状态）以及 OLED 屏幕刷新
         */
        void loop();
    private:
        // setup()按硬件、网络连接、ros实体三段初始化
        /**
         * @brief 初始化硬件设备与传感器
         *
         * 启动传感器管理器（MPU/超声波）、OLED 显示管理器（共享 I2C 引脚），
         * 并初始化电机驱动硬件（立即停止电机输出，防止上电产生随机 PWM 抖动）
         */
        void init_hardware();

        /**
         * @brief 初始化网络与连接回调
         *
         * 启动阻塞/非阻塞 WiFi 连接逻辑，同时向 RosEntityManager 注册 OLED 显示文本的回调函数
         */
        void init_connect();

        /**
         * @brief 尝试创建并初始化 ROS2 实体（带时间节流与前置条件校验）
         *
         * 在 WiFi 与 micro-ROS Agent 均稳定连通的前提下，创建节点、订阅者和发布者实体
         * 内部具备重试节流机制（ros_init_retry_period），避免高频重复创建销毁造成 CPU 阻塞
         *
         * @return true  ROS 实体初始化成功
         * @return false 条件未满足或 RCL 初始化失败
         */
        bool init_ros_entities();

        /**
         * @brief 维护 ROS 实体的生命周期与重连状态机
         *
         * 监听 Agent 连接状态的变化。若检测到 Agent 从已连接变为断开，触发实体销毁，
         * 若处于未初始化状态，则节流尝试重新构建 ROS 实体
         */
        void maintain_ros_entities();

        /**
         * @brief 重置并销毁当前已创建的 ROS 实体
         *
         * 用于 Agent 断开、连续发布失败或手动重置场景。安全销毁 ROS 句柄（内部会自动停止电机），
         * 清空 OLED 显示的远端过期消息，并重置 ROS 错误计数
         */
        void reset_ros_entities();

        // 周期发布集中在一个函数里，保证arduino loop尽快返回，电机超时保护不被阻塞
        /**
         * @brief 采样传感器并周期性发布 ROS2 话题数据
         *
         * 1. 实时读取 IMU 传感器，
         * 2. 构建并按配置周期发布 /imu/data 与 /car/mcu_status 话题，
         * 3. 发布后按单独周期读取超声波并同步给电机控制器，避免阻塞心跳，
         * 4. 统计连续发布失败次数，若超过限制（ros_publish_failure_limit）则自动触发 ROS 实体重建
         */
        void publish_periodic_topics();

        /**
         * @brief 聚合当前传感器与控制器数据，构建 MCU 综合状态结构体
         *
         * 收集 WiFi/Agent 连接状态、IMU/编码器/电机驱动/超声波状态及实测车速，
         * 并通过位运算按契约生成用于上位机快速故障定位的 fault_bits 位图
         *
         * @param imu       当前的 IMU 采样数据
         * @param ultrasonic 当前的超声波采样数据
         * @return McuStatus 组装完成的 MCU 综合状态结构体
         */
        McuStatus build_status(const IMU& imu, const Ultrasonic ultrasonic) const;

        /**
         * @brief 触发 OLED 屏幕绘制与刷新
         *
         * 收集当前连接状态、ROS就绪状态及最新的 MCU 状态传给 display_manager_
         * 实际刷新频率由显示管理器内部按配置时间节流
         */
        void update_display();

        /**
         * @brief 接收远端 OLED 显示文本的静态回调中转函数
         *
         * 由 RosEntityManager 触发。将 Context 指针强转为 CarApp 实例，
         * 并将收到来自 Jetson/Web 的状态文本下发给 display_manager_ 进行储存和显示
         *
         * @param context 用户上下文指针（指向当前的 CarApp 实例）
         * @param message 接收到的显示字符串
         */
        static void handle_display_message(void* context, const char* message);

        // 全局配置
        Config config_;
        // wifi和micro-ros连接管理
        WiFiConnectManager connect_manager_;
        // 电机、编码器、差速运动学和底层停车保护
        MotionController motion_controller_;
        // IMU、超声波
        SensorManager sensor_manager_;
        // 0.96寸oled屏幕
        OledDisplayManager display_manager_;
        // micro-ros管理
        RosEntityManager ros_manager_;
        // 最近一次发布/car/mcu_status的arduino毫秒tick
        unsigned long last_mcu_publish_tick_;
        // 最近一次发布/imu/data的arduino毫秒tick
        unsigned long last_imu_publish_tick_;
        // 最近一次读取超声波的arduino毫秒tick
        unsigned long last_ultrasonic_read_tick_;
        // 最近一次尝试创建ros实体的arduino毫秒tick
        unsigned long last_ros_init_attempt_tick_;
        // 上一轮agent连接状态，用于检测断开边沿并销毁ros实体
        bool previous_agent_connected_;
        // 连续ros发布、执行失败计数，超过阈值后重建ros实体
        uint8_t ros_failure_count_;
        // 最近一次IMU采样
        IMU last_imu_;
        // 最近一次超声波采样
        Ultrasonic last_ultrasonic_;
        // 最近一次MCU状态
        McuStatus last_mcu_status_;
    };

}
