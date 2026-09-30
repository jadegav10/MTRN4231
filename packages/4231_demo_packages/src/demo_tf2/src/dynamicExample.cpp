#include "dynamicExample.hpp"

DynamicExampleNode::DynamicExampleNode() : Node("dynamic_example_node")
{
    timer_ = create_wall_timer(std::chrono::milliseconds(100), std::bind(&DynamicExampleNode::publishDynamicTransform, this));
    tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
}

void DynamicExampleNode::publishDynamicTransform()
{
    double t = rclcpp::Clock().now().seconds();
    double sin_value = std::sin(t);
    // Publish the static transform
    geometry_msgs::msg::TransformStamped transform_msg;
    transform_msg.header.stamp = this->get_clock()->now();
    transform_msg.header.frame_id = "static_frame";
    transform_msg.child_frame_id = "dynamic_frame";
    transform_msg.transform.translation.x = 1.0;
    transform_msg.transform.translation.y = sin_value;
    transform_msg.transform.translation.z = 0;

    tf2::Quaternion q;
    q.setRPY(0.0, 0.0 , 0.0); 
    transform_msg.transform.rotation.x = q.x();
    transform_msg.transform.rotation.y = q.y();
    transform_msg.transform.rotation.z = q.z();
    transform_msg.transform.rotation.w = q.w();

    tf_broadcaster_->sendTransform(transform_msg);
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<DynamicExampleNode>());
    rclcpp::shutdown();
    return 0;
}