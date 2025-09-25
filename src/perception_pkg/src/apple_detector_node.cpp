#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/string.hpp"

class AppleDetectorNode : public rclcpp::Node {
public:
  AppleDetectorNode() : Node("apple_detector_node") {
    image_sub_ = this->create_subscription<sensor_msgs::msg::Image>(
      "/camera/image_raw", 10,
      std::bind(&AppleDetectorNode::image_callback, this, std::placeholders::_1));
    apple_pub_ = this->create_publisher<std_msgs::msg::String>(
      "/perception/apples", 10);
    RCLCPP_INFO(this->get_logger(), "Apple Detector Node started.");
  }

private:
  void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
    (void)msg; // Unused for now
    RCLCPP_INFO(this->get_logger(), "Received image!");
    auto dummy_msg = std_msgs::msg::String();
    dummy_msg.data = "Detected 0 apples (stub)";
    apple_pub_->publish(dummy_msg);
  }

  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr apple_pub_;
};

int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AppleDetectorNode>());
  rclcpp::shutdown();
  return 0;
}
