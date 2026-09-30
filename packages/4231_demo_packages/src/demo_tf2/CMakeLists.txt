cmake_minimum_required(VERSION 3.8)
project(demo_tf2)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)

add_executable(staticExample src/staticExample.cpp)

add_executable(dynamicExample include/dynamicExample.hpp src/dynamicExample.cpp )
target_include_directories(dynamicExample PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
#target_compile_features(dynamicExample PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

add_executable(listenerExample include/listenerExample.hpp src/listenerExample.cpp )
target_include_directories(listenerExample PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
#target_compile_features(dynamicExample PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17




# Link the necessary libraries to your executable
ament_target_dependencies( staticExample geometry_msgs rclcpp tf2 tf2_ros )

ament_target_dependencies( dynamicExample geometry_msgs rclcpp tf2 tf2_ros )

ament_target_dependencies( listenerExample geometry_msgs rclcpp tf2 tf2_ros )





# Add the installation rules
install(TARGETS 
  dynamicExample
  staticExample
  listenerExample
  DESTINATION lib/${PROJECT_NAME}
)




ament_package()
