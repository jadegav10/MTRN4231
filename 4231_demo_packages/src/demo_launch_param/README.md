# demo_launch_param

This package provides a demonstration of the some of core concepts about the launch files and parameter. The default node spawns a red ball in rviz at (0,0,0). Through the use of a launch file this node is then initalised 2 times under different namespaces with different parameters pass in for each name space. The launch file also calls a seperate rviz launch file which can be toggled from command line. 

Examples are also provided of how to launch nodes with parameters from command lines. As parameters function on a seperate server they can be get and set dynamically by making param calls to the server. More information at https://docs.ros.org/en/humble/Tutorials/Beginner-CLI-Tools/Understanding-ROS2-Parameters/Understanding-ROS2-Parameters.html.



Nodes included:
* marker_node


### Launching the launch file

    ros2 launch demo_launch_param marker_launch.py

This launch file spawns the node twice under seperate name spaces. It additionally launches rviz but does not load an rviz configuration file. Make sure to add the markers, this is easily done by adding by topic. 


### Launching the launch file without rviz 

    ros2 launch demo_launch_param marker_launch.py launch_rviz:=false

Demonstation of command line interfaces with launch files. 


### Launching the marker node with default parameters

    ros2 run demo_launch_param marker_node

Spawns a red marker at (0,0,0) default parameters

### Launching the marker node with command line parameters

    ros2 run demo_launch_param marker_node --ros-args -p color:="[0.5, 0.0, 0.5, 1.0]" -p x:=4.0

Spawns a purple marker at x = 4.