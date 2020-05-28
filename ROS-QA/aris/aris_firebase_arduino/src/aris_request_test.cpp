#include <iostream>
#include <ros/ros.h>
#include "aris_msgs/aris_coordinate_service.h"

float srcXcoord;
float srcYcoord;
float destXcoord;
float destYcoord;

int main(int argc, char **argv){
    ros::init(argc,argv,"firebase_requester");
    ros::NodeHandle nh;
    ros::ServiceClient client = nh.serviceClient<aris_msgs::aris_coordinate_service>("aris_data_call");
    aris_msgs::aris_coordinate_service srv;
    srv.request.aris_data_request = 0;
    if(client.call(srv)){
        srcXcoord = srv.response.aris_source_x_coord;
        srcYcoord = srv.response.aris_source_y_coord;
        destXcoord = srv.response.aris_destination_x_coord;
        destYcoord = srv.response.aris_destination_y_coord;               
        ROS_INFO("Data of src : %f,%f",srcXcoord,srcYcoord);
        ROS_INFO("Data of dest : %f,%f",destXcoord,destYcoord);
    }
    else{
        ROS_ERROR("Failed to call service firebase_requester");
        return 1;
    }
    return 0;
}