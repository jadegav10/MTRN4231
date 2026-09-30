import launch
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # Declare a launch argument to conditionally launch RViz
        DeclareLaunchArgument(
            'launch_rviz',
            default_value='true',
            description='Launch RViz'
        ),

        # Conditionally launch RViz based on the 'launch_rviz' argument
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen',
            condition=IfCondition(LaunchConfiguration('launch_rviz'))
        )
    ])
