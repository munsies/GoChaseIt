#include "ros/ros.h"
#include "ball_chaser_2/DriveToTarget_2.h"
#include <sensor_msgs/Image.h>

// Define global client that can request services
ros::ServiceClient client;

class SubscribeAndPublish
{
public:
  SubscribeAndPublish()
  {

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    sub1 = n_.subscribe("/camera/rgb/image_raw", 10, &SubscribeAndPublish::process_image_callback, this);   
  }

  // Methods
ros::NodeHandle n(){ return n_;}

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("Moving the arm to the center");
    
    // Request a service and pass the velocities to it to drive the robot
    ball_chaser_2::DriveToTarget_2 srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255; // ball pixel
    bool left = false, middle = false, right = false; // image regions
    int width_counter = 0; // column counter
    float linear_x =0.0, angular_z = 0.0; // speed command

    // Loop through each pixel in the image and check if ball is on the left, middle or right of the image
    for (int i = 0; i < img.height * img.step; i++) {
        if (img.data[i] == white_pixel) {
            if(width_counter < (int)img.step/3) {
                left = true;
            }
            else if(width_counter >= (int)img.step/3 && width_counter < (int)img.step*2/3) {
                middle = true;
            }
            else {
                right = true;
            } 
        }
        width_counter++;
        if (width_counter >= img.step) {
            width_counter = 0;
        }
    }

    if (left) {linear_x = 0; angular_z = 1.0;}
    else if (right) {linear_x = 0; angular_z = -1.0;}
    else if (middle){linear_x = 1.0; angular_z = 0;}
    else {linear_x = 0;angular_z = 0;}

    drive_robot(linear_x,angular_z);
    ROS_INFO("Moving the robot x:%1.2f, z:%1.2f", (float)linear_x,(float)angular_z);
}

private:
  ros::NodeHandle n_; 
  ros::Publisher pub_;
  ros::Subscriber sub1;

};//End of class SubscribeAndPublish

int main(int argc, char **argv)
{
  //Initiate ROS
  ros::init(argc, argv, "process_image_2");

  //Create an object of class SubscribeAndPublish that will take care of everything
  SubscribeAndPublish SAPObject;

  // Define a client service capable of requesting services from command_robot
  client = SAPObject.n().serviceClient<ball_chaser_2::DriveToTarget_2>("/ball_chaser_2/command_robot");

  ros::spin();

  return 0;
}