// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from car_interfaces:msg/CarMcuStatus.idl
// generated code does not contain a copyright notice

#ifndef CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__BUILDER_HPP_
#define CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "car_interfaces/msg/detail/car_mcu_status__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace car_interfaces
{

namespace msg
{

namespace builder
{

class Init_CarMcuStatus_fault_bits
{
public:
  explicit Init_CarMcuStatus_fault_bits(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  ::car_interfaces::msg::CarMcuStatus fault_bits(::car_interfaces::msg::CarMcuStatus::_fault_bits_type arg)
  {
    msg_.fault_bits = std::move(arg);
    return std::move(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_obstacle_distance
{
public:
  explicit Init_CarMcuStatus_obstacle_distance(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_fault_bits obstacle_distance(::car_interfaces::msg::CarMcuStatus::_obstacle_distance_type arg)
  {
    msg_.obstacle_distance = std::move(arg);
    return Init_CarMcuStatus_fault_bits(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_obstacle_ok
{
public:
  explicit Init_CarMcuStatus_obstacle_ok(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_obstacle_distance obstacle_ok(::car_interfaces::msg::CarMcuStatus::_obstacle_ok_type arg)
  {
    msg_.obstacle_ok = std::move(arg);
    return Init_CarMcuStatus_obstacle_distance(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_ultrasonic_ok
{
public:
  explicit Init_CarMcuStatus_ultrasonic_ok(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_obstacle_ok ultrasonic_ok(::car_interfaces::msg::CarMcuStatus::_ultrasonic_ok_type arg)
  {
    msg_.ultrasonic_ok = std::move(arg);
    return Init_CarMcuStatus_obstacle_ok(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_right_encoder_ticks
{
public:
  explicit Init_CarMcuStatus_right_encoder_ticks(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_ultrasonic_ok right_encoder_ticks(::car_interfaces::msg::CarMcuStatus::_right_encoder_ticks_type arg)
  {
    msg_.right_encoder_ticks = std::move(arg);
    return Init_CarMcuStatus_ultrasonic_ok(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_left_encoder_ticks
{
public:
  explicit Init_CarMcuStatus_left_encoder_ticks(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_right_encoder_ticks left_encoder_ticks(::car_interfaces::msg::CarMcuStatus::_left_encoder_ticks_type arg)
  {
    msg_.left_encoder_ticks = std::move(arg);
    return Init_CarMcuStatus_right_encoder_ticks(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_right_wheel_speed
{
public:
  explicit Init_CarMcuStatus_right_wheel_speed(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_left_encoder_ticks right_wheel_speed(::car_interfaces::msg::CarMcuStatus::_right_wheel_speed_type arg)
  {
    msg_.right_wheel_speed = std::move(arg);
    return Init_CarMcuStatus_left_encoder_ticks(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_left_wheel_speed
{
public:
  explicit Init_CarMcuStatus_left_wheel_speed(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_right_wheel_speed left_wheel_speed(::car_interfaces::msg::CarMcuStatus::_left_wheel_speed_type arg)
  {
    msg_.left_wheel_speed = std::move(arg);
    return Init_CarMcuStatus_right_wheel_speed(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_cmd_timeout_ok
{
public:
  explicit Init_CarMcuStatus_cmd_timeout_ok(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_left_wheel_speed cmd_timeout_ok(::car_interfaces::msg::CarMcuStatus::_cmd_timeout_ok_type arg)
  {
    msg_.cmd_timeout_ok = std::move(arg);
    return Init_CarMcuStatus_left_wheel_speed(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_motor_driver_ok
{
public:
  explicit Init_CarMcuStatus_motor_driver_ok(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_cmd_timeout_ok motor_driver_ok(::car_interfaces::msg::CarMcuStatus::_motor_driver_ok_type arg)
  {
    msg_.motor_driver_ok = std::move(arg);
    return Init_CarMcuStatus_cmd_timeout_ok(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_encoder_ok
{
public:
  explicit Init_CarMcuStatus_encoder_ok(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_motor_driver_ok encoder_ok(::car_interfaces::msg::CarMcuStatus::_encoder_ok_type arg)
  {
    msg_.encoder_ok = std::move(arg);
    return Init_CarMcuStatus_motor_driver_ok(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_imu_ok
{
public:
  explicit Init_CarMcuStatus_imu_ok(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_encoder_ok imu_ok(::car_interfaces::msg::CarMcuStatus::_imu_ok_type arg)
  {
    msg_.imu_ok = std::move(arg);
    return Init_CarMcuStatus_encoder_ok(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_agent_connect_ok
{
public:
  explicit Init_CarMcuStatus_agent_connect_ok(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_imu_ok agent_connect_ok(::car_interfaces::msg::CarMcuStatus::_agent_connect_ok_type arg)
  {
    msg_.agent_connect_ok = std::move(arg);
    return Init_CarMcuStatus_imu_ok(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_wifi_connect_ok
{
public:
  explicit Init_CarMcuStatus_wifi_connect_ok(::car_interfaces::msg::CarMcuStatus & msg)
  : msg_(msg)
  {}
  Init_CarMcuStatus_agent_connect_ok wifi_connect_ok(::car_interfaces::msg::CarMcuStatus::_wifi_connect_ok_type arg)
  {
    msg_.wifi_connect_ok = std::move(arg);
    return Init_CarMcuStatus_agent_connect_ok(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

class Init_CarMcuStatus_stamp
{
public:
  Init_CarMcuStatus_stamp()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_CarMcuStatus_wifi_connect_ok stamp(::car_interfaces::msg::CarMcuStatus::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return Init_CarMcuStatus_wifi_connect_ok(msg_);
  }

private:
  ::car_interfaces::msg::CarMcuStatus msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::car_interfaces::msg::CarMcuStatus>()
{
  return car_interfaces::msg::builder::Init_CarMcuStatus_stamp();
}

}  // namespace car_interfaces

#endif  // CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__BUILDER_HPP_
