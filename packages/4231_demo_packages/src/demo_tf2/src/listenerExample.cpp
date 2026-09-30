#include "listenerExample.hpp"

TF2Listener::TF2Listener() : Node("tf2_listener_node")
{
    tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
    tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

    timer_ = this->create_wall_timer( std::chrono::milliseconds(200), std::bind(&TF2Listener::tfCallback, this));
}

void TF2Listener::tfCallback()
{
  // Check if the transformation is between "map" and "dynamic_frame"
    std::string fromFrameRel = "map";
    std::string toFrameRel = "dynamic_frame";

    geometry_msgs::msg::TransformStamped t;

    try {
        t = tf_buffer_->lookupTransform( toFrameRel, fromFrameRel, tf2::TimePointZero);
    } catch (const tf2::TransformException & ex) {
          RCLCPP_INFO( this->get_logger(), "Could not transform %s to %s: %s", toFrameRel.c_str(), fromFrameRel.c_str(), ex.what());
          return;
    }

    std::cout << "transformation found" <<std::endl;
    std::cout << " [ " << t.transform.translation.x 
                << " , " << t.transform.translation.y 
                << " , " << t.transform.translation.z 
                << std::endl;


}

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<TF2Listener>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}