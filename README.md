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
 
$ cd ~/EPOS_Linux_Library
$ chmod +x install.sh
$ ./install.sh



this Process will help to run Maxon Linux Library to allow their API to use in development environment in Nvidia Arm devices like (nano , jetson TX1/TX2 , xavier )
