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

#include <geometry_msgs/TransformStamped.h>

#include <geometry_msgs/PointStamped.h>
#include <geometry_msgs/PoseStamped.h>
#define PI 3.14159

#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;
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


auto previousMillis = 0;
float loopTime = 10 ;

int pos0 ;
int pos1 ;

int vel0;
int vel1;

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

float linear;
float angular;

std::chrono::milliseconds interval1(10);







unsigned int ulErrorCode = 0;
int lResult = MMC_FAILED;

void filterVelocityCallback(const geometry_msgs::Twist& msg){

    
    linear = msg.linear.x;
    angular = msg.angular.z;

    

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
	nav_msgs::Odometry odom_msg;
	tf::TransformBroadcaster tfb;
    ros::Subscriber sub = n.subscribe("/cmd_vel",1,&filterVelocityCallback);
	ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 50);
 
    


	ROS_INFO("Ready to move.");
	while(ros::ok()){

		ros::spinOnce();
		


		
		std::this_thread::sleep_for( interval1) ;

			
			forward0 = linear* 17389;
			forward1 = linear* 17389;

			forward0 = forward0 * -1;


			turn0 =    angular* 3695;
			turn1 =    angular* 3695;

			demand1 = forward0 + turn0 ;
			demand2 = forward1 + turn1 ;

			
			MoveWithVelocity(g_pKeyHandle, g_usNodeId, demand1, &ulErrorCode);
			MoveWithVelocity(g_pKeyHandle2, g_usNodeId2, demand2, &ulErrorCode);

			get_position(g_pKeyHandle, g_usNodeId, &pos0, &ulErrorCode);
			get_position(g_pKeyHandle2, g_usNodeId2, &pos1, &ulErrorCode);

			get_position(g_pKeyHandle, g_usNodeId, &vel0, &ulErrorCode);
			get_position(g_pKeyHandle2, g_usNodeId2, &vel1, &ulErrorCode);

			

			pos0 = pos0 * -1 ;


			cout << pos0 << ".................."  << pos1  << "........." << endl;


			pos0_diff = pos0 - pos0_old;
            pos1_diff = pos1 - pos1_old;            
            pos0_old = pos0;
            pos1_old = pos1;


            pos0_mm_diff = pos0_diff / 642.0;
            pos1_mm_diff = pos1_diff / 642.0;

			pos_average_mm_diff = (pos0_mm_diff + pos1_mm_diff) / 2;
			pos_total_mm += pos_average_mm_diff; 

			float phi = ((pos1_mm_diff - pos0_mm_diff) / 420);


            if(abs(pos0_mm_diff - pos1_mm_diff) <= 20 && pos1_mm_diff * pos0_mm_diff >0){
                y += pos_average_mm_diff * sin(theta);
                x += pos_average_mm_diff * cos(theta);
				cout << "loop 1 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;

            }

            else if(abs(pos0_mm_diff - pos1_mm_diff) <= 20 && pos1_mm_diff * pos0_mm_diff < 0){
                theta += phi;
          
                if (theta >= 2 * PI) {
                theta -= 2 * PI;
                }
                if (theta <= (- 2*PI)) {
                theta += 2*PI;
                }
				cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<< loop 2 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
            }
            else
            {
                auto alpha = 1.57 - phi /2 ;
                double delta_x = pos_average_mm_diff * cos(alpha);
                double delta_y = pos_average_mm_diff * sin(alpha);
				float temp;
				temp = x;
                x = x * sin(alpha) + y * cos(alpha) + delta_x ;
			
                y = temp * cos(alpha) - y * sin(alpha) + delta_y ;

				cout << delta_x << "<<<<<<<<<<<<<<<<<<<<<<<<<<<" << delta_y <<  endl;

                theta += phi;

                if (theta >= 2 * PI) {
                theta -= 2 * PI;
                }
                if (theta <= (- 2*PI)) {
                theta += 2*PI;
                }
				cout <<"..................." << "<<<<<<<<<<<<<<<" << ">>>>>>>>>>>>>>>" << endl;
            }

			
			std::cout << x/1000  << " .........."  <<  y / 1000  << "...................."     <<  theta << std::endl;

			geometry_msgs::TransformStamped tr;
			
			tr.header.stamp = ros::Time::now();
                      
            tr.header.frame_id = odom;
            tr.child_frame_id = base_link;
            
            tr.transform.translation.x = x/1000;   // convert to metres
            tr.transform.translation.y = y/1000;
            tr.transform.translation.z = 0;


			tr.transform.rotation.x = tf::createQuaternionFromYaw(theta).getX();
			tr.transform.rotation.y = tf::createQuaternionFromYaw(theta).getY();
			tr.transform.rotation.z = tf::createQuaternionFromYaw(theta).getZ();
			tr.transform.rotation.w = tf::createQuaternionFromYaw(theta).getW();

            tfb.sendTransform(tr); 

            nav_msgs::Odometry odom_msg;
            odom_msg.header.stamp = ros::Time::now();
            odom_msg.header.frame_id = odom;
            odom_msg.pose.pose.position.x = x/1000;
            odom_msg.pose.pose.position.y = y/1000;
            odom_msg.pose.pose.position.z = 0.0;
			tf::Quaternion q1;
			q1.setRPY(0,0,theta);
            odom_msg.pose.pose.orientation.x = q1.getX();
			odom_msg.pose.pose.orientation.y = q1.getY();
			odom_msg.pose.pose.orientation.z = q1.getZ();
			odom_msg.pose.pose.orientation.w = q1.getW();


            odom_msg.child_frame_id = base_link;
            odom_msg.twist.twist.linear.x = ((pos0_mm_diff + pos1_mm_diff) / 2)/10;          // forward linear velovity
            odom_msg.twist.twist.linear.y = 0.0;                                        // robot does not move sideways
            odom_msg.twist.twist.angular.z = ((pos1_mm_diff - pos0_mm_diff) / 420)*100;      // anglular velocity

            odom_pub.publish(odom_msg);


		
	

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
