#include <memory>
#include <string>
#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_visual_tools/moveit_visual_tools.h>
#include <thread>

// For enabling the constraint path planning, edit the ompl_planning.yaml file to include:
//  projection_evaluator: joints(shoulder_pan_joint,shoulder_lift_joint)
//  enforce_constrained_state_space: true

// For more info, see https://moveit.picknik.ai/humble/doc/how_to_guides/using_ompl_constrained_planning/ompl_constrained_planning.html?highlight=box_constraint%20weight

// Thanks to Jasper for testing and confirmation of this node

// ROS
#include <rclcpp/rclcpp.hpp>

// C++
#include <string>
#include <vector>

// Msgs
#include <geometry_msgs/msg/vector3.hpp>
#include <tf2/LinearMath/Quaternion.h>

using namespace std::chrono_literals;

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

double radiansToDegrees(double radians) {
    return radians * 180.0 / M_PI_4;
}

namespace rvt = rviz_visual_tools;


int main(int argc, char* argv[])
{
  // Initialize ROS and create the Node
  rclcpp::init(argc, argv);
  auto const node = std::make_shared<rclcpp::Node>("hello_moveit", rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

  // Create the MoveIt MoveGroup Interface
  using moveit::planning_interface::MoveGroupInterface;
  auto move_group_interface = MoveGroupInterface(node, "ur_manipulator");
  move_group_interface.setPlanningTime(10.0);

  std::string frame_id = move_group_interface.getPlanningFrame();

  // Testing to see if joint names are right
  std::cout << "These are the avalible joints: " << std::endl;
  auto jointNames = move_group_interface.getJointNames();
  for (std::string i: jointNames) {
    std::cout << i << std::endl;
  }


  //size, pos, frame, id
  auto collision_object = generateCollisionObject( 0.08, 0.6, 0.57, 0.5, 0.2, 0.2, frame_id, "box");
  auto col_object_table = generateCollisionObject( 2.4, 1.2, 0.04, 0.85, 0.25, -0.03, frame_id, "table");
  auto col_object_backWall = generateCollisionObject( 2.4, 0.04, 1.0, 0.85, -0.45, 0.5, frame_id, "backWall");
  auto col_object_sideWall = generateCollisionObject( 0.04, 1.2, 1.0, -0.45, 0.25, 0.5, frame_id, "sideWall");

  // Add the collision object to the scene
  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;
  //  planning_scene_interface.applyCollisionObject(collision_object);
  planning_scene_interface.applyCollisionObject(col_object_table);
  planning_scene_interface.applyCollisionObject(col_object_backWall);
  //  planning_scene_interface.applyCollisionObject(col_object_sideWall);
  

  // Create a ROS logger
  auto const logger = rclcpp::get_logger("hello_moveit");

  // We spin up a SingleThreadedExecutor so MoveItVisualTools interact with ROS
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  auto spinner = std::thread([&executor]() { executor.spin(); });
  
  // Create visualisation tool to use with rviz
  std::cout << "Create visualisation tool to use with rviz" << std::endl;
  auto moveit_visual_tools =
      moveit_visual_tools::MoveItVisualTools{ node, "base_link", rviz_visual_tools::RVIZ_MARKER_TOPIC,
                                              move_group_interface.getRobotModel() };

  moveit_visual_tools.deleteAllMarkers();
  moveit_visual_tools.loadRemoteControl();

  ////////////////////////////////////////////////////////////////////////////////

  // Set a target Pose
  auto target_pose = generatePoseMsg(0.3,0.3, 0.3,0.0,1.0,0.0,0.0);
  move_group_interface.setPoseTarget(target_pose);


  moveit::planning_interface::MoveGroupInterface::Plan planMessage;
  auto success = static_cast<bool>(move_group_interface.plan(planMessage));
    

  // Execute the plan
  if (success) {
    move_group_interface.execute(planMessage);
  } else {
    RCLCPP_ERROR(logger, "Planning failed! First");
  }


  target_pose.position.x += 0.0;
  target_pose.position.y += 0.3;
  target_pose.position.z += 0.3;

  std::cout << target_pose.position.x << std::endl;
  std::cout << target_pose.position.y << std::endl;
  std::cout << target_pose.position.z << std::endl;

  auto current_pose = move_group_interface.getCurrentPose();
  moveit_visual_tools.publishSphere(current_pose.pose, rviz_visual_tools::RED, 0.05);
  moveit_visual_tools.publishSphere(target_pose, rviz_visual_tools::GREEN, 0.05);
  moveit_visual_tools.trigger();


  ////////////////////////////////////// BOX CONSTRAINT ////////////////////////////////////////////////////////////////////

  
    // Let's try the simple box constraints first!
  moveit_msgs::msg::PositionConstraint box_constraint;
  box_constraint.header.frame_id = move_group_interface.getPoseReferenceFrame();
  box_constraint.link_name = move_group_interface.getEndEffectorLink();
  shape_msgs::msg::SolidPrimitive box;
  box.type = shape_msgs::msg::SolidPrimitive::BOX;
  box.dimensions = { 0.4, 0.8, 0.7 };
  box_constraint.constraint_region.primitives.emplace_back(box);


  geometry_msgs::msg::Pose box_pose;
  box_pose.position.x = target_pose.position.x;
  box_pose.position.y = 0.45;
  box_pose.position.z = 0.45;
  box_constraint.constraint_region.primitive_poses.emplace_back(box_pose);
  box_constraint.weight = 1.0; // Weird this doesnt effect anything 

  // Visualize the box constraint
  Eigen::Vector3d box_point_1(box_pose.position.x - box.dimensions[0] / 2, box_pose.position.y - box.dimensions[1] / 2,
                              box_pose.position.z - box.dimensions[2] / 2);
  Eigen::Vector3d box_point_2(box_pose.position.x + box.dimensions[0] / 2, box_pose.position.y + box.dimensions[1] / 2,
                              box_pose.position.z + box.dimensions[2] / 2);
  moveit_visual_tools.publishCuboid(box_point_1, box_point_2, rviz_visual_tools::TRANSLUCENT_DARK);
  moveit_visual_tools.trigger();

  
  moveit_msgs::msg::Constraints box_constraints;
  box_constraints.position_constraints.emplace_back(box_constraint);
  move_group_interface.setPathConstraints(box_constraints);
  
  


  //////////////////////////////////////////////////////// PLANE CONTRAINT ////////////////////////////////////////////////////////////
  // Can produce weird results and may not be working directly, positioning the plane is difficult
  // However this code is left here to aid in further development
  
  /*
  moveit_msgs::msg::PositionConstraint plane_constraint;
  plane_constraint.header.frame_id = move_group_interface.getPoseReferenceFrame();
  plane_constraint.link_name = move_group_interface.getEndEffectorLink();
  shape_msgs::msg::SolidPrimitive plane;
  plane.type = shape_msgs::msg::SolidPrimitive::BOX;
  plane.dimensions = { 1.0, 0.0005, 1.0 };
  plane_constraint.constraint_region.primitives.emplace_back(plane);

  geometry_msgs::msg::Pose plane_pose;
  plane_pose.position.x = current_pose.pose.position.x;
  plane_pose.position.y = current_pose.pose.position.y;
  plane_pose.position.z = current_pose.pose.position.z;
  
  //plane_pose.orientation.x = sin(M_PI_4 / 2);
  //plane_pose.orientation.y = 0.0;
  //plane_pose.orientation.z = 0.0;
  //plane_pose.orientation.w = cos(M_PI_4 / 2);


  // SEE http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/ for an idea of whats happening 

  // Define vector between current and target point
  Eigen::Vector3d betweenPoints;
  betweenPoints.x() = target_pose.position.x - current_pose.pose.position.x;
  betweenPoints.y() = target_pose.position.y - current_pose.pose.position.y;
  betweenPoints.z() = target_pose.position.z - current_pose.pose.position.z;

  std::cout << "My Between Vector Start is " << std::endl;
  std::cout << betweenPoints.x() << std::endl;
  std::cout << betweenPoints.y() << std::endl;
  std::cout << betweenPoints.z() << std::endl;

  Eigen::Vector3d rotationVector;
  auto xDif = betweenPoints.x();
  auto yDif = betweenPoints.y();
  auto zDif = betweenPoints.z();

  // We then find the rotation axis for the quaternion by taking a vector that is pependicular to the betweenPoints vector and has 0 z component
  // If betweenPoints z component is already 0, then there is no need for any rotation 
  
  if (zDif == 0) {
    plane_pose.orientation.x = 0.0;
    plane_pose.orientation.y = 0.0;
    plane_pose.orientation.z = 0.0;
    plane_pose.orientation.w = 0.0;
  } else {
    // Otherwise define the perpendicular vector as follows
    if ((xDif =! 0) || (yDif =! 0)) {
      rotationVector.x() = betweenPoints.y();
      rotationVector.y() = -betweenPoints.x();
      rotationVector.z() = 0.0;
    } else {
      // If target and current are above or below each other

      // Need to cosider edge case where points are in line but above or below each other (What axis do you rotate by x or y?)
      rotationVector.x() = 0.0;
      rotationVector.y()= 1.0;
      rotationVector.z() = 0.0;

    }
  }
  
  // Find angle of rotation needed
  Eigen::Vector3d normalVector(0.0, 0.0, 1.0);

  auto opposite = sqrt(pow(betweenPoints.x(), 2) + pow(betweenPoints.y(), 2));
  auto myAngle = std::atan(opposite/betweenPoints.z());

  //plane_pose.orientation.x = rotationVector.x;
  //plane_pose.orientation.y = rotationVector.y;
  //plane_pose.orientation.z = rotationVector.z;
  //plane_pose.orientation.w = 0.0;


  plane_pose.orientation.x = rotationVector.x() * sin(M_PI_4 / 2);
  plane_pose.orientation.y = 0.0;
  plane_pose.orientation.z = 0.0;
  plane_pose.orientation.w = -cos(M_PI_4 / 2);


  // NEEDS to be a unit vector
  plane_pose.orientation.x = 1.0 * sin(myAngle / 2);//rotationVector.x() * sin(myAngle / 2);
  plane_pose.orientation.y = 0.0;//rotationVector.y() * sin(myAngle / 2);
  plane_pose.orientation.z = 0.0;//rotationVector.z() * sin(myAngle / 2);
  plane_pose.orientation.w = -cos(myAngle / 2);
  
  plane_constraint.constraint_region.primitive_poses.emplace_back(plane_pose);
  plane_constraint.weight = 1.0;

  moveit_msgs::msg::Constraints plane_constraints;
  plane_constraints.position_constraints.emplace_back(plane_constraint);
  plane_constraints.name = "use_equality_constraints";
  move_group_interface.setPathConstraints(plane_constraints);

  auto d = sqrt(pow(target_pose.position.y, 2) + pow(target_pose.position.z, 2));


  Eigen::Vector3d normal(plane_pose.orientation.x, plane_pose.orientation.y, plane_pose.orientation.z);
  //tf2::Quaternion myQuaternion;
  //myQuaternion.
  //perpendicularVector.x = vector.y;
  //perpendicularVector.y = -vector.x;
  //perpendicularVector.z = 0.0;
  
  if (plane_pose.position.x != 0 || plane_pose.position.y != 0) {
    Eigen::Vector3d normal(plane_pose.position.y, -plane_pose.position.x, 0);
  } else {
    Eigen::Vector3d normal(0, plane_pose.position.z, -plane_pose.position.y);
  }

  if (plane_pose.position.x == 0 && plane_pose.position.y == 0 && plane_pose.position.z == 0){
    Eigen::Vector3d normal(1, 0, 0);
  }
  


  //Eigen::Vector3d normal(0, 0, 1);
  moveit_visual_tools.publishNormalAndDistancePlane(normal, 0, rviz_visual_tools::TRANSLUCENT_DARK);
  moveit_visual_tools.trigger();
  
  */

 //////////////////////////////////////////////////////////// LINE CONSTRAINT ///////////////////////////////////////////////////////////////////
  // Can produce weird results and may not be working directly.
  // However this code is left here to aid in further development
  /*
  // Building on the previous constraint, we can make it a line, by also reducing the dimension of the box in the x-direction.
  moveit_msgs::msg::PositionConstraint line_constraint;
  line_constraint.header.frame_id = move_group_interface.getPoseReferenceFrame();
  line_constraint.link_name = move_group_interface.getEndEffectorLink();
  shape_msgs::msg::SolidPrimitive line;
  line.type = shape_msgs::msg::SolidPrimitive::BOX;
  line.dimensions = { 0.0005, 0.0005, 1.0 };
  line_constraint.constraint_region.primitives.emplace_back(line);

  tf2::Quaternion myQuaternion;
  auto vectorX = target_pose.position.x - current_pose.pose.position.x;
  auto vectorY = target_pose.position.y - current_pose.pose.position.y;
  auto vectorZ = target_pose.position.z - current_pose.pose.position.z;
  //auto rollRadians = sin((target_pose.position.z - current_pose.pose.position.z) / (target_pose.position.y - current_pose.pose.position.y)) * (M_PI_4/180.0);
  //auto yawRadians = sin((target_pose.position.y - current_pose.pose.position.y) / (target_pose.position.x - current_pose.pose.position.x)) * (M_PI_4/180.0);
  //auto pitchRadians = sin((target_pose.position.z - current_pose.pose.position.z) / (target_pose.position.x - current_pose.pose.position.x)) * (M_PI_4/180.0);
  //myQuaternion.setRPY( rollRadians, pitchRadians, yawRadians);

  double angleX = atan2(vectorY, vectorX);
  double angleY = atan2(vectorZ, sqrt(vectorX * vectorX + vectorY * vectorY));
  double angleZ = atan2(vectorY, vectorZ);

  angleX = radiansToDegrees(angleX);
  angleY = radiansToDegrees(angleY);
  angleZ = radiansToDegrees(angleZ);
  
  myQuaternion.setRPY( angleX, angleZ, angleY);

  std::cout << "x: " << myQuaternion.getX() << " y: " << myQuaternion.getY() << " z: " << myQuaternion.getZ() << " w: " << myQuaternion.getW() << std::endl;

  geometry_msgs::msg::Pose line_pose;
  line_pose.position.x = current_pose.pose.position.x;
  line_pose.position.y = current_pose.pose.position.y;
  line_pose.position.z = current_pose.pose.position.z;
  line_pose.orientation.x = 0.0;//myQuaternion.getX();//sin(M_PI_4 / 2);
  line_pose.orientation.y = 0.0;//myQuaternion.getY();//0.0;
  line_pose.orientation.z = 0.0;//myQuaternion.getZ();//0.0;
  line_pose.orientation.w = 0.0;//myQuaternion.getW();//cos(M_PI_4 / 2);
  line_constraint.constraint_region.primitive_poses.emplace_back(line_pose);
  line_constraint.weight = 1.0;

  // Visualize the constraint
  moveit_visual_tools.publishLine(current_pose.pose.position, target_pose.position,
                                  rviz_visual_tools::TRANSLUCENT_DARK);
  moveit_visual_tools.trigger();

  moveit_msgs::msg::Constraints line_constraints;
  line_constraints.position_constraints.emplace_back(line_constraint);
  line_constraints.name = "use_equality_constraints";
  move_group_interface.setPathConstraints(line_constraints);
  */


 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  move_group_interface.setPlannerId("RRTConnectkConfigDefault");    // Potentially change this for better or faster results (See avalible planners in ompl_planning.yaml)
  move_group_interface.setPoseTarget(target_pose);
  move_group_interface.setPlanningTime(30.0);
  success = static_cast<bool>(move_group_interface.plan(planMessage));
  
  std::cout << "Sleeping Now!" << std::endl;
  //sleep(5);

  if (success) {
    move_group_interface.execute(planMessage);
  } else {
    RCLCPP_ERROR(logger, "Planning failed in Constraint!");
  }


  moveit_visual_tools.deleteAllMarkers();
  moveit_visual_tools.trigger();
  move_group_interface.clearPathConstraints();

  // Shutdown ROS
  rclcpp::shutdown();
  spinner.join();
  return 0;
}
