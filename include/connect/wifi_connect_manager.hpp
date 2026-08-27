#pragma once

#include <WiFi.h>
#include "common/car_types.hpp"

namespace car {

    class WiFiConnectManager {
    public:
        /**
         * @brief 保存网络配置并初始化 WiFi 和 Agent 状态
         *
         * @param config 全局只读配置，生命周期必须长于本管理器
         */
        explicit WiFiConnectManager(const Config& config);

        // 当前使用wifi作为micro-ros传输，之后换成usb串口时ros话题不需要变化
        /**
         * @brief 阻塞式初始化 WiFi 连接
         *
         * 在setup阶段调用，禁用WiFi休眠以降低UDP延迟，尝试阻塞式连接WiFi最多15秒
         * 用于让OLED或串口尽快获取并显示初始IP地址，如果超时未连上则交给maintain_wifi()非阻塞重试
         */
        void connect_wifi();

        /**
         * @brief 维护并保持WiFi状态
         *
         * 在主循环loop()中持续调用，检测连接和断开边沿
         * 按照wifi_reconnect_period节流重连，避免频繁调用begin()阻塞主循环
         */
        void maintain_wifi();

        /**
         * @brief 初始化并注册micro-ROS的UDP自定义传输层
         *
         * 在WiFi稳定连通后执行，配置Agent的IP和端口并注册UDP回调
         * 每次WiFi连接周期只注册一次
         */
        void setup_micro_ros_connect();

        /**
         * @brief 更新micro-ROS Agent连接状态
         *
         * WiFi稳定后按周期发送Agent ping
         * 连续成功达到门槛后才允许创建ROS实体
         */
        void update_agent_state();

        /**
         * @brief 查询当前WiFi是否连接
         *
         * @return true WiFi已连接并获取IP
         * @return false WiFi处于断开或重连状态
         */
        bool wifi_connected() const;

        /**
         * @brief 查询micro-ROS Agent是否稳定连接
         *
         * @return true Agent已连接或仍处于短暂丢包宽限期
         * @return false Agent未连接或网络已断开
         */
        bool agent_connected() const;

    private:
        /**
         * @brief 重置micro-ROS Agent连接状态
         *
         * 清空连接标志和ping计数器
         * ROS实体由上层在检测到断开边沿后销毁
         */
        void reset_agent_state();

        // 配置引用，包含ssid，密码和micro-ros-agent地址
        const Config& config_;
        // 上一轮wifi连接状态，用于识别连接和断开边沿
        bool wifi_was_connected_;
        // agent是否可用，短暂ping丢包会宽容处理
        bool agent_connected_;
        // micro-ros自定义udp传输是否已经注册
        bool udp_transport_configured_;
        // 连续agent ping成功次数，用于确认agent稳定上线
        uint8_t consecutive_agent_successes_;
        // 连续agent ping失败次数，用于断线判断
        uint8_t consecutive_agent_failures_;
        // wifi连上arduino毫秒tick，用于稳定等待
        unsigned long wifi_connected_since_tick_;
        // 最近一次成功ping到agent的arduino毫秒tick
        unsigned long last_agent_ping_tick_;
        // 最近一次执行agent ping的arduino毫秒tick
        unsigned long last_agent_check_tick_;
        // 最近一次发起wifi重连的arduino毫秒tick
        unsigned long last_wifi_reconnect_tick_;
    };

}
