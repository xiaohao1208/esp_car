#include "connect/wifi_connect_manager.hpp"
#include <micro_ros_platformio.h>
#include <rcl/types.h>

namespace car {

    namespace {
        // micro-ros自定义udp传输回调共享的agent地址定位器
        micro_ros_agent_locator agent_locator;
    }

    WiFiConnectManager::WiFiConnectManager(const Config& config)
    : config_(config),
      wifi_was_connected_(false),
      agent_connected_(false),
      udp_transport_configured_(false),
      consecutive_agent_successes_(0),
      consecutive_agent_failures_(0),
      wifi_connected_since_tick_(0),
      last_agent_ping_tick_(0),
      last_agent_check_tick_(0),
      last_wifi_reconnect_tick_(0) {}

    void WiFiConnectManager::connect_wifi(){
        // setup阶段先尝试阻塞连接一次，让OLED/串口尽快显示初始IP
        WiFi.persistent(false);
        // 关闭WiFi sleep可以降低UDP延迟和micro-ROS ping抖动
        WiFi.mode(WIFI_STA);
        WiFi.setSleep(false);
        WiFi.disconnect(false, false);
        delay(100);
        WiFi.begin(config_.wifi_ssid, config_.wifi_password);
        // 首次WiFi连接等待开始时的Arduino毫秒tick
        const unsigned long start_tick = millis();
        // 首次连接最多等15秒，失败后交给loop()中的maintain_wifi非阻塞重试
        while (WiFi.status() != WL_CONNECTED && millis() - start_tick < 15000) {
            delay(250);
        }
        if (WiFi.status() == WL_CONNECTED) {
            wifi_was_connected_ = true;
            wifi_connected_since_tick_ = millis();
            Serial.print("WiFi已连接，IP=");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("WiFi首次连接超时，进入自动重连");
        }
        last_wifi_reconnect_tick_ = millis();
    }

    void WiFiConnectManager::maintain_wifi() {
        const bool connected_now = WiFi.status() == WL_CONNECTED;
        if (connected_now) {
            // 只在断开到连接的边沿记录时间，保持连接时不能反复重置
            if (!wifi_was_connected_) {
                wifi_was_connected_ = true;
                wifi_connected_since_tick_ = millis();
                Serial.print("WiFi已重新连接，IP=");
                Serial.println(WiFi.localIP());
            }
            return;
        }
        if (wifi_was_connected_) {
            Serial.println("WiFi已断开，等待重新连接");
        }
        wifi_was_connected_ = false;
        wifi_connected_since_tick_ = 0;
        // WiFi断开后必须重新注册micro-ROS transport，否则底层UDP仍持有旧状态
        udp_transport_configured_ = false;
        reset_agent_state();
        // 当前Arduino毫秒tick，用于节流WiFi重连
        const unsigned long now_tick = millis();
        // 重连动作节流，避免WiFi.begin()在loop()中高频阻塞
        if (now_tick - last_wifi_reconnect_tick_ < config_.wifi_reconnect_period) {
            return;
        }
        last_wifi_reconnect_tick_ = now_tick;
        WiFi.disconnect(false, false);
        WiFi.mode(WIFI_STA);
        WiFi.setSleep(false);
        WiFi.begin(config_.wifi_ssid, config_.wifi_password);
    }

    void WiFiConnectManager::setup_micro_ros_connect() {
        // transport只需要注册一次，WiFi掉线后maintain_wifi会清掉该标志
        if (udp_transport_configured_) {
            return;
        }
        if (!wifi_connected()) {
            return;
        }
        // 直接注册micro-ROS WiFi UDP transport
        // 该上游helper会再次WiFi.begin()，第一次上电时可能打断刚稳定的WiFi/UDP状态
        agent_locator.address = config_.agent_ip;
        agent_locator.port = config_.agent_port;
        // false表示非串口framed模式，直接使用UDP自定义读写回调
        rmw_uros_set_custom_transport(
            false,
            static_cast<void *>(&agent_locator),
            platformio_transport_open,
            platformio_transport_close,
            platformio_transport_write,
            platformio_transport_read);
        udp_transport_configured_ = true;
        Serial.print("micro-ROS UDP传输已配置，Agent=");
        Serial.print(config_.agent_ip);
        Serial.print(":");
        Serial.println(config_.agent_port);
    }

    void WiFiConnectManager::reset_agent_state() {
        // 清理Agent状态不会直接销毁ROS实体，上层根据状态变化执行销毁
        if (agent_connected_) {
            Serial.println("micro-ROS Agent已断开");
        }
        agent_connected_ = false;
        consecutive_agent_successes_ = 0;
        consecutive_agent_failures_ = 0;
        last_agent_ping_tick_ = 0;
        last_agent_check_tick_ = 0;
    }

    void WiFiConnectManager::update_agent_state() {
        // 没有WiFi时不要ping Agent，避免micro-ROS transport在无网络状态下阻塞
        if (!wifi_connected()) {
            reset_agent_state();
            return;
        }
        // 当前Arduino毫秒tick，用于Agent检查和丢失宽限
        const unsigned long now_tick = millis();
        if (wifi_connected_since_tick_ > 0 &&
         now_tick - wifi_connected_since_tick_ < config_.wifi_settle_delay) {
            // WiFi刚拿到IP时UDP路由/DHCP仍可能未完全稳定，先等待一小段时间
            return;
        }
        setup_micro_ros_connect();
        if (!udp_transport_configured_) {
            return;
        }
        if (now_tick - last_agent_check_tick_ < config_.agent_ping_period) {
            // ping周期由配置控制，避免频繁ping占用无线链路
            return;
        }
        last_agent_check_tick_ = now_tick;
        // WiFi UDP偶尔会丢ping。这里用最后一次成功时间判定Agent丢失
        // 避免短暂抖动时频繁销毁micro-ROS实体
        // 本轮UDP Agent ping是否成功
        const bool ping_ok = rmw_uros_ping_agent(
            config_.agent_ping_timeout,
            config_.agent_ping_attempts) == RCL_RET_OK;
        if (ping_ok) {
            if (consecutive_agent_successes_ < 255) {
                ++consecutive_agent_successes_;
            }
            consecutive_agent_failures_ = 0;
            last_agent_ping_tick_ = now_tick;
        } else if (consecutive_agent_failures_ < 255) {
            ++consecutive_agent_failures_;
            consecutive_agent_successes_ = 0;
        }
        // 最近一次成功ping是否仍处于断线宽限窗口
        const bool recently_seen =
            last_agent_ping_tick_ > 0 && now_tick - last_agent_ping_tick_ <= config_.agent_lost_timeout;
        // 连续成功次数是否已达到首次上线门槛
        const bool stable_now = consecutive_agent_successes_ >= config_.agent_required_successes;
        // 已连接状态下允许短暂ping丢失，超过lost_timeout才认为Jetson端退出或断网
        // 综合首次上线门槛和已连接宽限后的最终Agent状态
        const bool connected_now = stable_now || (agent_connected_ && recently_seen);
        if (!agent_connected_ && connected_now) {
            Serial.println("micro-ROS Agent已稳定连接");
        } else if (agent_connected_ && !connected_now) {
            Serial.println("micro-ROS Agent连接超时");
        }
        agent_connected_ = connected_now;
    }

    bool WiFiConnectManager::wifi_connected() const {
        return WiFi.status() == WL_CONNECTED;
    }

    bool WiFiConnectManager::agent_connected() const {
        return agent_connected_;
    }

}
