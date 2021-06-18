#include <ros/ros.h>
#include <std_msgs/Int64.h>
#include <chrono>
#include <thread>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Float32.h>


using namespace std::chrono;
ros::Publisher pub;
std::chrono::seconds interval( 10) ;


float linear[] = { 0.00 , .031 , 0.00};
float rotaion[] = { 0.1 , 0.0 , -0.1};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "talker");

  ros::NodeHandle n;

   pub = n.advertise<const geometry_msgs::Twist>("/cmd_vel", 1000);

  ros::Rate loop_rate(100);

 
  while (ros::ok())
  {
    geometry_msgs::Twist msg;

    auto start = high_resolution_clock::now();

    for (int i = 0 ; i < 3 ; i++){
        

        msg.linear.x = linear[i];
        msg.angular.z = rotaion[i];
        pub.publish(msg);
        std::this_thread::sleep_for( interval ) ;

        
    }

auto stop = high_resolution_clock::now();

auto duration = duration_cast<seconds>(stop - start);
  

std::cout << duration.count() << "..........................................."<< std::endl;

    ros::spinOnce();

    loop_rate.sleep();
    


    
  }

  return 0;
}