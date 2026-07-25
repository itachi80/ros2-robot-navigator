// sensor_fusion.cpp
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include <chrono>

class SensorFusion : public rclcpp::Node {
public:
    SensorFusion() : Node("sensor_fusion") {
        // Create a publisher for safe velocity commands
        safe_velocity_pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
            "/safe_cmd_vel", 10);
        
        // Subscribe to raw velocity commands
        velocity_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10,
            std::bind(&SensorFusion::velocity_callback, this, std::placeholders::_1));
        
        // Subscribe to LaserScan (simulated)
        laser_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", 10,
            std::bind(&SensorFusion::laser_callback, this, std::placeholders::_1));
        
        RCLCPP_INFO(this->get_logger(), "Sensor Fusion node initialized!");
        RCLCPP_INFO(this->get_logger(), "Subscribing to /cmd_vel and /scan");
        RCLCPP_INFO(this->get_logger(), "Publishing safe commands to /safe_cmd_vel");
        
        obstacle_distance_threshold_ = 0.3;  // 30cm threshold
    }

private:
    void laser_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
        // Update timestamp
        last_scan_time_ = this->now();
        
        // Check if there's an obstacle in front
        obstacle_detected_ = false;
        min_distance_ = msg->range_max;
        
        // Check front ranges (center 90 degrees of the scan)
        int num_ranges = msg->ranges.size();
        int start_idx = num_ranges / 4;  // 25% from left
        int end_idx = 3 * num_ranges / 4;  // 75% from left (front center)
        
        for (int i = start_idx; i < end_idx; ++i) {
            float range = msg->ranges[i];
            
            // Skip invalid ranges
            if (range < msg->range_min || range > msg->range_max) {
                continue;
            }
            
            if (range < min_distance_) {
                min_distance_ = range;
            }
            
            // Obstacle detected if too close
            if (range < obstacle_distance_threshold_) {
                obstacle_detected_ = true;
            }
        }
        
        if (obstacle_detected_) {
            RCLCPP_WARN(this->get_logger(), 
                "🚨 OBSTACLE DETECTED! Distance: %.2f m", min_distance_);
        } else {
            RCLCPP_INFO(this->get_logger(), 
                "✓ Path clear. Nearest object: %.2f m", min_distance_);
        }
    }

    void velocity_callback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        auto safe_velocity = *msg;
        
        // Emergency stop if obstacle ahead and moving forward
        if (obstacle_detected_ && msg->linear.x > 0.0) {
            RCLCPP_ERROR(this->get_logger(), 
                "⛔ EMERGENCY STOP! Obstacle at %.2f m", min_distance_);
            safe_velocity.linear.x = 0.0;
            safe_velocity.linear.y = 0.0;
            safe_velocity.angular.z = 0.0;
        } else if (msg->linear.x > 0.0 && !obstacle_detected_) {
            RCLCPP_INFO(this->get_logger(), 
                "✓ Moving forward safely");
        }
        
        safe_velocity_pub_->publish(safe_velocity);
    }
    
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr safe_velocity_pub_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr velocity_sub_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub_;
    
    bool obstacle_detected_ = false;
    float min_distance_ = 10.0;
    float obstacle_distance_threshold_;
    rclcpp::Time last_scan_time_ = rclcpp::Time(0);
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SensorFusion>());
    rclcpp::shutdown();
    return 0;
}
