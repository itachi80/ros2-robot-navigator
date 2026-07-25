# ROS2 Robot Navigator - Multi-Directional Control with Sensor Fusion

A ROS2 Jazzy project demonstrating a robot controller with multi-directional movement commands and obstacle detection using sensor fusion.

## Project Overview

This project implements:
- **Multi-Directional Control**: Forward, backward, strafe left/right, rotate left/right
- **Sensor Fusion**: LaserScan processing with obstacle detection
- **Emergency Stop**: Automatic velocity halt when obstacles detected
- **ROS2 Service Interface**: Direction commands via service calls

##  Architecture

```
┌─────────────────────────────────────────┐
│      Command Interface (ROS2 Service)   │
│    /command_direction (DirectionCommand)│
└─────────────┬───────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────┐
│      Velocity Commander Node            │
│  - Converts direction → Twist message   │
│  - Publishes to /cmd_vel                │
└─────────────┬───────────────────────────┘
              │
              ▼
┌─────────────────────────────────────────┐
│      Sensor Fusion Node                 │
│  - Subscribes to /scan (LaserScan)      │
│  - Detects obstacles                    │
│  - Emergency stop if needed             │
│  - Publishes safe velocity to /safe_cmd_vel
└─────────────────────────────────────────┘
```

## Quick Start

### Prerequisites
- ROS2 Jazzy
- Ubuntu 24.04 (recommended)

### Build

```bash
cd ~/ros2_robot_navigator
colcon build --packages-select robot_controller
source install/setup.bash
```

### Run

**Start the robot stack:**
```bash
ros2 launch robot_controller robot_stack.launch.py
```

**Send a movement command (in another terminal):**
```bash
source install/setup.bash
ros2 service call /command_direction robot_controller/srv/DirectionCommand "{direction: 'forward'}"
```

##  Available Commands

### Direction Commands
Send via the `/command_direction` service:

| Direction | Description |
|-----------|-------------|
| `forward` | Move forward (0.5 m/s) |
| `backward` | Move backward (-0.5 m/s) |
| `strafe_left` | Strafe left (0.5 m/s lateral) |
| `strafe_right` | Strafe right (-0.5 m/s lateral) |
| `rotate_left` | Rotate counter-clockwise (0.5 rad/s) |
| `rotate_right` | Rotate clockwise (-0.5 rad/s) |
| `stop` | Stop all movement |

### Example Commands

```bash
# Move forward
ros2 service call /command_direction robot_controller/srv/DirectionCommand "{direction: 'forward'}"

# Strafe left
ros2 service call /command_direction robot_controller/srv/DirectionCommand "{direction: 'strafe_left'}"

# Stop
ros2 service call /command_direction robot_controller/srv/DirectionCommand "{direction: 'stop'}"
```

## Topics

### Published Topics
- `/cmd_vel` (geometry_msgs/Twist): Raw velocity commands
- `/safe_cmd_vel` (geometry_msgs/Twist): Safe velocity (after sensor fusion)

### Subscribed Topics
- `/scan` (sensor_msgs/LaserScan): Laser scanner data for obstacle detection

## Sensor Fusion & Safety

The sensor fusion node:
1. **Subscribes** to LaserScan data
2. **Analyzes** the front 90° arc for obstacles
3. **Detects** obstacles closer than 0.3m (30cm)
4. **Triggers emergency stop** when:
   - Moving forward AND
   - Obstacle detected in front

### Testing Obstacle Detection

**Publish LaserScan with obstacle (0.2m ahead):**
```bash
ros2 topic pub /scan sensor_msgs/msg/LaserScan \
  "{header: {frame_id: 'laser'}, angle_min: -1.57, angle_max: 1.57, \
    angle_increment: 0.01, range_min: 0.1, range_max: 10.0, \
    ranges: [10.0, 10.0, 10.0, 10.0, 0.2, 10.0, 10.0, 10.0]}" -r 10
```

**Publish LaserScan with no obstacles (all clear):**
```bash
ros2 topic pub /scan sensor_msgs/msg/LaserScan \
  "{header: {frame_id: 'laser'}, angle_min: -1.57, angle_max: 1.57, \
    angle_increment: 0.01, range_min: 0.1, range_max: 10.0, \
    ranges: [10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0]}" -r 10
```

## Project Structure

```
ros2_robot_navigator/
├── src/
│   └── robot_controller/
│       ├── src/
│       │   ├── velocity_commander.cpp    # Main command handler
│       │   └── sensor_fusion.cpp         # Sensor processing
│       ├── srv/
│       │   └── DirectionCommand.srv      # Service definition
│       ├── launch/
│       │   └── robot_stack.launch.py     # Launch both nodes
│       ├── CMakeLists.txt
│       └── package.xml
├── build/
├── install/
├── log/
└── README.md
```

##  Implementation Details

### Velocity Commander Node
- **Type**: C++ ROS2 Node
- **Functionality**: 
  - Accepts direction commands via `/command_direction` service
  - Converts directions to Twist velocity messages
  - Publishes to `/cmd_vel` at 10Hz

### Sensor Fusion Node
- **Type**: C++ ROS2 Node
- **Functionality**:
  - Subscribes to `/scan` (LaserScan)
  - Analyzes front 90° sector
  - Implements emergency stop logic
  - Publishes safe commands to `/safe_cmd_vel`

##  Learning Outcomes

This project demonstrates:
-  ROS2 package structure and build system
-  C++ node implementation with publishers/subscribers
-  Custom ROS2 service definitions
-  Launch file configuration
-  Sensor data processing and fusion
-  Safety-critical control logic

##  Future Extensions

- Integrate with Gazebo simulation
- Add camera-based perception
- Implement path planning algorithms
- Connect to real Unitree G1 robot
- Add reinforcement learning for gait optimization
- Integrate NVIDIA GR00T for imitation learning

##  Notes

- Obstacle threshold: 0.3m (configurable in sensor_fusion.cpp)
- Velocity commands: 0.5 m/s linear, 0.5 rad/s angular
- LaserScan front sector: center 90° (configurable)

---

