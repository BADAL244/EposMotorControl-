#include "ros/ros.h"
#include "epos2/Velocity.h"
#include <geometry_msgs/Twist.h>
#include "wrap.h"
#include <iostream>

#include <math.h>
#include <chrono>
#include <sys/time.h>
#include <ctime>

#include <chrono>
#include <thread>

#include "std_msgs/Int32MultiArray.h"


using namespace std::chrono;
long demand1;
long demand2;


int pos0 ;
int pos1 ;


int forward0 ;
int forward1 ;
int turn0 ;
int turn1 ;

float linear;
float angular;

std::chrono::milliseconds interval1(10);

std_msgs::Int32MultiArray array_data;

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
    ros::Subscriber sub = n.subscribe("/cmd_vel",1,&filterVelocityCallback);
	ros::Publisher tick_sender = n.advertise<std_msgs::Int32MultiArray>("wheels" , 100);
 
    


	ROS_INFO("Ready to move.");
	while(ros::ok()){

		ros::spinOnce();
		std::this_thread::sleep_for( interval1) ;

			
        forward0 = linear* 17389;
        forward1 = linear* 17389;

        forward0 = forward0 * -1;

        turn0 =    angular* 3695;
        turn1 =    angular* 3695;

        // demand1 = forward0 + turn0 ;
        // demand2 = forward1 + turn1 ;
		if(angular != 0){
			if(angular > 0){
				MoveWithVelocity(g_pKeyHandle, g_usNodeId, turn0, &ulErrorCode);
        		MoveWithVelocity(g_pKeyHandle2, g_usNodeId2, turn1, &ulErrorCode);
			}
			else{
				MoveWithVelocity(g_pKeyHandle, g_usNodeId, turn0, &ulErrorCode);
        		MoveWithVelocity(g_pKeyHandle2, g_usNodeId2, turn1, &ulErrorCode);
			}
		}
		else{
			    MoveWithVelocity(g_pKeyHandle, g_usNodeId, forward0, &ulErrorCode);
    			MoveWithVelocity(g_pKeyHandle2, g_usNodeId2, forward1, &ulErrorCode);
			}
		
        

        get_position(g_pKeyHandle, g_usNodeId, &pos0, &ulErrorCode);
        get_position(g_pKeyHandle2, g_usNodeId2, &pos1, &ulErrorCode);



        

        pos0 = pos0 * -1 ;

        array_data.data.clear();

        for(int i= 0 ; i < 1 ; i++){
            array_data.data.push_back(pos0);
            array_data.data.push_back(pos1);
        }


        tick_sender.publish(array_data);

        ROS_INFO("hey i started sending Tick as distance please check");
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
