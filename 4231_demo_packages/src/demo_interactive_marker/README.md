# demo_interactive_marker
Package created and maintained by Mabel Zhang

This package provides some examples for interactive markers. Interactive markers allow a user to manipulate a marker within 3d space inside of rviz. Restrictions based on directions and orientations can be set. Interactive markers post positional updates when they are manipulated. 

While this package contains 6 examples, basic_controls is the most useful and pong is cool just to see what can be created in rviz. 

Nodes included:
* basic_controls
* int_addition_server


Setting up rviz:
* open rviz
* set fixed frame to 'base_link'
* add interactive markers to rviz

### Launching the basic controls demo 

    ros2 run demo_interactive_marker basic_controls 

### Launching the pong demo 

    ros2 run demo_interactive_marker pong 



