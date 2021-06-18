#include "ros/ros.h"
#include "epos2/Velocity.h"

#include <geometry_msgs/Twist.h>
#include "wrap.h"
#include <iostream>

#include <math.h>
#include <chrono>
#include <sys/time.h>
#include <ctime>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Vector3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf2_ros/buffer.h>
#include <geometry_msgs/PointStamped.h>
#include <geometry_msgs/PoseStamped.h>


#include <std_msgs/Int64.h>
#include <chrono>
#include <thread>


#define PI 3.14159


uint64_t timestamp_ns;
long demand1;
long demand2;
int position_new1;
int position_new2;
int velocity_new1;
int velocity_new2;
int velocity_left;
int velocity_right;
double x , y , theta;
char base_link[] = "/base_link";
char odom[] = "/odom";

std::chrono::milliseconds interval(10);
time_t  previousMillis = 0;
float loopTime = 10 ;

int pos0 ;
int pos1 ;

long pos0_old ;
long pos1_old ;
long pos0_diff ;
long pos1_diff ;

float pos0_mm_diff ;
float pos1_mm_diff ;

float pos_average_mm_diff ;
float pos_total_mm ;

int forward0 ;
int forward1 ;
int turn0 ;
int turn1 ;





//static tf2_ros::TransformBroadcaster br;



unsigned int ulErrorCode = 0;
int lResult = MMC_FAILED;

void filterVelocityCallback(const std_msgs::Int64& msg){

    
  demand1 = msg.data;
  ROS_INFO("%d", msg.data);
    

}




int main(int argc, char **argv)
{
	
	
	// Set parameter for usb IO operation
	SetDefaultParameters();
	// open device
	if((lResult = OpenDevice(&ulErrorCode))!=MMC_SUCCESS)
	{
		LogError("OpenDevice", lResult, ulErrorCode);
		return lResult;
	}
	
	SetEnableState(g_pKeyHandle, g_usNodeId, &ulErrorCode);
	ActivateProfileVelocityMode(g_pKeyHandle, g_usNodeId, &ulErrorCode);




	ros::init(argc, argv, "epos2");
	ros::NodeHandle n;

    ros::Subscriber sub = n.subscribe("/cmd_vel",1,&filterVelocityCallback);



	ROS_INFO("Ready to move.");
	while(ros::ok()){

		ros::spinOnce();

        std::this_thread::sleep_for( interval ) ;

  		// ros::Time frame_time;
  		// frame_time.sec = (int32_t)(timestamp_ns/1000000000UL);
  		// frame_time.nsec = (int32_t)(timestamp_ns % 1000000000UL);
		// time_t now = time(nullptr);
    	// time_t mnow = now * 1000;

		// auto currentMillis = mnow;
		// if(currentMillis - previousMillis >= loopTime){

		// 	previousMillis = currentMillis;
		// 	// forward0 = (newBot.linear_x)* 1715;
		// 	// forward1 = (newBot.linear_x)* 1715;

		// 	// turn0 =    (newBot.angular_z)* 1715;
		// 	// turn1 =    (newBot.angular_z)* 1715;

		// 	// demand1 = forward0 - turn0 ;
		// 	// demand2 = forward1 + turn1 ;

		// 	// demand1 = demand1 * -1 ;






		// }
		MoveWithVelocity(g_pKeyHandle, g_usNodeId, demand1, &ulErrorCode);


		get_position(g_pKeyHandle, g_usNodeId, &pos0, &ulErrorCode);

        cout << pos0 << ".........." << endl;

	}

	//disable epos
	SetDisableState(g_pKeyHandle, g_usNodeId, &ulErrorCode);
	//close device
	if((lResult = CloseDevice(&ulErrorCode))!=MMC_SUCCESS)
	{
		LogError("CloseDevice", lResult, ulErrorCode);
		return lResult;
	}
	lResult = SetDisableState(g_pKeyHandle2, g_usNodeId2, &ulErrorCode);
	//close device

	return 0;
}
