import launch
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription, LogInfo
from launch.launch_description_sources import AnyLaunchDescriptionSource
from launch.substitutions import ThisLaunchFileDir

def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument('namespace_shape1', default_value='shape1', description='Namespace for shape1 marker node'),
        DeclareLaunchArgument('namespace_shape2', default_value='shape2', description='Namespace for shape2 marker node'),
        
        Node(
            package='demo_launch_param',
            executable='marker_node',
            name='shape1_marker_node',
            namespace=LaunchConfiguration('namespace_shape1'),
            output='screen',
            remappings=[('/shape1', LaunchConfiguration('namespace_shape1'))],
            parameters=[{'x': 1.0, 'color': [0.0, 0.0, 1.0, 1.0]}],  # Blue marker for shape1
        ),
        
        Node(
            package='demo_launch_param',
            executable='marker_node',
            name='shape2_marker_node',
            namespace=LaunchConfiguration('namespace_shape2'),
            output='screen',
            remappings=[('/shape2', LaunchConfiguration('namespace_shape2'))],
            parameters=[{'x': 2.0, 'color': [0.0, 1.0, 0.0, 1.0]}],  # Green marker for shape2
        ),

        IncludeLaunchDescription(
            AnyLaunchDescriptionSource(
                [ThisLaunchFileDir(), '/rviz_launch.py']
            )
        )
    ])
