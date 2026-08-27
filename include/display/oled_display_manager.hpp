#pragma once

#include <Arduino.h>
#include <U8g2lib.h>
#include "common/car_types.hpp"
#include "connect/wifi_connect_manager.hpp"

namespace car {

    class OledDisplayManager {
    public:
        explicit OledDisplayManager(const Config& config);

        /**
         * @brief 初始化 OLED 屏幕及 I2C 总线
         *
         * 在 setup 阶段调用。若配置使能了 OLED，则开启 I2C 总线并初始化 U8g2 显示库（设置地址、UTF8 字体等），
         * 随后在屏幕上绘制开机过渡标语（如“小车启动中”）并刷屏显示
         */
        void initialize();

        /**
         * @brief 设置来自远端（如 Web/Jetson）的状态提示文本
         *
         * 当收到上位机或网页发来的状态字符串时调用。更新本地消息缓冲区，刷新最后接收时间戳，
         * 并标记 remote_quit_ 为 true（表明 Jetson 曾成功上线过）。传入空指针或空字符串时直接忽略
         *
         * @param message 远端发来的状态文本字符串
         */
        void set_remote_message(const char* message);

        /**
         * @brief 清除当前保存的远端状态提示文本
         *
         * 将远端消息缓冲区清空并重置时间戳。清空后，下一次屏幕刷新（update）时会自动回退显示本地计算的状态信息
         */
        void clear_remote_message();

        /**
         * @brief 周期性刷新 OLED 显示屏（带节流）
         *
         * 在主循环 loop() 中持续调用。按照配置的 period 进行节流刷新（避免频繁 I2C 通信占用总线和导致屏幕闪烁）
         * 格式化并绘制当前 WiFi SSID、IP 地址、Agent 服务器地址，以及计算后的最终显示状态
         *
         * @param connect    WiFi与micro-ROS Agent连接管理器
         * @param ros_ready  ROS 实体（节点/发布者/订阅者）是否已成功创建就绪
         * @param mcu_status 底层 MCU 传感器与硬件状态
         */
        void update(
            const WiFiConnectManager& connect,
            bool ros_ready,
            const McuStatus& mcu_status);
    private:
        /**
         * @brief 计算本地基础保底状态文本
         *
         * 当没有有效/新鲜的远端消息时调用。仅依据本地能直接感知的状态进行阶梯判断：
         * 依次排查“等待WiFi” -> “等待Agent” -> “等待ROS” -> “前方障碍物” -> “等待Web状态”
         *
         * @param connect    WiFi与micro-ROS Agent连接管理器
         * @param ros_ready  ROS 实体是否就绪
         * @param mcu_status 底层 MCU 硬件状态
         * @return const char* 匹配到的本地保底状态文本字符串
         */
        const char* fallback_message(
            const WiFiConnectManager& connect,
            bool ros_ready,
            const McuStatus& mcu_status) const;

        /**
         * @brief 决断并返回当前 OLED 状态栏应显示的最终文本
         *
         * 结合远端消息时效性与本地网络状态：
         * 1. 优先使用在有效期内的远端 Web 状态，
         * 2. 若 Jetson 曾上线但当前失联，判定并提示“Jetson项目已停止”，
         * 3. 否则退回调用 fallback_message() 计算本地状态
         *
         * @param connect    WiFi与micro-ROS Agent连接管理器
         * @param ros_ready  ROS 实体是否就绪
         * @param mcu_status 底层 MCU 硬件状态
         * @return const char* 最终呈现给用户的状态字符串
         */
        const char* active_message(
            const WiFiConnectManager& connect,
            bool ros_ready,
            const McuStatus& mcu_status) const;

        /**
         * @brief 在 OLED 屏幕指定 Y 轴高度绘制一行“标签: 内容”格式的文本
         *
         * 辅助绘制函数。内部使用固定缓冲区将 label 与 value 拼接为 "label: value" 格式，并以 UTF-8 编码绘制到屏幕
         *
         * @param y     文字绘制的左下角 Y 坐标
         * @param label 行左侧的属性标签（如 "WiFi", "IP"）
         * @param value 行右侧的具体数值或状态字符串
         */
        void draw_line(int y, const char* label, const char* value);

        /**
         * @brief 安全复制远端消息字符串到本地固定大小缓冲区
         *
         * 内部字符串拷贝函数。采用截断保护机制，确保拷贝后的 remote_message_ 数组始终以 '\0' 结尾，防止缓冲区溢出
         *
         * @param message 待拷贝源字符串
         */
        void copy_message(const char* message);

        // 显示和网络配置引用
        const Config& config_;
        // 0.96寸oled显示屏驱动
        U8G2_SSD1306_128X64_NONAME_F_HW_I2C display_;
        // oled初始化是否成功，未成功时update()直接返回
        bool initialized_;
        // 用于jetson退出后的oled提示
        bool remote_quit_;
        // 最近一次刷新屏幕的arduino毫秒tick
        unsigned long last_update_tick_;
        // 最近一次收到/car/display_message的arduino毫秒tick
        unsigned long last_remote_message_tick_;
        // 最近一次远程状态文本，按oled可显示宽度保留短字符
        char remote_message_[80];
        // 本机wifi ip字符串缓存
        char ip_buffer_[24];
        // jetson agent地址字符串缓存
        char server_buffer_[32];
    };

}