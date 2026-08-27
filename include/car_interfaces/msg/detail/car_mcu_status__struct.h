// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from car_interfaces:msg/CarMcuStatus.idl
// generated code does not contain a copyright notice

#ifndef CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__STRUCT_H_
#define CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.h"

/// Struct defined in msg/CarMcuStatus in the package car_interfaces.
/**
  * esp32发布的状态接口，jetson端的包订阅该消息接口
 */
typedef struct car_interfaces__msg__CarMcuStatus
{
  /// 消息时间戳，esp32和jetson端必须统一
  builtin_interfaces__msg__Time stamp;
  /// wifi是否连接到jetson热点
  bool wifi_connect_ok;
  /// micro-ros的agent是否连接
  bool agent_connect_ok;
  /// imu是否完成初始化且当前读数有效
  bool imu_ok;
  /// 编码器是否完成初始化且当前轮速计算有效
  bool encoder_ok;
  /// 电机驱动是否完成初始化
  bool motor_driver_ok;
  /// esp32是否因为长时间没有收到/cmd_vel_move而停车（/cmd_vel_move是esp32接收唯一移动话题）
  bool cmd_timeout_ok;
  /// 左轮编码器估算速度，单位m/s
  float left_wheel_speed;
  /// 右轮编码器估算速度，单位m/s
  float right_wheel_speed;
  /// 左轮累计编码器tick值，可以使用相邻消息差值计算位移（不空转）
  int32_t left_encoder_ticks;
  /// 右轮累计编码器tick值，可以使用相邻消息差值计算位移（不空转）
  int32_t right_encoder_ticks;
  /// 超声波测距是否有效
  bool ultrasonic_ok;
  /// 前方是否有障碍物
  bool obstacle_ok;
  /// 前方障碍物距离，单位m
  float obstacle_distance;
  /// 故障位，esp32低位，jetson使用18急停，19离线，20前方有障碍
  uint32_t fault_bits;
} car_interfaces__msg__CarMcuStatus;

// Struct for a sequence of car_interfaces__msg__CarMcuStatus.
typedef struct car_interfaces__msg__CarMcuStatus__Sequence
{
  car_interfaces__msg__CarMcuStatus * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} car_interfaces__msg__CarMcuStatus__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__STRUCT_H_
