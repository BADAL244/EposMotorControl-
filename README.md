# EposMotorControl-
Motor control  for Mapping and Navigation


Motor specification : 
  maxon motor (326622)
  swiss made --> 2363149
  
Motor control Driver specification 
  EPOS2  24/5
  part no . 367676
  
Encoder Specification : 
  HEDS-5540-A11
  
  
In order to run This code .. 
 intall intel / arm support  driver for EPOS2 
 https://www.maxongroup.com/medias/sys_master/root/8841575039006/EPOS-Linux-Library-En.zip
 
 
 unzip the Linux Library 
``` 
 -------------
 EPOS_Linux_Library
 |
 |
 |---examples
 |---include
 |---lib
 |---misc
 |---EULA.txt
 |---install.sh
 ```
 
 ```
 
$ cd ~/EPOS_Linux_Library
$ chmod +x install.sh
$ ./install.sh
```


this Process will help to run Maxon Linux Library to allow their API to use in development environment in Nvidia Arm devices like (nano , jetson TX1/TX2 , xavier )

<b>Getting Started with ROS on Jetson Tx2 </b>

Installtion 
Open a new terminal by pressing Ctrl + Alt + t or executing the “Terminal” application using the Ubuntu 18 launch system.

Set up the Jetson TX2 to accept software from packages.ros.org:

```
$ sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
```
Add a new apt key:
```
$ sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
```
Update the Debian packages index:
```
$ sudo apt update
```
Install the ROS Desktop package, including support for rqt, rvizand other useful robotics packages:

```
$ sudo apt install ros-melodic-desktop
```
Initialize rosdep. rosdep enables you to easily install system dependencies for source code you want to compile and is required to run some core components in ROS:

```
$ sudo rosdep init 
$ rosdep update
```
It is recommended to load the ROS environment variables automatically when you execute a new shell session. Update your .bashrc script:
```
$ echo "source /opt/ros/melodic/setup.bash" >> ~/.bashrc 
$ source ~/.bashrc
```
<b>Configure a catkin workspace </b>
To start running your own ROS packages or install other packages from source (such as the EPOS2  for example), you must create and configure a catkin workspace.
Install the following dependencies:
```
$ sudo apt-get install cmake python-catkin-pkg python-empy python-nose python-setuptools libgtest-dev python-rosinstall python-rosinstall-generator python-wstool build-essential git
```
Create the catkin root and source folders:

```
$ mkdir -p ~/catkin_ws/src 
$ cd ~/catkin_ws/
```
Configure the catkin workspace by issuing a first “empty” build command:

```
$ catkin_make
```
Finally, update your .bashrc script with the information about the new workspace:
```
$ echo "source ~/catkin_ws/devel/setup.bash" >> ~/.bashrc 
$ source ~/.bashrc
```

Your catkin workspace is now ready to compile your ROS packages from source directly onto the Jetson TX2.
