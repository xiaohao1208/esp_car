#pragma once

#include <geometry_msgs/msg/twist.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <sensor_msgs/msg/imu.h>
#include <std_msgs/msg/string.h>
#include "car_interfaces/msg/car_mcu_status.h"
#include "common/car_types.hpp"

namespace car {

    class MotionController;

    class RosEntityManager {
    public:
        using DisplayMessageCallback = void (*)(void* context, const char* message);
        /**
         * @brief 构造 RosEntityManager 对象
         *
         * 初始化单例指针 instance_，清空/复位所有 ROS2 实体句柄（Support、Node、Executor、Subscribers、Publishers），
         * 并使用底层 rosidl C API 初始化各个 ROS2 消息结构体内存
         */
        RosEntityManager();

        /**
         * @brief 析构函数
         *
         * 自动调用 destroy() 函数释放并销毁所有分配的 ROS2 句柄与实体资源
         */
        ~RosEntityManager();

        // 创建所有rclc实体，MotionController通过指针注入，避免电机控制类直接依赖ros类型
        /**
         * @brief 初始化 micro-ROS 的所有 ROS2 实体
         *
         * 按照 RCL/RCLC 严格顺序依次创建 Support、Node、/cmd_vel_move 订阅器、/car/display_message 订阅器、
         * /imu/data 发布器、/car/mcu_status 发布器以及 Executor，并关联订阅回调与预置 IMU 协方差/坐标系属性
         * 若已初始化则直接返回 true，初始化失败会自动调用 destroy() 进行清理
         *
         * @param motion_controller 运动控制器对象指针，用于绑定控制逻辑
         * @return true  所有 ROS 实体初始化成功
         * @return false 初始化过程中任一步骤失败
         */
        bool init(MotionController* motion_controller);

        /**
         * @brief 设置来自 /car/display_message 订阅的消息回调函数
         *
         * 用于向管理器注册一个外部函数和上下文指针。当接收到新的显示文本消息时，将触发该回调处理
         *
         * @param callback 显示文本的回调函数指针
         * @param context  传递给回调函数的上下文上下文指针（通常为调用者对象指针）
         */
        void set_display_message_callback(DisplayMessageCallback callback, void* context);

        /**
         * @brief 安全销毁所有 ROS 实体并释放资源
         *
         * 销毁前会先安全切断/停止电机，然后依次清理发布者、订阅者、执行器、节点及 support 对象
         * 最后将所有句柄重置为 zero_initialized，确保内部状态复位，便于下次重新初始化
         */
        void destroy();

        /**
         * @brief 查询当前 ROS 实体是否全部初始化就绪
         *
         * @return true  所有 ROS 实体均已正确初始化且可正常使用
         * @return false 尚未初始化或处于销毁重置状态
         */
        bool is_init() const;

        /**
         * @brief 触发 Executor 处理等待中的订阅消息（非阻塞/限时）
         *
         * 在主循环 loop() 中持续调用。内部限时处理最多 5ms，用于及时响应 /cmd_vel_move 和 OLED 文本更新，
         * 同时避免长时间阻塞底层的电机安全控制流程
         *
         * @return true  成功执行了一次 spin_some
         * @return false ROS 未初始化或 spin 过程出错
         */
        bool spin_some();

        /**
         * @brief 发布 IMU 传感器数据到 /imu/data 话题
         *
         * 将输入的 IMU 线加速度与角速度填充至 sensor_msgs/msg/Imu 消息中，附带当前的 ROS 时间戳与坐标系（imu_link），
         * 并通过 RMW 协议层发布出去
         *
         * @param imu 包含线加速度和角速度的本地 IMU 结构体引用
         * @return true  发布成功
         * @return false 未初始化或 RCL 发布过程失败
         */
        bool publish_imu(const IMU& imu);

        /**
         * @brief 发布 MCU 综合状态到 /car/mcu_status 话题
         *
         * 将 ESP32 的网络连接、传感器状态、电机/编码器数据及故障标志位打包填充至 car_interfaces/msg/CarMcuStatus，
         * 附带时间戳后发布给 Jetson 上位机
         *
         * @param mcu_status MCU 底盘综合状态结构体引用
         * @return true  发布成功
         * @return false 未初始化或 RCL 发布过程失败
         */
        bool publish_mcu_status(const McuStatus& mcu_status);

    private:
        // /car/display_message是可变长字符串，micro-ros订阅前必须预留足够容量
        static constexpr size_t kDisplayMessageBufferCapacity = 256;
        // rclc使用C回调，静态回调函数再转发到当前实例
        /**
         * @brief 尝试与 micro-ROS Agent 进行时间同步
         *
         * 循环尝试与 Agent 建立 Epoch 时间同步（最多尝试 10 次）。同步成功后，发布的消息将带有高精度的 ROS 时间戳
         */
        void sync_time();

        /**
         * @brief 获取当前系统的 Epoch 时间戳（毫秒）
         *
         * 如果与 Agent 时间同步成功，则返回 Agent 同步的毫秒级 ROS 时间，否则退回使用 ESP32 本地的开机时间（millis()）
         *
         * @return int64_t 毫秒级时间戳
         */
        int64_t epoch_time() const;

        /**
         * @brief 预分配 /car/display_message 订阅消息的字符串内存缓冲区
         *
         * 辅助函数。由于 micro-ROS 接收可变长字符串前必须明确分配足够的内部容量（capacity），
         * 本函数确保 display_message_msg_ 拥有固定的内存空间，防止接收长文本时因溢出崩溃
         *
         * @return true  容量预分配成功
         * @return false 内存分配失败
         */
        bool prepare_display_message_buffer();

        /**
         * @brief /cmd_vel_move 订阅话题的静态 C 风格回调函数
         *
         * RCLC Executor 调用的静态入口，内部通过 instance_ 单例指针转发给 handle_twist() 进行实际的成员函数处理
         *
         * @param msg_in 传入的 geometry_msgs/msg/Twist 消息指针
         */
        static void twist_callback(const void* msg_in);

        /**
         * @brief /car/display_message 订阅话题的静态 C 风格回调函数
         *
         * RCLC Executor 调用的静态入口，内部通过 instance_ 单例指针转发给 handle_display_message() 进行处理
         *
         * @param msg_in 传入的 std_msgs/msg/String 消息指针
         */
        static void display_message_callback(const void* msg_in);

        /**
         * @brief 处理实际接收到的 /cmd_vel_move 速度控制指令
         *
         * 提取 Twist 消息中的线速度 linear.x 和角速度 angular.z，并将其下发给 MotionController 运动控制器
         *
         * @param msg 解析后的 geometry_msgs::msg::Twist 消息引用
         */
        void handle_twist(const geometry_msgs__msg__Twist& msg);

        /**
         * @brief 处理实际接收到的 /car/display_message 显示字符串消息
         *
         * 检查接收到的字符串有效性，并通过此前注册的 display_message_callback_ 回调转发给 OLED 管理器进行显示
         *
         * @param msg 解析后的 std_msgs::msg::String 消息引用
         */
        void handle_display_message(const std_msgs__msg__String& msg);

        // micro-ros默认分配器
        rcl_allocator_t allocator_;
        // rclc支持对象，保存agent会话上下文
        rclc_support_t support_;
        // rclc执行器，处理订阅和计时器回调的执行
        rclc_executor_t executor_;
        // esp32端ros节点
        rcl_node_t node_;
        // 话题/cmd_vel_move订阅者
        rcl_subscription_t cmd_vel_move_sub_;
        // 话题/car/display_message订阅者
        rcl_subscription_t display_message_sub_;
        // 话题/imu/data发布者
        rcl_publisher_t imu_pub_;
        // 话题/car/mcu_status发布者
        rcl_publisher_t mcu_status_pub_;
        // 速度命令接收缓冲
        geometry_msgs__msg__Twist cmd_vel_move_msg_;
        // oled文本接收缓冲
        std_msgs__msg__String display_message_msg_;
        // imu数据复用缓冲，避免loop中频繁分配
        sensor_msgs__msg__Imu imu_msg_;
        // mcu状态数据复用缓冲
        car_interfaces__msg__CarMcuStatus mcu_status_msg_;
        // 电机控制器指针，速度订阅回调通过它更新目标速度
        MotionController* motion_controller_;
        // oled文本回调函数
        DisplayMessageCallback display_message_callback_;
        // oled文本回调上下文
        void* display_message_context_;
        // 所有实体是否创建完成
        bool all_init_;
        // rclc support是否已经创建，用于安全销毁半初始化对象
        bool support_init_;
        // 执行器是否已经创建
        bool executor_init_;
        // 节点是否已经创建
        bool node_init_;
        // 速度命令订阅者是否已经创建
        bool cmd_vel_move_sub_init_;
        // oled文本订阅者是否已经创建
        bool display_message_sub_init_;
        // imu数据发布者是否已经创建
        bool imu_pub_init_;
        // mcu状态发布者是否已经创建
        bool mcu_status_pub_init_;
        // C静态回调当前要转发到的唯一管理器实例
        static RosEntityManager* instance_;
    };

}