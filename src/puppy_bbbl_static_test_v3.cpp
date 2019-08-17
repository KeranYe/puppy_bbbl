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
#include <iostream>
using namespace std;

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
double pos_front_left_upper = 0;
double pos_front_left_lower = 0;

double pos_rear_left_upper = 0;
double pos_rear_left_lower = 0;

double pos_front_right_upper = 0;
double pos_front_right_lower = 0;

double pos_rear_right_upper = 0;
double pos_rear_right_lower = 0;

// Position limits
double POS_MAX = M_PI/2.0;

// Pos waypoint list
double pos_list_1[40]={0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, \
		     1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, \
		     0.0,-0.1,-0.2,-0.3,-0.4,-0.5,-0.6,-0.7,-0.8,-0.9, \
		    -1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1};
double pos_list_2[20]={0.0, 0.2, 0.4, 0.6, 0.8, \
		     1.0, 0.8, 0.6, 0.4, 0.2, \
		     0.0,-0.2,-0.4,-0.6,-0.8, \
		    -1.0,-0.8,-0.6,-0.4,-0.2};

void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", msg->data.c_str());
}

void pos_Callback(const psr_msgs::Puppy_pos::ConstPtr& puppy_pos_msg)
{
  ROS_INFO("Message received!!!");
  // assign the commands if the array is of the correct length
/*
  pos_front_left_upper = puppy_pos_msg->pos_front_left_upper_des;
  pos_front_left_lower = puppy_pos_msg->pos_front_left_lower_des;
  
  pos_rear_left_upper = puppy_pos_msg->pos_rear_left_upper_des;
  pos_rear_left_lower = puppy_pos_msg->pos_rear_left_lower_des;
  
  pos_front_right_upper = puppy_pos_msg->pos_front_right_upper_des;
  pos_front_right_lower = puppy_pos_msg->pos_front_right_lower_des;
  
  pos_rear_right_upper = puppy_pos_msg->pos_rear_right_upper_des;
  pos_rear_right_lower = puppy_pos_msg->pos_rear_right_lower_des;
  
  if(rc_servo_send_pulse_normalized(ch_front_left_upper,pos_front_left_upper)==-1) return;
  if(rc_servo_send_pulse_normalized(ch_front_left_lower,pos_front_left_lower)==-1) return;
	
  if(rc_servo_send_pulse_normalized(ch_rear_left_upper,pos_rear_left_upper)==-1) return;
  if(rc_servo_send_pulse_normalized(ch_rear_left_lower,pos_rear_left_lower)==-1) return;
	
  if(rc_servo_send_pulse_normalized(ch_front_right_upper,pos_front_right_upper)==-1) return;
  if(rc_servo_send_pulse_normalized(ch_front_right_lower,pos_front_right_lower)==-1) return;
	
  if(rc_servo_send_pulse_normalized(ch_rear_right_upper,pos_rear_right_upper)==-1) return;
  if(rc_servo_send_pulse_normalized(ch_rear_right_lower,pos_rear_right_lower)==-1) return;
 */
}

void ros_compatible_shutdown_signal_handler(int signo)
{
  if (signo == SIGINT)
    {
      if_running = false;
      rc_servo_power_rail_en(0);
      rc_servo_cleanup();
      rc_dsm_cleanup();
      //rc_set_state(EXITING);
      ROS_INFO("\nReceived SIGINT Ctrl-C.");
      ros::shutdown();
    }
  else if (signo == SIGTERM)
    {
      if_running = false;
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
	int frequency_hz = 50;
  	unsigned int index = 0;
  	unsigned int call_back_queue_len = 10;
	unsigned int loop_rate = 10;
	char yes_or_no = 'n';
	while(1){	
		yes_or_no = 'n';
		if_running = true;		
		cout << "Please enter Looping Rate (int, default = 10): ";
		cin >> loop_rate;
		cout << "Please enter Call Back Queue Length (int, default = 10): ";
		cin >> call_back_queue_len;
		cout << "Please enter RC Frequency (int, default = 50): ";
		cin >> frequency_hz;
		cout << "Correct input for Looping rate = " << loop_rate \
			<< " and Queue Length = " << call_back_queue_len \
			<< " Freq = " << frequency_hz << "?(y/n)";
		cin >> yes_or_no;		
		if(yes_or_no == 'y') break;
	}
	
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

  //ros::Rate r(loop_rate);  //100 hz
  index = 0;
  while(if_running){
	//rc_enable_motors();
	// send pulse
	ROS_INFO("Running!!!");
	pos_front_left_upper = pos_list_2[index];
	//pos_front_left_lower = pos_list[index];
	pos_rear_left_upper = pos_list_2[index];
	  
	if(rc_servo_send_pulse_normalized(ch_front_left_upper,pos_front_left_upper)==-1) return -1;
	if(rc_servo_send_pulse_normalized(ch_front_left_lower,pos_front_left_lower)==-1) return -1;
	
	if(rc_servo_send_pulse_normalized(ch_rear_left_upper,pos_rear_left_upper)==-1) return -1;
	if(rc_servo_send_pulse_normalized(ch_rear_left_lower,pos_rear_left_lower)==-1) return -1;
	
	if(rc_servo_send_pulse_normalized(ch_front_right_upper,pos_front_right_upper)==-1) return -1;
	if(rc_servo_send_pulse_normalized(ch_front_right_lower,pos_front_right_lower)==-1) return -1;
	
	if(rc_servo_send_pulse_normalized(ch_rear_right_upper,pos_rear_right_upper)==-1) return -1;
	if(rc_servo_send_pulse_normalized(ch_rear_right_lower,pos_rear_right_lower)==-1) return -1;
	
	index = index + 1;
	if(index > 19) index = 0;
	  
//	ros::spinOnce();
//	r.sleep();
	rc_usleep(1000000/frequency_hz);

}

  /**
   * ros::spin() will enter a loop, pumping callbacks.  With this version, all
   * callbacks will be called from within this thread (the main one).  ros::spin()
   * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
   */
//  ros::spin();
rc_usleep(50000);
// turn off power rail and cleanup
  rc_servo_power_rail_en(0);
  rc_servo_cleanup();
  rc_dsm_cleanup();
  return 0;
}
