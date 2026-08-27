// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from car_interfaces:msg/CarMcuStatus.idl
// generated code does not contain a copyright notice
#include "car_interfaces/msg/detail/car_mcu_status__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `stamp`
#include "builtin_interfaces/msg/detail/time__functions.h"

bool
car_interfaces__msg__CarMcuStatus__init(car_interfaces__msg__CarMcuStatus * msg)
{
  if (!msg) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__init(&msg->stamp)) {
    car_interfaces__msg__CarMcuStatus__fini(msg);
    return false;
  }
  // wifi_connect_ok
  // agent_connect_ok
  // imu_ok
  // encoder_ok
  // motor_driver_ok
  // cmd_timeout_ok
  // left_wheel_speed
  // right_wheel_speed
  // left_encoder_ticks
  // right_encoder_ticks
  // ultrasonic_ok
  // obstacle_ok
  // obstacle_distance
  // fault_bits
  return true;
}

void
car_interfaces__msg__CarMcuStatus__fini(car_interfaces__msg__CarMcuStatus * msg)
{
  if (!msg) {
    return;
  }
  // stamp
  builtin_interfaces__msg__Time__fini(&msg->stamp);
  // wifi_connect_ok
  // agent_connect_ok
  // imu_ok
  // encoder_ok
  // motor_driver_ok
  // cmd_timeout_ok
  // left_wheel_speed
  // right_wheel_speed
  // left_encoder_ticks
  // right_encoder_ticks
  // ultrasonic_ok
  // obstacle_ok
  // obstacle_distance
  // fault_bits
}

bool
car_interfaces__msg__CarMcuStatus__are_equal(const car_interfaces__msg__CarMcuStatus * lhs, const car_interfaces__msg__CarMcuStatus * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__are_equal(
      &(lhs->stamp), &(rhs->stamp)))
  {
    return false;
  }
  // wifi_connect_ok
  if (lhs->wifi_connect_ok != rhs->wifi_connect_ok) {
    return false;
  }
  // agent_connect_ok
  if (lhs->agent_connect_ok != rhs->agent_connect_ok) {
    return false;
  }
  // imu_ok
  if (lhs->imu_ok != rhs->imu_ok) {
    return false;
  }
  // encoder_ok
  if (lhs->encoder_ok != rhs->encoder_ok) {
    return false;
  }
  // motor_driver_ok
  if (lhs->motor_driver_ok != rhs->motor_driver_ok) {
    return false;
  }
  // cmd_timeout_ok
  if (lhs->cmd_timeout_ok != rhs->cmd_timeout_ok) {
    return false;
  }
  // left_wheel_speed
  if (lhs->left_wheel_speed != rhs->left_wheel_speed) {
    return false;
  }
  // right_wheel_speed
  if (lhs->right_wheel_speed != rhs->right_wheel_speed) {
    return false;
  }
  // left_encoder_ticks
  if (lhs->left_encoder_ticks != rhs->left_encoder_ticks) {
    return false;
  }
  // right_encoder_ticks
  if (lhs->right_encoder_ticks != rhs->right_encoder_ticks) {
    return false;
  }
  // ultrasonic_ok
  if (lhs->ultrasonic_ok != rhs->ultrasonic_ok) {
    return false;
  }
  // obstacle_ok
  if (lhs->obstacle_ok != rhs->obstacle_ok) {
    return false;
  }
  // obstacle_distance
  if (lhs->obstacle_distance != rhs->obstacle_distance) {
    return false;
  }
  // fault_bits
  if (lhs->fault_bits != rhs->fault_bits) {
    return false;
  }
  return true;
}

bool
car_interfaces__msg__CarMcuStatus__copy(
  const car_interfaces__msg__CarMcuStatus * input,
  car_interfaces__msg__CarMcuStatus * output)
{
  if (!input || !output) {
    return false;
  }
  // stamp
  if (!builtin_interfaces__msg__Time__copy(
      &(input->stamp), &(output->stamp)))
  {
    return false;
  }
  // wifi_connect_ok
  output->wifi_connect_ok = input->wifi_connect_ok;
  // agent_connect_ok
  output->agent_connect_ok = input->agent_connect_ok;
  // imu_ok
  output->imu_ok = input->imu_ok;
  // encoder_ok
  output->encoder_ok = input->encoder_ok;
  // motor_driver_ok
  output->motor_driver_ok = input->motor_driver_ok;
  // cmd_timeout_ok
  output->cmd_timeout_ok = input->cmd_timeout_ok;
  // left_wheel_speed
  output->left_wheel_speed = input->left_wheel_speed;
  // right_wheel_speed
  output->right_wheel_speed = input->right_wheel_speed;
  // left_encoder_ticks
  output->left_encoder_ticks = input->left_encoder_ticks;
  // right_encoder_ticks
  output->right_encoder_ticks = input->right_encoder_ticks;
  // ultrasonic_ok
  output->ultrasonic_ok = input->ultrasonic_ok;
  // obstacle_ok
  output->obstacle_ok = input->obstacle_ok;
  // obstacle_distance
  output->obstacle_distance = input->obstacle_distance;
  // fault_bits
  output->fault_bits = input->fault_bits;
  return true;
}

car_interfaces__msg__CarMcuStatus *
car_interfaces__msg__CarMcuStatus__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  car_interfaces__msg__CarMcuStatus * msg = (car_interfaces__msg__CarMcuStatus *)allocator.allocate(sizeof(car_interfaces__msg__CarMcuStatus), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(car_interfaces__msg__CarMcuStatus));
  bool success = car_interfaces__msg__CarMcuStatus__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
car_interfaces__msg__CarMcuStatus__destroy(car_interfaces__msg__CarMcuStatus * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    car_interfaces__msg__CarMcuStatus__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
car_interfaces__msg__CarMcuStatus__Sequence__init(car_interfaces__msg__CarMcuStatus__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  car_interfaces__msg__CarMcuStatus * data = NULL;

  if (size) {
    data = (car_interfaces__msg__CarMcuStatus *)allocator.zero_allocate(size, sizeof(car_interfaces__msg__CarMcuStatus), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = car_interfaces__msg__CarMcuStatus__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        car_interfaces__msg__CarMcuStatus__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
car_interfaces__msg__CarMcuStatus__Sequence__fini(car_interfaces__msg__CarMcuStatus__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      car_interfaces__msg__CarMcuStatus__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

car_interfaces__msg__CarMcuStatus__Sequence *
car_interfaces__msg__CarMcuStatus__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  car_interfaces__msg__CarMcuStatus__Sequence * array = (car_interfaces__msg__CarMcuStatus__Sequence *)allocator.allocate(sizeof(car_interfaces__msg__CarMcuStatus__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = car_interfaces__msg__CarMcuStatus__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
car_interfaces__msg__CarMcuStatus__Sequence__destroy(car_interfaces__msg__CarMcuStatus__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    car_interfaces__msg__CarMcuStatus__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
car_interfaces__msg__CarMcuStatus__Sequence__are_equal(const car_interfaces__msg__CarMcuStatus__Sequence * lhs, const car_interfaces__msg__CarMcuStatus__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!car_interfaces__msg__CarMcuStatus__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
car_interfaces__msg__CarMcuStatus__Sequence__copy(
  const car_interfaces__msg__CarMcuStatus__Sequence * input,
  car_interfaces__msg__CarMcuStatus__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(car_interfaces__msg__CarMcuStatus);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    car_interfaces__msg__CarMcuStatus * data =
      (car_interfaces__msg__CarMcuStatus *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!car_interfaces__msg__CarMcuStatus__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          car_interfaces__msg__CarMcuStatus__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!car_interfaces__msg__CarMcuStatus__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
