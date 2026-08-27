// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from car_interfaces:msg/CarMcuStatus.idl
// generated code does not contain a copyright notice

#ifndef CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__STRUCT_HPP_
#define CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__car_interfaces__msg__CarMcuStatus __attribute__((deprecated))
#else
# define DEPRECATED__car_interfaces__msg__CarMcuStatus __declspec(deprecated)
#endif

namespace car_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct CarMcuStatus_
{
  using Type = CarMcuStatus_<ContainerAllocator>;

  explicit CarMcuStatus_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : stamp(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->wifi_connect_ok = false;
      this->agent_connect_ok = false;
      this->imu_ok = false;
      this->encoder_ok = false;
      this->motor_driver_ok = false;
      this->cmd_timeout_ok = false;
      this->left_wheel_speed = 0.0f;
      this->right_wheel_speed = 0.0f;
      this->left_encoder_ticks = 0l;
      this->right_encoder_ticks = 0l;
      this->ultrasonic_ok = false;
      this->obstacle_ok = false;
      this->obstacle_distance = 0.0f;
      this->fault_bits = 0ul;
    }
  }

  explicit CarMcuStatus_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : stamp(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->wifi_connect_ok = false;
      this->agent_connect_ok = false;
      this->imu_ok = false;
      this->encoder_ok = false;
      this->motor_driver_ok = false;
      this->cmd_timeout_ok = false;
      this->left_wheel_speed = 0.0f;
      this->right_wheel_speed = 0.0f;
      this->left_encoder_ticks = 0l;
      this->right_encoder_ticks = 0l;
      this->ultrasonic_ok = false;
      this->obstacle_ok = false;
      this->obstacle_distance = 0.0f;
      this->fault_bits = 0ul;
    }
  }

  // field types and members
  using _stamp_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _stamp_type stamp;
  using _wifi_connect_ok_type =
    bool;
  _wifi_connect_ok_type wifi_connect_ok;
  using _agent_connect_ok_type =
    bool;
  _agent_connect_ok_type agent_connect_ok;
  using _imu_ok_type =
    bool;
  _imu_ok_type imu_ok;
  using _encoder_ok_type =
    bool;
  _encoder_ok_type encoder_ok;
  using _motor_driver_ok_type =
    bool;
  _motor_driver_ok_type motor_driver_ok;
  using _cmd_timeout_ok_type =
    bool;
  _cmd_timeout_ok_type cmd_timeout_ok;
  using _left_wheel_speed_type =
    float;
  _left_wheel_speed_type left_wheel_speed;
  using _right_wheel_speed_type =
    float;
  _right_wheel_speed_type right_wheel_speed;
  using _left_encoder_ticks_type =
    int32_t;
  _left_encoder_ticks_type left_encoder_ticks;
  using _right_encoder_ticks_type =
    int32_t;
  _right_encoder_ticks_type right_encoder_ticks;
  using _ultrasonic_ok_type =
    bool;
  _ultrasonic_ok_type ultrasonic_ok;
  using _obstacle_ok_type =
    bool;
  _obstacle_ok_type obstacle_ok;
  using _obstacle_distance_type =
    float;
  _obstacle_distance_type obstacle_distance;
  using _fault_bits_type =
    uint32_t;
  _fault_bits_type fault_bits;

  // setters for named parameter idiom
  Type & set__stamp(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->stamp = _arg;
    return *this;
  }
  Type & set__wifi_connect_ok(
    const bool & _arg)
  {
    this->wifi_connect_ok = _arg;
    return *this;
  }
  Type & set__agent_connect_ok(
    const bool & _arg)
  {
    this->agent_connect_ok = _arg;
    return *this;
  }
  Type & set__imu_ok(
    const bool & _arg)
  {
    this->imu_ok = _arg;
    return *this;
  }
  Type & set__encoder_ok(
    const bool & _arg)
  {
    this->encoder_ok = _arg;
    return *this;
  }
  Type & set__motor_driver_ok(
    const bool & _arg)
  {
    this->motor_driver_ok = _arg;
    return *this;
  }
  Type & set__cmd_timeout_ok(
    const bool & _arg)
  {
    this->cmd_timeout_ok = _arg;
    return *this;
  }
  Type & set__left_wheel_speed(
    const float & _arg)
  {
    this->left_wheel_speed = _arg;
    return *this;
  }
  Type & set__right_wheel_speed(
    const float & _arg)
  {
    this->right_wheel_speed = _arg;
    return *this;
  }
  Type & set__left_encoder_ticks(
    const int32_t & _arg)
  {
    this->left_encoder_ticks = _arg;
    return *this;
  }
  Type & set__right_encoder_ticks(
    const int32_t & _arg)
  {
    this->right_encoder_ticks = _arg;
    return *this;
  }
  Type & set__ultrasonic_ok(
    const bool & _arg)
  {
    this->ultrasonic_ok = _arg;
    return *this;
  }
  Type & set__obstacle_ok(
    const bool & _arg)
  {
    this->obstacle_ok = _arg;
    return *this;
  }
  Type & set__obstacle_distance(
    const float & _arg)
  {
    this->obstacle_distance = _arg;
    return *this;
  }
  Type & set__fault_bits(
    const uint32_t & _arg)
  {
    this->fault_bits = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    car_interfaces::msg::CarMcuStatus_<ContainerAllocator> *;
  using ConstRawPtr =
    const car_interfaces::msg::CarMcuStatus_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<car_interfaces::msg::CarMcuStatus_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<car_interfaces::msg::CarMcuStatus_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      car_interfaces::msg::CarMcuStatus_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<car_interfaces::msg::CarMcuStatus_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      car_interfaces::msg::CarMcuStatus_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<car_interfaces::msg::CarMcuStatus_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<car_interfaces::msg::CarMcuStatus_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<car_interfaces::msg::CarMcuStatus_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__car_interfaces__msg__CarMcuStatus
    std::shared_ptr<car_interfaces::msg::CarMcuStatus_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__car_interfaces__msg__CarMcuStatus
    std::shared_ptr<car_interfaces::msg::CarMcuStatus_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const CarMcuStatus_ & other) const
  {
    if (this->stamp != other.stamp) {
      return false;
    }
    if (this->wifi_connect_ok != other.wifi_connect_ok) {
      return false;
    }
    if (this->agent_connect_ok != other.agent_connect_ok) {
      return false;
    }
    if (this->imu_ok != other.imu_ok) {
      return false;
    }
    if (this->encoder_ok != other.encoder_ok) {
      return false;
    }
    if (this->motor_driver_ok != other.motor_driver_ok) {
      return false;
    }
    if (this->cmd_timeout_ok != other.cmd_timeout_ok) {
      return false;
    }
    if (this->left_wheel_speed != other.left_wheel_speed) {
      return false;
    }
    if (this->right_wheel_speed != other.right_wheel_speed) {
      return false;
    }
    if (this->left_encoder_ticks != other.left_encoder_ticks) {
      return false;
    }
    if (this->right_encoder_ticks != other.right_encoder_ticks) {
      return false;
    }
    if (this->ultrasonic_ok != other.ultrasonic_ok) {
      return false;
    }
    if (this->obstacle_ok != other.obstacle_ok) {
      return false;
    }
    if (this->obstacle_distance != other.obstacle_distance) {
      return false;
    }
    if (this->fault_bits != other.fault_bits) {
      return false;
    }
    return true;
  }
  bool operator!=(const CarMcuStatus_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct CarMcuStatus_

// alias to use template instance with default allocator
using CarMcuStatus =
  car_interfaces::msg::CarMcuStatus_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace car_interfaces

#endif  // CAR_INTERFACES__MSG__DETAIL__CAR_MCU_STATUS__STRUCT_HPP_
