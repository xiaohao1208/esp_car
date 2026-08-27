# ESP32 小车固件

## 第一部分：项目介绍

这套固件运行在通用 ESP32 DevKit 上，负责差速电机闭环、左右编码器、MPU6050、HC-SR04、OLED、Wi-Fi 和 micro-ROS 生命周期。Jetson 通过 /cmd_vel_move 下发已经完成安全仲裁的速度；ESP32 发布 IMU 与原始底盘状态，并在通信失效时独立停车。

### 模块结构

| 目录 | 职责 |
| --- | --- |
| include/config | 网络、引脚、机械、控制周期和安全阈值 |
| include/common | 模块之间传递的固定数据类型和故障位 |
| include/control、src/control | 差速换算、软启动、前馈和双轮 PID |
| include/sensors、src/sensors | MPU6050、编码器和超声波 |
| include/connect、src/connect | Wi-Fi 重连和 Agent 在线判定 |
| include/ros、src/ros | micro-ROS 发布器、订阅器和实体恢复 |
| include/display、src/display | OLED 本机状态和 Jetson 文本 |
| include/app、src/app | 硬件初始化与非阻塞周期调度 |

### 硬件和接线

| 功能 | GPIO |
| --- | --- |
| 左电机 A / B | 22 / 23 |
| 右电机 A / B | 12 / 13 |
| 左编码器 A / B | 32 / 33 |
| 右编码器 A / B | 26 / 25 |
| I2C SDA / SCL | 18 / 19 |
| 超声波 Trig / Echo | 27 / 21 |

MPU6050 和 0.96 英寸 OLED 共用 I2C，总线地址由各自驱动识别，OLED 默认 0x3C。电机使用 MCPWM，默认载波 20 kHz。初始化后会读取左右定时器实际频率，配置失败时保持停车并上报电机驱动故障。

### 网络、接口和安全

| 项目 | 默认值 |
| --- | --- |
| Wi-Fi SSID / 密码 | jetson / 88888888 |
| Agent 地址 | 192.168.4.1 |
| Agent 端口 | UDP 8888 |
| 串口 | 115200 |
| 左轮每 tick 距离 | 0.0001039203 m |
| 右轮每 tick 距离 | 0.0001033942 m |
| 驱动轮中心距 | 0.175 m |

ROS 节点名为 esp32_car：

| 方向 | 接口 | 类型 |
| --- | --- | --- |
| 订阅 | /cmd_vel_move | geometry_msgs/msg/Twist |
| 订阅 | /car/display_message | std_msgs/msg/String |
| 发布 | /imu/data | sensor_msgs/msg/Imu |
| 发布 | /car/mcu_status | car_interfaces/msg/CarMcuStatus |

Wi-Fi 或 Agent 失联时销毁 ROS 实体并停止电机；连接恢复后按退避周期重建。超过 750 ms 没有收到速度命令时本地停车。超声波小于 0.10 m 时锁止前进，达到 0.15 m 才解除，后退始终允许。停止后起步或换向时，双轮目标在 100 ms 内按同一比例上升；急停、近障、故障和超时不经过软启动，直接归零。

固件只使用 CarMcuStatus。include/car_interfaces 和 lib 中保留的是该接口对应的固定头文件和 Xtensa 静态库；未使用且容易过期的 CarStatus 和 EmergencyStop 生成文件不再放进固件仓库。

## 第二部分：从零实现与部署

### 1. 安装 PlatformIO Core

Linux 或 macOS 使用 PlatformIO 官方安装脚本，不要用 sudo 运行安装器：

    sudo apt update
    sudo apt install -y python3 python3-venv curl git
    curl -fsSL -o get-platformio.py \
      https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
    python3 get-platformio.py
    source ~/.platformio/penv/bin/activate
    pio --version

让 pio 在后续终端可用：

    echo 'export PATH="$HOME/.platformio/penv/bin:$PATH"' >> ~/.bashrc
    source ~/.bashrc

安装 PlatformIO 的 udev 规则后重新插拔开发板：

    curl -fsSL \
      https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules |
      sudo tee /etc/udev/rules.d/99-platformio-udev.rules >/dev/null
    sudo udevadm control --reload-rules
    sudo udevadm trigger
    sudo usermod -aG dialout "$USER"

用户组修改需要注销后重新登录。

### 2. 从空目录创建工程

    mkdir esp32_car
    cd esp32_car
    pio project init --board esp32dev
    mkdir -p include src lib

platformio.ini 使用 Arduino、Wi-Fi UDP micro-ROS 和固定的 Espressif32 6.13.0。项目依赖固定为：

| 库 | 版本 |
| --- | --- |
| MPU6050_light | d3e10a2545b03cfb874a9faaf4fc125d02d98421 |
| Esp32McpwmMotor | 145c06da90599769419abd46d858f17b4f018929 |
| Esp32PcntEncoder | 7cce8d01fa2d343878f3e70c71d95cb51a387316 |
| micro_ros_platformio | a2f9de79f90af96ee63175a28d441da3eeac40e4 |
| U8g2 | 2.36.18 |

正式使用时直接克隆仓库即可：

    ESP32_CAR_REPOSITORY_URL="<esp32_car 仓库地址>"
    git clone "$ESP32_CAR_REPOSITORY_URL" esp32_car
    cd esp32_car
    pio run -e esp32_car

第一次构建会下载固定平台和依赖，因此需要网络；完成后相同缓存可以离线构建。

### 3. 实现顺序

1. 在 car_config.hpp 集中定义引脚、网络、机械参数、速度范围、周期和阈值，毫秒周期统一使用 uint32_t。
2. 在 SensorManager 初始化 I2C、MPU6050 和超声波，任何非有限 IMU 读数都标记为无效。
3. 在 MotionController 完成差速逆运动学、轮速估计、编码器异常门禁、前馈、PI 修正和软启动。
4. 在 WifiConnectManager 使用无阻塞时间差完成 Wi-Fi 重试、Agent 连续成功确认和掉线宽限。
5. 在 RosEntityManager 创建唯一速度订阅、IMU 与 CarMcuStatus 发布器以及 OLED 文本订阅。
6. 在 CarApp 按硬件、网络、ROS 的顺序初始化，并在 loop 中调度连接、executor、电机、传感器、发布和显示。
7. 所有周期判断都使用 当前 tick - 上次 tick，保持 millis 回绕安全；控制循环中不分配动态内存。

### 4. 同步 CarMcuStatus 接口

Jetson 的 car_interfaces/msg/CarMcuStatus.msg 是接口源定义。修改它以后必须重新生成 ESP32 的 C 头文件和以下四类 Xtensa 静态库：

    rosidl_generator_c
    rosidl_typesupport_c
    rosidl_typesupport_introspection_c
    rosidl_typesupport_microxrcedds_c

生成环境必须使用与 platformio.ini 固定版本兼容的 ROS 2 Humble micro-ROS 交叉工具链。生成后只复制 CarMcuStatus 相关头文件到 include/car_interfaces，并替换 lib 中四个同名静态库；不要把 Jetson 上的 x86_64 或 aarch64 库直接复制给 ESP32。

更新完成后先核对字段顺序和类型：

    builtin_interfaces/Time stamp
    bool wifi_connect_ok
    bool agent_connect_ok
    bool imu_ok
    bool encoder_ok
    bool motor_driver_ok
    bool cmd_timeout_ok
    float32 left_wheel_speed
    float32 right_wheel_speed
    int32 left_encoder_ticks
    int32 right_encoder_ticks
    bool ultrasonic_ok
    bool obstacle_ok
    float32 obstacle_distance
    uint32 fault_bits

然后执行一次完整固件构建。不要对当前 FishROS micro_ros_platformio 分支执行 clean_microros；该分支依赖随库提供的预构建 micro-ROS 工作区，普通的 pio clean 不受影响。

### 5. 编译、烧录和监控

    pio run -e esp32_car
    pio run -e esp32_car --target upload
    pio device monitor --baud 115200

指定串口：

    pio run -e esp32_car --target upload --upload-port /dev/ttyUSB0
    pio device monitor --port /dev/ttyUSB0 --baud 115200

如果串口被 brltty 占用，确认机器不需要盲文设备支持后再卸载：

    sudo apt remove --purge brltty

### 6. 首次联调

先架空驱动轮，再按顺序检查：

1. 左右电机方向与左右编码器增量符号一致。
2. Jetson 热点为 jetson，地址为 192.168.4.1，Agent 监听 UDP 8888。
3. 串口和 OLED 显示 Wi-Fi、Agent、ROS 实体均已就绪。
4. Jetson 能持续收到 /car/mcu_status 与 /imu/data。
5. 低速命令下左右轮反馈合理，停止发送后 750 ms 内停车。
6. 0.10 m 近障能阻止前进，移到 0.15 m 外解除，并始终允许后退。
7. 断开 Agent 或热点时电机立即停止，恢复网络后实体能自动重建。

低速命令：

    ros2 topic pub --rate 10 /cmd_vel_move geometry_msgs/msg/Twist \
      "{linear: {x: 0.05}, angular: {z: 0.0}}"

### 7. 机械和控制标定

先确定 wheel_distance 和左右 wheel_per_tick，再同步修改 Jetson 的 car_base/config/base.yaml。轮速控制按以下顺序调：

1. 架空车轮并关闭 PID，分别测量左右轮刚好连续转动的输出，写入 feedforward_static。
2. 发送多组稳定轮速，用输出变化量除以轮速变化量估算 feedforward_velocity。
3. 开启 PID，逐步增加 Kp，直到响应足够快且没有持续振荡。
4. 小幅增加 Ki 消除稳态误差；编码器噪声较大时保持 Kd 为 0。
5. 保留积分和输出限幅，不用增大积分掩盖机械堵转。

每次改动后重新执行架空、低速、超时、近障、断网和急停检查，再让车辆落地。
