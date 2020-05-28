#include <iostream>
#include <ros/ros.h>
#include <aris_msgs/aris_service.h>

uint mat;
uint src;
uint dest;

int main(int argc, char **argv){
    ros::init(argc,argv,"firebase_fetcher_client");
    ros::NodeHandle nh;
    ros::ServiceClient client = nh.serviceClient<aris_msgs::aris_service>("aris_firebase");
    aris_msgs::aris_service srv;
    srv.request.aris_req_status = atoll(argv[1]);
    if(client.call(srv)){
        mat = srv.response.aris_material;
        src = srv.response.aris_source;
        dest = srv.response.aris_destination;
        ROS_INFO("Data from Database : %d,%d,%d",mat,src,dest);
    }
    else{
        ROS_ERROR("Failed to call service aris_firebase");
        return 1;
    }
    return 0;
}