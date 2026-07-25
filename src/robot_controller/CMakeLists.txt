cmake_minimum_required(VERSION 3.8)
project(robot_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# Generate service files
rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/DirectionCommand.srv"
)

# Add executable for velocity_commander
add_executable(velocity_commander src/velocity_commander.cpp)

rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
target_link_libraries(velocity_commander "${cpp_typesupport_target}")

ament_target_dependencies(velocity_commander 
  rclcpp 
  geometry_msgs
  rosidl_runtime_cpp
)

# Add sensor fusion executable
add_executable(sensor_fusion src/sensor_fusion.cpp)

rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}" "rosidl_typesupport_cpp")
target_link_libraries(sensor_fusion "${cpp_typesupport_target}")

ament_target_dependencies(sensor_fusion 
  rclcpp 
  geometry_msgs
  sensor_msgs
  rosidl_runtime_cpp
)

# Install executables
install(TARGETS
    velocity_commander
    sensor_fusion
    DESTINATION lib/${PROJECT_NAME})

# Install launch files
install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch)

ament_package()
