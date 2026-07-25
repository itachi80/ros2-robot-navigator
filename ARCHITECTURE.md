# Technical Architecture

## System Overview

The robot navigator system consists of three main components:

1. **Velocity Commander Node** - Handles direction commands
2. **Sensor Fusion Node** - Processes sensor data and implements safety
3. **ROS2 Communication Layer** - Manages node-to-node messaging

## Component Descriptions

### Velocity Commander Node

**File**: `src/robot_controller/src/velocity_commander.cpp`

**Responsibilities**:
- Listen for direction commands on `/command_direction` service
- Convert textual directions to Twist messages
- Publish velocity commands to `/cmd_vel` topic

**Service Interface**:
```
Service: /command_direction
Request: string direction
Response: bool success, string message
```

**Supported Directions**:
- forward, backward
- strafe_left, strafe_right
- rotate_left, rotate_right
- stop

### Sensor Fusion Node

**File**: `src/robot_controller/src/sensor_fusion.cpp`

**Responsibilities**:
- Subscribe to LaserScan data on `/scan`
- Process laser ranges to detect obstacles
- Monitor robot velocity commands on `/cmd_vel`
- Implement emergency stop logic
- Publish safe velocity to `/safe_cmd_vel`

**Obstacle Detection Algorithm**:
```
1. Receive LaserScan message
2. Extract center 90° sector of laser readings
3. Find minimum distance in that sector
4. If min_distance < 0.3m: obstacle_detected = true
5. If moving forward AND obstacle_detected: emergency_stop = true
```

## Data Flow

```
User Command
    ↓
    ├─→ /command_direction (Service Call)
    │       ↓
    │   Velocity Commander
    │       ↓
    │   /cmd_vel (Twist)
    ↓       ↓
Sensor Fusion
    ├─→ Subscribe /cmd_vel
    └─→ Subscribe /scan
        ↓
    Obstacle Detection Logic
        ↓
    /safe_cmd_vel (Safe Twist)
```

## ROS2 Interfaces

### Custom Service: DirectionCommand.srv
```
string direction
---
bool success
string message
```

### Standard Messages Used
- `geometry_msgs/Twist` - Velocity commands
- `sensor_msgs/LaserScan` - Laser data

## Configuration Parameters

### Velocity Commander
- Timer frequency: 100ms
- Velocity magnitudes: 0.5 m/s (linear), 0.5 rad/s (angular)

### Sensor Fusion
- Obstacle detection threshold: 0.3m
- Scanned sector: center 90° (25% to 75% of laser scan)
- Scan data timeout: 1.0 second

## Safety Features

1. **Emergency Stop**: Halts forward motion on obstacle detection
2. **Timeout Handling**: Forgets old sensor data if no updates
3. **Sector-based Detection**: Only checks front 90° to avoid false positives
4. **Velocity Gating**: Only stops forward motion, allows lateral/rotational movement

---

