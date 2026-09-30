#include <memory>
#include <string>
#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>

auto generateCollisionObject(float sx,float sy, float sz, float x, float y, float z, std::string frame_id, std::string id) {
  moveit_msgs::msg::CollisionObject collision_object;
  collision_object.header.frame_id = frame_id;
  collision_object.id = id;
  shape_msgs::msg::SolidPrimitive primitive;

  primitive.type = primitive.BOX;
  primitive.dimensions.resize(3);
  primitive.dimensions[primitive.BOX_X] = sx;
  primitive.dimensions[primitive.BOX_Y] = sy;
  primitive.dimensions[primitive.BOX_Z] = sz;

  geometry_msgs::msg::Pose box_pose;
  box_pose.orientation.w = 1.0; 
  box_pose.position.x = x;
  box_pose.position.y = y;
  box_pose.position.z = z;

  collision_object.primitives.push_back(primitive);
  collision_object.primitive_poses.push_back(box_pose);
  collision_object.operation = collision_object.ADD;

  return collision_object;
}

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
      "moveit_avoid_ur", rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

  // Create a ROS logger
  auto const logger = rclcpp::get_logger("moveit_avoid_ur");

  // Create the MoveIt MoveGroup Interface
  using moveit::planning_interface::MoveGroupInterface;
  auto move_group_interface = MoveGroupInterface(node, "ur_manipulator");
  move_group_interface.setPlanningTime(10.0);

  std::string frame_id = move_group_interface.getPlanningFrame();

  //size, pos, frame, id
  auto collision_object = generateCollisionObject( 0.08, 0.6, 0.57, 0.5, 0.2, 0.2, frame_id, "box");
  auto col_object_table = generateCollisionObject( 2.4, 1.2, 0.04, 0.85, 0.25, -0.03, frame_id, "table");
  auto col_object_backWall = generateCollisionObject( 2.4, 0.04, 1.0, 0.85, -0.45, 0.5, frame_id, "backWall");
  auto col_object_sideWall = generateCollisionObject( 0.04, 1.2, 1.0, -0.45, 0.25, 0.5, frame_id, "sideWall");

  // Add the collision object to the scene
  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;
  planning_scene_interface.applyCollisionObject(collision_object);
  planning_scene_interface.applyCollisionObject(col_object_table);
  planning_scene_interface.applyCollisionObject(col_object_backWall);
  planning_scene_interface.applyCollisionObject(col_object_sideWall);
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
