#ifndef DYNAMIC_EXAMPLE_HPP
#define DYNAMIC_EXAMPLE_HPP


#include <rclcpp/rclcpp.hpp>
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/transform_broadcaster.h"
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <cmath>

class DynamicExampleNode : public rclcpp::Node
{
public:
  DynamicExampleNode();

private:
  void publishDynamicTransform();
  rclcpp::TimerBase::SharedPtr timer_;
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

#endif // DYNAMIC_EXAMPLE_HPP