#include "ros/ros.h"
#include "aris_msgs/aris_coordinate_service.h"
#include <aris_msgs/aris_service.h>
#include <aris_msgs/node_error_msg.h>
#include <std_msgs/Bool.h>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>

ros::ServiceClient nodeMcuClient;
ros::Publisher errorPub;
std_msgs::Bool node_error_msg;
std::map<uint,std::vector<float> > aris_database;

std::vector<uint> node_data_fetcher(int clientArg){
    uint mat;
    uint src;
    uint dest;
    uint quality;
    std::vector<uint> aris_data_array_temp;
    aris_msgs::aris_service srv;
    srv.request.aris_req_status = clientArg;
    if(nodeMcuClient.call(srv)){
        mat = srv.response.aris_material;
        src = srv.response.aris_source;
        dest = srv.response.aris_destination;
        quality = 1;
        ROS_INFO("Data from Database : %d,%d,%d",mat,src,dest);
        aris_data_array_temp.push_back(mat);
        aris_data_array_temp.push_back(src);
        aris_data_array_temp.push_back(dest);
        aris_data_array_temp.push_back(quality);
        return aris_data_array_temp;
    }
    else{
        ROS_ERROR("Failed to call service aris_firebase");
        mat = 999;
        src = 999;
        dest = 999;
        quality = 0;
        aris_data_array_temp.push_back(mat);
        aris_data_array_temp.push_back(src);
        aris_data_array_temp.push_back(dest);
        aris_data_array_temp.push_back(quality);
        return aris_data_array_temp;
    }
}

bool fetcher(aris_msgs::aris_coordinate_service::Request &req,
             aris_msgs::aris_coordinate_service::Response &res)
{
    std::vector<uint> aris_data_array;
        aris_data_array = node_data_fetcher(req.aris_data_request);
        if(aris_data_array[3] == 1){
            node_error_msg.data = false;
            if(aris_data_array[1] == aris_data_array[2]){
                res.aris_source_x_coord = (aris_database[0])[0]; 
                res.aris_source_y_coord = (aris_database[0])[1];
                res.aris_destination_x_coord = (aris_database[0])[0];
                res.aris_destination_y_coord = (aris_database[0])[1];
                ROS_INFO("if condition");
            }
            else{
                ROS_INFO("%d,%d",aris_data_array[1],aris_data_array[2]);
                std::vector<float> sourceCoord = aris_database.at(aris_data_array[1]);
                std::vector<float> destCoord = aris_database.at(aris_data_array[2]);
                ROS_INFO("%f,%f",sourceCoord[0],sourceCoord[1]);
                ROS_INFO("%f,%f",destCoord[0],destCoord[1]);
                /*res.aris_source_x_coord = (aris_database[aris_data_array[1]])[0]; 
                res.aris_source_y_coord = (aris_database[aris_data_array[1]])[1];
                res.aris_destination_x_coord = (aris_database[aris_data_array[2]])[0];
                res.aris_destination_y_coord = (aris_database[aris_data_array[2]])[1];*/
                res.aris_source_x_coord = sourceCoord[0];
                res.aris_source_y_coord = sourceCoord[1];
                res.aris_destination_x_coord = destCoord[0];
                res.aris_destination_y_coord = destCoord[1];
                ROS_INFO("else condition");
            }
        }
        else{
            node_error_msg.data = true;
            res.aris_source_x_coord = std::numeric_limits<float>::infinity(); 
            res.aris_source_y_coord = std::numeric_limits<float>::infinity();
            res.aris_destination_x_coord = std::numeric_limits<float>::infinity();
            res.aris_destination_y_coord = std::numeric_limits<float>::infinity();
        }
    
}

void retParseData(std::string line, uint& doorNumber, float& xCoord, float& yCoord){
    std::vector<size_t> index;
    size_t i =0;
    std::string doorNum;
    std::string x;
    std::string y;
    
    while(i < line.size() - 1 ){
		//check what is the position of ',' or token
		i = line.find(';',i);
		if(i != std::string::npos){
			index.push_back(i);
		}
		else{
			break;
		}
		//slice string into substring using above position
		//increment counter by 1
		i++;
	}
    
    doorNum = line.substr(0,index[0]-0);
	x = line.substr(index[0]+1,index[1]-index[0]-1);
	y = line.substr(index[1]+1,line.size());
    
    doorNumber = atoi(doorNum.c_str());
    xCoord = atof(x.c_str());
    yCoord = atof(y.c_str());
    

} 

void database_fetcher(){
    std::ifstream fp;
    std::string line;
    uint doorNumber;
    float xCoord;
    float yCoord;
    std::vector<float> tempCoord;
    fp.open("/home/youbot/catkin_ws/src/aris/aris_firebase_arduino/src/doorData.txt");
    if(!fp.is_open()){
        ROS_INFO("Failed to create database");
    }else{
        ROS_INFO("Creating the database now");
        //getline(fp,line,';');
        //ROS_INFO(line.c_str());
        while(getline(fp,line)){
            retParseData(line,doorNumber,xCoord,yCoord);
            //tempCoord.push_back(xCoord);
            //tempCoord.push_back(yCoord);
            ROS_INFO("Door number is %d", doorNumber);
            ROS_INFO("x is %f", xCoord);
            ROS_INFO("y is %f", yCoord);
            aris_database.insert(std::pair<uint,std::vector<float> >(doorNumber,tempCoord));
            aris_database[doorNumber].push_back(xCoord);
            aris_database[doorNumber].push_back(yCoord);
        }
    }
    fp.close();     
}


int main(int argc, char **argv)
{
    database_fetcher();
    node_error_msg.data = false;
    for(std::map<uint,std::vector<float> >::const_iterator it =aris_database.begin(); it!=aris_database.end();++it){
        std::cout << it->first << " " << it->second[0] << " " << it->second[1] << "\n";
    }
    ros::init(argc, argv, "aris_data_call");
    ros::NodeHandle n;
    ros::ServiceServer server = n.advertiseService("aris_data_call", fetcher);
    nodeMcuClient = n.serviceClient<aris_msgs::aris_service>("aris_firebase");
    errorPub = n.advertise<std_msgs::Bool>("nodeMcu_error",1000);
    ros::Rate rate(10);
    ROS_INFO("Working on data request");
    while(ros::ok())
    {
        errorPub.publish(node_error_msg);
        ros::spinOnce();
        rate.sleep();
    }
    ros::spin();
    return 0;
}