/*
extern "C"
{
#include "rc_usefulincludes.h"
}
extern "C"
{  
#include "roboticscape.h"
}
*/

#include "stdio.h"
#include "getopt.h"
#include "stdlib.h" // for atoi
#include "signal.h"
#include "rc/time.h"
#include "rc/adc.h"
#include "rc/dsm.h"
#include "rc/servo.h"
#include "math.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "unistd.h"
#include <psr_msgs/Puppy_pos.h>

// Define Globle Variables
// Flag
bool if_running = true;

// Servo channel
unsigned int ch_front_left_upper = 8;
unsigned int ch_front_left_lower = 7;

unsigned int ch_rear_left_upper = 6;
unsigned int ch_rear_left_lower = 5;

unsigned int ch_front_right_upper = 4;
unsigned int ch_front_right_lower = 3;

unsigned int ch_rear_right_upper = 2;
unsigned int ch_rear_right_lower = 1;

// Servo positions [-pi/2, pi/2] or [-1, 1]
float pos_front_left_upper = 0;
float pos_front_left_lower = 0;

float pos_rear_left_upper = 0;
float pos_rear_left_lower = 0;

float pos_front_right_upper = 0;
float pos_front_right_lower = 0;

float pos_rear_right_upper = 0;
float pos_rear_right_lower = 0;

// Position limits
float POS_MAX = M_PI/2.0;

void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
}

void pos_Callback(const psr_msgs::Puppy_pos::ConstPtr& puppy_pos_msg)
{
  ROS_INFO("Message received!!!");
  // assign the commands if the array is of the correct length
  pos_front_left_upper = puppy_pos_msg->pos_front_left_upper_des;
  pos_front_left_lower = puppy_pos_msg->pos_front_left_lower_des;
  
  pos_rear_left_upper = puppy_pos_msg->pos_rear_left_upper_des;
  pos_rear_left_lower = puppy_pos_msg->pos_rear_left_lower_des;
  
  pos_front_right_upper = puppy_pos_msg->pos_front_right_upper_des;
  pos_front_right_lower = puppy_pos_msg->pos_front_right_lower_des;
  
  pos_rear_right_upper = puppy_pos_msg->pos_rear_right_upper_des;
  pos_rear_right_lower = puppy_pos_msg->pos_rear_right_lower_des;
}

void ros_compatible_shutdown_signal_handler(int signo)
{
  if (signo == SIGINT)
    {
      rc_servo_power_rail_en(0);
      rc_servo_cleanup();
      rc_dsm_cleanup();
      //rc_set_state(EXITING);
      ROS_INFO("\nReceived SIGINT Ctrl-C.");
      ros::shutdown();
    }
  else if (signo == SIGTERM)
    {
      rc_servo_power_rail_en(0);
      rc_servo_cleanup();
      rc_dsm_cleanup();
      //rc_set_state(EXITING);
      ROS_INFO("Received SIGTERM.");
      ros::shutdown();
    }
}


int main(int argc, char **argv)
{
  unsigned int call_back_queue_len = 10;
  ros::init(argc, argv, "puppy_pos");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("/PUPPY/pos", call_back_queue_len, pos_Callback);
	
  signal(SIGINT,  ros_compatible_shutdown_signal_handler);	
  signal(SIGTERM, ros_compatible_shutdown_signal_handler);
	
/*
  if(rc_initialize()<0){
      ROS_INFO("ERROR: failed to initialize cape.");
      return -1;
  }
*/
  // read adc to make sure battery is connected
  if(rc_adc_init()){
      fprintf(stderr,"ERROR: failed to run rc_adc_init()\n");
      return -1;
  }
  if(rc_adc_batt()<6.0){
      fprintf(stderr,"ERROR: battery disconnected or insufficiently charged to drive servos\n");
      return -1;
  }
  rc_adc_cleanup();
	
  // initialize PRU
  if(rc_servo_init()) return -1;
	
  // turn on power
  printf("Turning On 6V Servo Power Rail\n");
  rc_servo_power_rail_en(1);

  ros::Rate r(100);  //100 hz
  while(ros::ok()){
	//rc_enable_motors();
	// send pulse
        if(rc_servo_send_pulse_normalized(ch_front_left_upper,pos_front_left_upper)==-1) return -1;
	if(rc_servo_send_pulse_normalized(ch_front_left_lower,pos_front_left_lower)==-1) return -1;
	
	if(rc_servo_send_pulse_normalized(ch_rear_left_upper,pos_rear_left_upper)==-1) return -1;
	if(rc_servo_send_pulse_normalized(ch_rear_left_lower,pos_rear_left_lower)==-1) return -1;
	
	if(rc_servo_send_pulse_normalized(ch_front_right_upper,pos_front_right_upper)==-1) return -1;
	if(rc_servo_send_pulse_normalized(ch_front_right_lower,pos_front_right_lower)==-1) return -1;
	
	if(rc_servo_send_pulse_normalized(ch_rear_right_upper,pos_rear_right_upper)==-1) return -1;
	if(rc_servo_send_pulse_normalized(ch_rear_right_lower,pos_rear_right_lower)==-1) return -1;
	
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
// turn off power rail and cleanup
  rc_servo_power_rail_en(0);
  rc_servo_cleanup();
  rc_dsm_cleanup();
  return 0;
}
