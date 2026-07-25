#!/bin/bash
# test_commands.sh - Test script for robot commands

source /opt/ros/jazzy/setup.bash
source ~/ros2_robot_navigator/install/setup.bash

echo " Robot Navigator - Command Tester"
echo "===================================="
echo ""

# Function to send command
send_command() {
    local direction=$1
    echo " Sending command: $direction"
    ros2 service call /command_direction robot_controller/srv/DirectionCommand "{direction: '$direction'}"
    sleep 1
}

echo "Testing all directions..."
echo ""

send_command "forward"
send_command "backward"
send_command "strafe_left"
send_command "strafe_right"
send_command "rotate_left"
send_command "rotate_right"
send_command "stop"

echo ""
echo " All tests completed!"
