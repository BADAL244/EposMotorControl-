#include "ros/ros.h"
#include "epos2/Velocity.h"
#include "epos2/DriveToBot.h"
#include <geometry_msgs/Twist.h>
#include "wrap.h"
#include <iostream>
#include "PID.h"
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


epos2::DriveToBot newBot;

unsigned int ulErrorCode = 0;
int lResult = MMC_FAILED;

void filterVelocityCallback(const geometry_msgs::Twist& msg){

    
    newBot.linear_x = msg.linear.x;
    newBot.angular_z  = msg.angular.z;

    

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

	
	// open device
	if((lResult = OpenDevice2(&ulErrorCode))!=MMC_SUCCESS)
	{
		LogError("OpenDevice", lResult, ulErrorCode);
		return lResult;
	}
	
	SetEnableState(g_pKeyHandle2, g_usNodeId2, &ulErrorCode);
	ActivateProfileVelocityMode(g_pKeyHandle2, g_usNodeId2, &ulErrorCode);


	ros::init(argc, argv, "epos2");
	ros::NodeHandle n;
    tf2_ros::TransformBroadcaster tfb;
	geometry_msgs::TransformStamped tr;
    ros::Subscriber sub = n.subscribe("/cmd_vel",1,&filterVelocityCallback);
	ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 50);
    epos2::DriveToBot bot;
    


	ROS_INFO("Ready to move.");
	while(ros::ok()){

		ros::spinOnce();
  		ros::Time frame_time;
  		frame_time.sec = (int32_t)(timestamp_ns/1000000000UL);
  		frame_time.nsec = (int32_t)(timestamp_ns % 1000000000UL);
		time_t now = time(nullptr);
    	time_t mnow = now * 1000;

		auto currentMillis = mnow;
		if(currentMillis - previousMillis >= loopTime){

			previousMillis = currentMillis;
			forward0 = (newBot.linear_x)* 1715;
			forward1 = (newBot.linear_x)* 1715;

			turn0 =    (newBot.angular_z)* 1715;
			turn1 =    (newBot.angular_z)* 1715;

			demand1 = forward0 - turn0 ;
			demand2 = forward1 + turn1 ;

			demand1 = demand1 * -1 ;

			MoveWithVelocity(g_pKeyHandle, g_usNodeId, demand1, &ulErrorCode);
			MoveWithVelocity(g_pKeyHandle2, g_usNodeId2, demand2, &ulErrorCode);

			get_position(g_pKeyHandle, g_usNodeId, &pos0, &ulErrorCode);
			get_position(g_pKeyHandle2, g_usNodeId2, &pos1, &ulErrorCode);

			pos0 = pos0 * -1 ;

			pos0_diff = pos0 - pos0_old;
            pos1_diff = pos1 - pos1_old;            
            pos0_old = pos0;
            pos1_old = pos1;


            pos0_mm_diff = pos0_diff / 573.7;
            pos1_mm_diff = pos1_diff / 573.7;

			pos_average_mm_diff = (pos0_mm_diff + pos1_mm_diff) / 2;
			pos_total_mm += pos_average_mm_diff; 

			float phi = ((pos1_mm_diff - pos0_mm_diff) / 360);
			theta += phi;
          
            if (theta >= 2 * PI) {
                theta -= 2 * PI;
            }
            if (theta <= (- 2*PI)) {
                theta += 2*PI;
            }

			y += pos_average_mm_diff * sin(theta);

			x += pos_average_mm_diff * cos(theta);
			
			std::cout << x/1000  <<  y / 1000  << theta << std::endl;

			
			tr.header.stamp = ros::Time::now();
                      
            tr.header.frame_id = odom;
            tr.child_frame_id = base_link;
            
            tr.transform.translation.x = x/1000;   // convert to metres
            tr.transform.translation.y = y/1000;
            tr.transform.translation.z = 0;
            tf2::Quaternion q;
			q.setRPY(0,0,theta);
			tr.transform.rotation.x = q.x();
			tr.transform.rotation.y = q.y();
  			tr.transform.rotation.z = q.z();
  			tr.transform.rotation.w = q.w();


            
            
                       
            tfb.sendTransform(tr); 

            nav_msgs::Odometry odom_msg;
            odom_msg.header.stamp = ros::Time::now();
            odom_msg.header.frame_id = odom;
            odom_msg.pose.pose.position.x = x/1000;
            odom_msg.pose.pose.position.y = y/1000;
            odom_msg.pose.pose.position.z = 0.0;
			tf2::Quaternion q1;
			q1.setRPY(0,0,theta);
            odom_msg.pose.pose.orientation.x = q1.getX();
			odom_msg.pose.pose.orientation.y = q1.getY();
			odom_msg.pose.pose.orientation.z = q1.getZ();
			odom_msg.pose.pose.orientation.w = q1.getW();


            odom_msg.child_frame_id = base_link;
            odom_msg.twist.twist.linear.x = ((pos0_mm_diff + pos1_mm_diff) / 2)/10;          // forward linear velovity
            odom_msg.twist.twist.linear.y = 0.0;                                        // robot does not move sideways
            odom_msg.twist.twist.angular.z = ((pos1_mm_diff - pos0_mm_diff) / 360)*100;      // anglular velocity

            odom_pub.publish(odom_msg);


		}
	

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
	if((lResult = CloseDevice2(&ulErrorCode))!=MMC_SUCCESS)
	{
		LogError("CloseDevice", lResult, ulErrorCode);
		return lResult;
	}
	return 0;
}
