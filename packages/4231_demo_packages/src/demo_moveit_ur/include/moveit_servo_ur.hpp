#ifndef MOVEIT_SERVO_UR_HPP
#define MOVEIT_SERVO_UR_HPP

#include <rclcpp/rclcpp.hpp>
#include "std_srvs/srv/trigger.hpp"
#include "controller_manager_msgs/srv/switch_controller.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "control_msgs/msg/joint_jog.hpp"


class moveit_servo_ur : public rclcpp::Node
{
public:
    moveit_servo_ur();

private:

    int state = 0;
    int count = 0;

    bool switched_controllers_flag = false;
    bool actived_servo_flag = false;

    void wait_for_services();
    void routine_callback();

    void request_activate_servo();
    void request_switch_controllers();

    void client_servo_response_callback(rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future);
    void client_switch_controller_response_callback(rclcpp::Client<controller_manager_msgs::srv::SwitchController>::SharedFuture future);

    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr client_servo_trigger_;
    rclcpp::Client<controller_manager_msgs::srv::SwitchController>::SharedPtr client_switch_controller_;
    rclcpp::Publisher<control_msgs::msg::JointJog>::SharedPtr joint_cmd_pub_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr twist_cmd_pub_;
    
    rclcpp::TimerBase::SharedPtr timer_routine_;
};

#endif // MOVEIT_SERVO_UR_HPP