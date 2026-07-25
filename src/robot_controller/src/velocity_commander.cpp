// velocity_commander.cpp
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "robot_controller/srv/direction_command.hpp"

class VelocityCommander : public rclcpp::Node {
public:
    VelocityCommander() : Node("velocity_commander") {
        // Create a publisher for /cmd_vel topic
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
        
        // Create a service for direction commands
        service_ = this->create_service<robot_controller::srv::DirectionCommand>(
            "/command_direction",
            std::bind(&VelocityCommander::handle_direction_command, this, 
                     std::placeholders::_1, std::placeholders::_2));
        
        // Create a timer that publishes velocity commands
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100),
            std::bind(&VelocityCommander::timer_callback, this));
        
        RCLCPP_INFO(this->get_logger(), "Velocity Commander initialized!");
        RCLCPP_INFO(this->get_logger(), "Service available at /command_direction");
    }

private:
    void handle_direction_command(
        const std::shared_ptr<robot_controller::srv::DirectionCommand::Request> request,
        std::shared_ptr<robot_controller::srv::DirectionCommand::Response> response) {
        
        std::string direction = request->direction;
        RCLCPP_INFO(this->get_logger(), "Received command: %s", direction.c_str());
        
        // Update the current velocity based on direction
        if (direction == "forward") {
            current_velocity_.linear.x = 0.5;
            current_velocity_.linear.y = 0.0;
            current_velocity_.angular.z = 0.0;
            response->message = "Moving forward";
            response->success = true;
        } 
        else if (direction == "backward") {
            current_velocity_.linear.x = -0.5;
            current_velocity_.linear.y = 0.0;
            current_velocity_.angular.z = 0.0;
            response->message = "Moving backward";
            response->success = true;
        } 
        else if (direction == "strafe_left") {
            current_velocity_.linear.x = 0.0;
            current_velocity_.linear.y = 0.5;
            current_velocity_.angular.z = 0.0;
            response->message = "Strafing left";
            response->success = true;
        } 
        else if (direction == "strafe_right") {
            current_velocity_.linear.x = 0.0;
            current_velocity_.linear.y = -0.5;
            current_velocity_.angular.z = 0.0;
            response->message = "Strafing right";
            response->success = true;
        } 
        else if (direction == "rotate_left") {
            current_velocity_.linear.x = 0.0;
            current_velocity_.linear.y = 0.0;
            current_velocity_.angular.z = 0.5;
            response->message = "Rotating left";
            response->success = true;
        } 
        else if (direction == "rotate_right") {
            current_velocity_.linear.x = 0.0;
            current_velocity_.linear.y = 0.0;
            current_velocity_.angular.z = -0.5;
            response->message = "Rotating right";
            response->success = true;
        }
        else if (direction == "stop") {
            current_velocity_.linear.x = 0.0;
            current_velocity_.linear.y = 0.0;
            current_velocity_.angular.z = 0.0;
            response->message = "Stopped";
            response->success = true;
        }
        else {
            response->message = "Unknown direction: " + direction;
            response->success = false;
            RCLCPP_WARN(this->get_logger(), "Unknown direction received: %s", direction.c_str());
        }
    }

    void timer_callback() {
        publisher_->publish(current_velocity_);
    }
    
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Service<robot_controller::srv::DirectionCommand>::SharedPtr service_;
    rclcpp::TimerBase::SharedPtr timer_;
    geometry_msgs::msg::Twist current_velocity_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<VelocityCommander>());
    rclcpp::shutdown();
    return 0;
}
