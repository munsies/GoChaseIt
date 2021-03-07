#include <ros/ros.h>
#include "ball_chaser_2/DriveToTarget_2.h"
#include "geometry_msgs/Twist.h"
#include <std_msgs/Float64.h>

class SubscribeAndPublish
{
public:
  SubscribeAndPublish()
  {
    // Define a publishers to publish Twist messages::Float64 messages on joints respective topics
    motor_command_publisher = n_.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

  }

// Methods
ros::NodeHandle n(){ return n_;}


// This callback function executes whenever a handle drive service is requested
bool handle_drive_request(ball_chaser_2::DriveToTarget_2::Request& req,
    ball_chaser_2::DriveToTarget_2::Response& res)
{

    ROS_INFO("DriveToTargetRequest received - j1:%1.2f, j2:%1.2f", (float)req.linear_x, (float)req.angular_z);

    // Create a motor_command object of type geometry_msgs::Twist
    geometry_msgs::Twist motor_command;
    
    // Set wheel velocities
    motor_command.linear.x = req.linear_x;
    motor_command.angular.z = req.angular_z;

    motor_command_publisher.publish(motor_command);

    // Return a response message
    res.msg_feedback = "linear speed_x is set to: " + std::to_string(req.linear_x) + " , angular_z " + std::to_string(req.angular_z);
    ROS_INFO_STREAM(res.msg_feedback);

    return true;
}

private:
  ros::NodeHandle n_; 
  ros::Publisher motor_command_publisher;
  ros::Subscriber sub_;

};//End of class SubscribeAndPublish

int main(int argc, char **argv)
{
  //Initiate ROS
  ros::init(argc, argv, "drive_bot");

  //Create an object of class SubscribeAndPublish that will take care of everything
  SubscribeAndPublish SAPObject;

  // Define a command_robt service with a handle_drive_request callback function
  ros::ServiceServer service = SAPObject.n().advertiseService("/ball_chaser_2/command_robot", &SubscribeAndPublish::handle_drive_request, &SAPObject);
  ROS_INFO("Ready to send joint commands");

  ros::spin();

  return 0;
}