# demo_tf2

This package provides a demonstration of the some of core concepts about the TF2 package. By running the broadcaster nodes new frames are added such that the frame link is 'map->static_frame->dynamic_frame'. A listener can then be used to find the transformation between 'map' and 'dynamic_frame'.

Additionally this package provides an example of ros2 c++ code with header files.


Nodes included:
* staticExample
* dynamicExample
* listenerExample



### Launching Static Broadcaster

    ros2 run demo_tf2 staticExample 

A latched static frame can be broadcasted once and doesn't require a heartbeat. This is good for frames such as sensors or actuators which are static relative to a base. 


### Launching Dynamic Broadcaster

    ros2 run demo_tf2 dynamicExample 

A dynamic frame must be updated regularly, it is used for dynamic frames such as odometry or tracked objects. 


### Launching Listener

    ros2 run demo_tf2 listenerExample 

A listener looks up the transformation between two frames and returns the resulting transformation. 

### Launching Rviz
After launching rviz add the TF frame viewer.
Ensure the fixed frame is set to 'map'.

    rviz2