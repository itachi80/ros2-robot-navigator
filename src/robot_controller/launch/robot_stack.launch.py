# robot_stack.launch.py
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    velocity_commander = Node(
        package='robot_controller',
        executable='velocity_commander',
        name='velocity_commander',
        output='screen'
    )
    
    sensor_fusion = Node(
        package='robot_controller',
        executable='sensor_fusion',
        name='sensor_fusion',
        output='screen'
    )
    
    return LaunchDescription([
        velocity_commander,
        sensor_fusion,
    ])
