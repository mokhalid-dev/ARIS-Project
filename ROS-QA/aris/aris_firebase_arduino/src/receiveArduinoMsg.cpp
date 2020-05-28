#include<ros/ros.h>
#include<std_msgs/Bool.h>
#include<iostream>

#include "aris_msgs/aris_arduino_msg.h"
using namespace std;

ros::Subscriber sub_ack;
ros::Subscriber sub_estop;
ros::Subscriber sub_US;

aris_msgs::aris_arduino_msg aris_arduino_msg;

void aris_ack_callbackMethod(std_msgs::Bool receivedData)
{
ROS_INFO("Received ACK %d",receivedData.data);
if(receivedData.data == true)
aris_arduino_msg.aris_ack = true;
else
aris_arduino_msg.aris_ack  = false;
}

void aris_estop_callbackMethod(std_msgs::Bool receivedData)
{
ROS_INFO("Received ESTOP %d",receivedData.data);
if(receivedData.data == true)
aris_arduino_msg.aris_estop = true;
}

void aris_US_callbackMethod(std_msgs::Bool receivedData)
{
ROS_INFO("Received US data %d",receivedData.data);
if(receivedData.data == true)
aris_arduino_msg.Obstacle = true;
else
aris_arduino_msg.Obstacle = false;
}

int main(int argc,char** argv)
{
ros::init(argc,argv,"nodeArduino");
ros::NodeHandle n;
ros::Publisher pub = n.advertise<aris_msgs::aris_arduino_msg>("arduino_Obstacle",1000);
ros::Rate rate(10);
sub_ack = n.subscribe("/aris_ack",10,&aris_ack_callbackMethod);
sub_estop = n.subscribe("/aris_estop",10,&aris_estop_callbackMethod);
sub_US = n.subscribe("/aris_US",10,&aris_US_callbackMethod);
while(ros::ok())
{
pub.publish(aris_arduino_msg);
ros::spinOnce();
rate.sleep();
}


}
