#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>



auto generatePoseMsg(float x,float y, float z,float qx,float qy,float qz,float qw) {
    geometry_msgs::msg::Pose msg;
    msg.orientation.x = qx;
    msg.orientation.y = qy;
    msg.orientation.z = qz;
    msg.orientation.w = qw;
    msg.position.x = x;
    msg.position.y = y;
    msg.position.z = z;
    return msg;
}



int main(int argc, char* argv[])
{
  // Initialize ROS and create the Node
  rclcpp::init(argc, argv);
  auto const node = std::make_shared<rclcpp::Node>(
      "moveit_basic_ur", rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

  // Create a ROS logger
  auto const logger = rclcpp::get_logger("moveit_basic_ur");

  // Create the MoveIt MoveGroup Interface
  using moveit::planning_interface::MoveGroupInterface;
  auto move_group_interface = MoveGroupInterface(node, "ur_manipulator");
  move_group_interface.setPlanningTime(10.0);


  auto const target_pose_1 = generatePoseMsg(0.3,0.2, 0.4,0.0,1.0,0.0,0.0);
  auto const target_pose_2 = generatePoseMsg(0.7,0.2, 0.5,0.0,1.0,0.0,0.0);
  
  auto success = false;

  moveit::planning_interface::MoveGroupInterface::Plan planMessage;
    

  for (int i = 0; i < 10; i ++) {

    //Plan movement to point 1
    move_group_interface.setPoseTarget(target_pose_1);
    success = static_cast<bool>(move_group_interface.plan(planMessage));

    //Execute movement to point 1
    if (success) {
      move_group_interface.execute(planMessage);
    } else {
      RCLCPP_ERROR(logger, "Planning failed!");
    }

    std::cout<<"finished moving to point 1" << std::endl;

    //Plan movement to point 2
    move_group_interface.setPoseTarget(target_pose_2);
    success = static_cast<bool>(move_group_interface.plan(planMessage));

    //Execute movement to point 2
    if (success) {
      move_group_interface.execute(planMessage);
    } else {
      RCLCPP_ERROR(logger, "Planning failed!");
    }

    std::cout<<"finished moving to point 2" << std::endl;

  }



  // Shutdown ROS
  rclcpp::shutdown();
  return 0;
}


