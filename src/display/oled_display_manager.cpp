#include "display/oled_display_manager.hpp"
#include <WiFi.h>
#include <Wire.h>
#include <string.h>
#include <vector>

namespace car {

    OledDisplayManager::OledDisplayManager(const Config& config)
    : config_(config),
      display_(U8G2_R0, U8X8_PIN_NONE),
      initialized_(false),
      remote_quit_(false),
      last_update_tick_(0),
      last_remote_message_tick_(0),
      remote_message_{0},
      ip_buffer_{0},
      server_buffer_{0} {}

    void OledDisplayManager::initialize() {
        if (!config_.oled_enabled) {
            // 允许通过配置关闭OLED，便于没有屏幕的开发板复用固件
            return;
        }
        // OLED和MPU共用ESP32的I2C总线，这里使用同一组引脚，避免两个设备抢占不同Wire配置
        Wire.begin(config_.oled_sda_pin, config_.oled_scl_pin);
        display_.setI2CAddress(static_cast<uint8_t>(config_.oled_i2c_address << 1));
        display_.begin();
        display_.enableUTF8Print();
        display_.setFont(u8g2_font_wqy12_t_gb2312);
        initialized_ = true;
        display_.clearBuffer();
        // 坐标原点是屏幕的左上角第一个像素点，绘制文字drawUTF8(x, y, str)，(x, y)代表文字的左下角坐标
        display_.drawUTF8(0, 14, "小车启动中");
        display_.sendBuffer();
    }

    void OledDisplayManager::set_remote_message(const char * message) {
        // Web下发空字符串时不覆盖本地连接状态
        if (message == nullptr || message[0] == '\0') {
            return;
        }
        copy_message(message);
        // 本轮运行知道Jetson曾经在线
        remote_quit_ = true;
        last_remote_message_tick_ = millis();
    }

    void OledDisplayManager::clear_remote_message() {
        // 清掉远端文本后，下一次update会回退到本地连接状态
        remote_message_[0] = '\0';
        last_remote_message_tick_ = 0;
    }

    void OledDisplayManager::update(
        const WiFiConnectManager& connect,
        bool ros_ready,
        const McuStatus& mcu_status) {

        if (!initialized_) {
            return;
        }
        // 当前Arduino毫秒tick，用于OLED刷新节流
        const unsigned long now_tick = millis();
        if (now_tick - last_update_tick_ < config_.oled_update_period) {
            // OLED刷新较慢，节流可以减少I2C占用并避免屏幕闪烁
            return;
        }
        last_update_tick_ = now_tick;
        // ESP32当前从热点DHCP获取的本机地址
        const IPAddress ip = WiFi.localIP();
        // U8g2绘制接口使用C字符串，这里把IPAddress格式化到固定缓冲区
        snprintf(
            ip_buffer_,
            sizeof(ip_buffer_),
            "%u.%u.%u.%u",
            ip[0],
            ip[1],
            ip[2],
            ip[3]);
        snprintf(
            server_buffer_,
            sizeof(server_buffer_),
            "%u.%u.%u.%u:%u",
            config_.agent_ip[0],
            config_.agent_ip[1],
            config_.agent_ip[2],
            config_.agent_ip[3],
            config_.agent_port);
        display_.clearBuffer();
        draw_line(11, "WiFi", config_.wifi_ssid);
        draw_line(25, "IP", connect.wifi_connected() ? ip_buffer_ : "未连接");
        draw_line(39, "Server", server_buffer_);
        draw_line(53, "状态", active_message(connect, ros_ready, mcu_status));
        display_.sendBuffer();
    }

    const char * OledDisplayManager::fallback_message(
        const WiFiConnectManager& connect,
        bool ros_ready,
        const McuStatus& mcu_status) const {
        // fallback_message只描述本地能判断的状态，不使用可能过期Web文本
        if (!connect.wifi_connected()) {
            return "等待WiFi";
        }
        if (!connect.agent_connected()) {
            return "等待Agent";
        }
        if (!ros_ready) {
            return "等待ROS";
        }
        if (mcu_status.obstacle_ok) {
            return "前方障碍物";
        }
        return "等待Web状态";
    }

    const char * OledDisplayManager::active_message(
        const WiFiConnectManager& connect,
        bool ros_ready,
        const McuStatus& mcu_status) const {
        // 当前Arduino毫秒tick，用于判断Web文本新鲜度
        const unsigned long now_tick = millis();
        // Web文本只有在超时窗口内才可信，避免Jetson退出后OLED长时间显示旧状态
        // 最近Web文本是否仍处于OLED显示有效窗口
        const bool remote_recent =
            last_remote_message_tick_ > 0 &&
            now_tick - last_remote_message_tick_ <= config_.oled_message_timeout;
        if (remote_recent && remote_message_[0] != '\0') {
            return remote_message_;
        }
        // 只要本轮曾收到Web状态，后续Agent/ROS失联就认为Jetson项目已经退出或重启中
        // 这样不会继续显示旧的小车在线，也不需要ESP32重启才能刷新OLED
        if (remote_quit_ && connect.wifi_connected() && (!connect.agent_connected() || !ros_ready)) {
            return "Jetson项目已停止";
        }
        if (remote_quit_ && connect.wifi_connected() && connect.agent_connected() && ros_ready) {
            return "Jetson项目已停止";
        }
        if (connect.wifi_connected() && connect.agent_connected() && ros_ready) {
            return "等待Web状态";
        }
        return fallback_message(connect, ros_ready, mcu_status);
    }

    void OledDisplayManager::draw_line(int y, const char * label, const char * value) {
        // 每行使用统一(标签: 值)格式，保证0.96寸屏幕上信息密度稳定
        // 拼接一行(标签: 值)文本的固定缓冲区
        char line[96];
        snprintf(line, sizeof(line), "%s: %s", label, value);
        display_.drawUTF8(0, y, line);
    }

    void OledDisplayManager::copy_message(const char * message) {
        // 固定数组复制必须手动补'\0'，防止长Web文本越界或未终止
        strncpy(remote_message_, message, sizeof(remote_message_) - 1);
        remote_message_[sizeof(remote_message_) - 1] = '\0';
    }


}