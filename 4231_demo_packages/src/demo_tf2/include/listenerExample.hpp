#ifndef TF2_LISTENER_HPP
#define TF2_LISTENER_HPP

#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "tf2/exceptions.h"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"
#include "geometry_msgs/msg/transform_stamped.hpp"

class TF2Listener : public rclcpp::Node
{
public:
  TF2Listener();

private:
  void tfCallback();
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_{nullptr};
  std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
  rclcpp::TimerBase::SharedPtr timer_;
};

#endif // TF2_LISTENER_HPP
