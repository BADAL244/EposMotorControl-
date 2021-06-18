#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <iostream>
using namespace std;

class Teleop
{
public:
 Teleop();
private:
 void callback(const sensor_msgs::Joy::ConstPtr& Joy);
   ros :: NodeHandle n; // node instantiation
 ros::Subscriber sub ;
 ros::Publisher pub ;
   double vlinear, vangular; // we control the turtle speed is adjusted by these two variables
   int axis_ang, axis_lin; // axes [] key
};

Teleop::Teleop()
{
 // we add these several variable parameters, you can easily modify server parameters
 n.param <int> ( "axis_linear", axis_lin, 1); // default axes receiving rate [1]
 n.param <int> ( "axis_angular", axis_ang, 0); // default axes [0] receiving angle
 n.param <double> ( "vel_linear", vlinear, 1); // default linear velocity of 1 m / s
 n.param <double> ( "vel_angular", vangular, 1); // default angular velocity unit 1 rad / s
 pub = n.advertise <geometry_msgs :: Twist> ( "/cmd_vel", 1); // send speed tortoise
sub = n.subscribe<sensor_msgs::Joy>("joy",10,&Teleop::callback,this);
 } // subscription data sent by the game controller
void Teleop::callback(const sensor_msgs::Joy::ConstPtr& Joy)
{
geometry_msgs::Twist v;
 v.linear.x = Joy-> axes [axis_lin] * vlinear; // will handle the data is multiplied by the speed of the game you want, and then sent to the tortoise
v.angular.z =Joy->axes[axis_ang]*vangular;
ROS_INFO("linear:%.3lf angular:%.3lf",v.linear.x,v.angular.z);
pub.publish(v);
}

int main(int argc,char** argv)
{
ros::init(argc, argv, "joy_to_turtlesim");
Teleop teleop_turtle;
ros::spin();
return 0;
}
