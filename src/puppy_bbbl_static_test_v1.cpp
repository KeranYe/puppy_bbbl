extern "C"
{
#include "rc_usefulincludes.h"
}
extern "C"
{  
#include "roboticscape.h"
}

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h> // for atoi
#include <signal.h>
#include <rc/time.h>
#include <rc/adc.h>
#include <rc/dsm.h>
#include <rc/servo.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "unistd.h"
#include <psr_msgs/PSR_Drive.h>

// Define Globle Variables
// Define velocity
float x_max = 1;
float z_max = 1;

// Define duty cycle for dual motors
unsigned int LeftChannel = 1;
unsigned int RightChannel  = 2;
float LeftDuty = 0;
float RightDuty = 0;


void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
}

void drive_Callback(const psr_msgs::PSR_Drive::ConstPtr& psr_drive_msg)
{

  // assign the commands if the array is of the correct length
  LeftDuty = psr_drive_msg->duty_left_des;
  RightDuty = psr_drive_msg->duty_right_des;

//  time_last_good_ros_command_sec = ros::Time::now().toSec();
  ROS_INFO("Left Duty, Right Duty= %1.2f %1.2f %1.2f" , LeftDuty, RightDuty);
//  return;

// Map from linear_premap & angular_premap to LeftDuty & RightDuty
// 1. Limit Duty Cycle to [0, x_max];
   if( LeftDuty > x_max ){
   	LeftDuty = 0;
	ROS_INFO("LeftDuty upper bound excessed!");
   }
   if( RightDuty > x_max ){
        RightDuty = 0;
	ROS_INFO("RightDuty lower bound excessed!");
   }
}

void ros_compatible_shutdown_signal_handler(int signo)
{
  if (signo == SIGINT)
    {
      rc_set_motor(LeftChannel,0);
      rc_set_motor(RightChannel,0);
      rc_disable_motors();
      rc_set_state(EXITING);
      ROS_INFO("\nReceived SIGINT Ctrl-C.");
      ros::shutdown();
    }
  else if (signo == SIGTERM)
    {
      rc_set_motor(LeftChannel,0);
      rc_set_motor(RightChannel,0);
      rc_disable_motors();
      rc_set_state(EXITING);
      ROS_INFO("Received SIGTERM.");
      ros::shutdown();
    }
}


int main(int argc, char **argv)
{
  unsigned int call_back_queue_len = 1;
  ros::init(argc, argv, "psr_drive");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("/PSR/motors", call_back_queue_len, drive_Callback);

  if(rc_initialize()<0)
    {
      ROS_INFO("ERROR: failed to initialize cape.");
      return -1;
}

  signal(SIGINT,  ros_compatible_shutdown_signal_handler);	
  signal(SIGTERM, ros_compatible_shutdown_signal_handler);	


//  initialize_motors();
  rc_enable_motors();
  ros::Rate r(100);  //100 hz
  while(ros::ok()){
	//rc_enable_motors();
	rc_set_motor(LeftChannel, LeftDuty);
	rc_set_motor(RightChannel, RightDuty);
	ros::spinOnce();
	r.sleep();
//	rc_usleep(10000);

}

  /**
   * ros::spin() will enter a loop, pumping callbacks.  With this version, all
   * callbacks will be called from within this thread (the main one).  ros::spin()
   * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
   */
//  ros::spin();

  return 0;
}
