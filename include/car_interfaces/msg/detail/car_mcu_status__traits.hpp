// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from car_interfaces:msg/CarMcuStatus.idl
// generated code does not contain a copyright notice

#ifndef CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__TRAITS_HPP_
#define CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "car_interfaces/msg/detail/car_mcu_status__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__traits.hpp"

namespace car_interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const CarMcuStatus & msg,
  std::ostream & out)
{
  out << "{";
  // member: stamp
  {
    out << "stamp: ";
    to_flow_style_yaml(msg.stamp, out);
    out << ", ";
  }

  // member: wifi_connect_ok
  {
    out << "wifi_connect_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.wifi_connect_ok, out);
    out << ", ";
  }

  // member: agent_connect_ok
  {
    out << "agent_connect_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.agent_connect_ok, out);
    out << ", ";
  }

  // member: imu_ok
  {
    out << "imu_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.imu_ok, out);
    out << ", ";
  }

  // member: encoder_ok
  {
    out << "encoder_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.encoder_ok, out);
    out << ", ";
  }

  // member: motor_driver_ok
  {
    out << "motor_driver_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.motor_driver_ok, out);
    out << ", ";
  }

  // member: cmd_timeout_ok
  {
    out << "cmd_timeout_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.cmd_timeout_ok, out);
    out << ", ";
  }

  // member: left_wheel_speed
  {
    out << "left_wheel_speed: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_speed, out);
    out << ", ";
  }

  // member: right_wheel_speed
  {
    out << "right_wheel_speed: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_speed, out);
    out << ", ";
  }

  // member: left_encoder_ticks
  {
    out << "left_encoder_ticks: ";
    rosidl_generator_traits::value_to_yaml(msg.left_encoder_ticks, out);
    out << ", ";
  }

  // member: right_encoder_ticks
  {
    out << "right_encoder_ticks: ";
    rosidl_generator_traits::value_to_yaml(msg.right_encoder_ticks, out);
    out << ", ";
  }

  // member: ultrasonic_ok
  {
    out << "ultrasonic_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.ultrasonic_ok, out);
    out << ", ";
  }

  // member: obstacle_ok
  {
    out << "obstacle_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.obstacle_ok, out);
    out << ", ";
  }

  // member: obstacle_distance
  {
    out << "obstacle_distance: ";
    rosidl_generator_traits::value_to_yaml(msg.obstacle_distance, out);
    out << ", ";
  }

  // member: fault_bits
  {
    out << "fault_bits: ";
    rosidl_generator_traits::value_to_yaml(msg.fault_bits, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const CarMcuStatus & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: stamp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "stamp:\n";
    to_block_style_yaml(msg.stamp, out, indentation + 2);
  }

  // member: wifi_connect_ok
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "wifi_connect_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.wifi_connect_ok, out);
    out << "\n";
  }

  // member: agent_connect_ok
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "agent_connect_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.agent_connect_ok, out);
    out << "\n";
  }

  // member: imu_ok
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "imu_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.imu_ok, out);
    out << "\n";
  }

  // member: encoder_ok
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "encoder_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.encoder_ok, out);
    out << "\n";
  }

  // member: motor_driver_ok
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "motor_driver_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.motor_driver_ok, out);
    out << "\n";
  }

  // member: cmd_timeout_ok
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "cmd_timeout_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.cmd_timeout_ok, out);
    out << "\n";
  }

  // member: left_wheel_speed
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "left_wheel_speed: ";
    rosidl_generator_traits::value_to_yaml(msg.left_wheel_speed, out);
    out << "\n";
  }

  // member: right_wheel_speed
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "right_wheel_speed: ";
    rosidl_generator_traits::value_to_yaml(msg.right_wheel_speed, out);
    out << "\n";
  }

  // member: left_encoder_ticks
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "left_encoder_ticks: ";
    rosidl_generator_traits::value_to_yaml(msg.left_encoder_ticks, out);
    out << "\n";
  }

  // member: right_encoder_ticks
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "right_encoder_ticks: ";
    rosidl_generator_traits::value_to_yaml(msg.right_encoder_ticks, out);
    out << "\n";
  }

  // member: ultrasonic_ok
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "ultrasonic_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.ultrasonic_ok, out);
    out << "\n";
  }

  // member: obstacle_ok
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "obstacle_ok: ";
    rosidl_generator_traits::value_to_yaml(msg.obstacle_ok, out);
    out << "\n";
  }

  // member: obstacle_distance
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "obstacle_distance: ";
    rosidl_generator_traits::value_to_yaml(msg.obstacle_distance, out);
    out << "\n";
  }

  // member: fault_bits
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "fault_bits: ";
    rosidl_generator_traits::value_to_yaml(msg.fault_bits, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const CarMcuStatus & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace car_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use car_interfaces::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const car_interfaces::msg::CarMcuStatus & msg,
  std::ostream & out, size_t indentation = 0)
{
  car_interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use car_interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const car_interfaces::msg::CarMcuStatus & msg)
{
  return car_interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<car_interfaces::msg::CarMcuStatus>()
{
  return "car_interfaces::msg::CarMcuStatus";
}

template<>
inline const char * name<car_interfaces::msg::CarMcuStatus>()
{
  return "car_interfaces/msg/CarMcuStatus";
}

template<>
struct has_fixed_size<car_interfaces::msg::CarMcuStatus>
  : std::integral_constant<bool, has_fixed_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct has_bounded_size<car_interfaces::msg::CarMcuStatus>
  : std::integral_constant<bool, has_bounded_size<builtin_interfaces::msg::Time>::value> {};

template<>
struct is_message<car_interfaces::msg::CarMcuStatus>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__TRAITS_HPP_
