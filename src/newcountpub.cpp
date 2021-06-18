#include <ros/ros.h>
#include <std_msgs/Int64.h>
#include <chrono>
#include <thread>


using namespace std::chrono;
ros::Publisher pub;
std::chrono::seconds interval( 5) ;


long int data1[] = {550 , 0};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "talker");

  ros::NodeHandle n;

   pub = n.advertise<std_msgs::Int64>("/cmd_vel", 1000);

  ros::Rate loop_rate(100);

 
  while (ros::ok())
  {
    std_msgs::Int64 msg;

auto start = high_resolution_clock::now();

    for (int i = 0 ; i < 2 ; i++){
        
        msg.data = data1[i];
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