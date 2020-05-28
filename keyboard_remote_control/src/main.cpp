
#include "ros/ros.h"
#include <iostream>
#include <vector>
#include <signal.h>
#include <curses.h>

//New line include : chackrapani
#include "aris_msgs/Obstacle.h"

#include "youbot_driver/youbot/YouBotBase.hpp"


using namespace std;
using namespace youbot;

bool aris_ObstacleAvailable=true;

void sigintHandler(int signal) {
 
  ros::shutdown();
  printf("End!\n\r");
}


//New function to print message : chackrapani
void rogerThat(const aris_msgs::Obstacle::ConstPtr& msg) {
  if(msg->Obstacle==true){
    ROS_INFO("No Obstacle available");
    ROS_INFO("\n");

    aris_ObstacleAvailable=true;

}else{
  ROS_INFO("Obstacle available");
  ROS_INFO("\n");
  aris_ObstacleAvailable=false;
}
}
//end


int main(int argc, char **argv) {

  signal(SIGINT, sigintHandler);

  ros::init(argc,argv,"main");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("Obstacle",1000,rogerThat);

  ros::Rate loop_rate(10);

  try {

 
    ConfigFile configfile("/applications.cfg", std::string(CONFIG_DIR) );

    int ch = 0;
    double linearVel = 0.05; //meter_per_second
    double angularVel = 0.2; //radian_per_second

    configfile.readInto(linearVel,  "KeyboardRemoteContol", "TanslationalVelocity_[meter_per_second]");
    configfile.readInto(angularVel, "KeyboardRemoteContol", "RotationalVelocity_[radian_per_second]");

    YouBotBase myYouBotBase("youbot-base", YOUBOT_CONFIGURATIONS_DIR);
		myYouBotBase.doJointCommutation();

    JointVelocitySetpoint setVel;
    quantity<si::velocity> longitudinalVelocity = 0 * meter_per_second;
    quantity<si::velocity> transversalVelocity = 0 * meter_per_second;
    quantity<si::angular_velocity> angularVelocity = 0 * radian_per_second;

    (void) initscr(); /* initialize the curses library */
    keypad(stdscr, TRUE); /* enable keyboard mapping */
    (void) nonl(); /* tell curses not to do NL->CR/NL on output */
    (void) cbreak(); /* take input chars one at a time, no wait for \n */
    // (void) echo(); /* echo input - in color */

    def_prog_mode();

    refresh();
    printf("up = drive forward\n\r"
            "down = drive backward\n\r"
            "any other key = stop\n\r\n");
    
    refresh();

ch = getch();
if(ch == KEY_UP)
{       
    while (ros::ok()) {
      

     // switch (ch) {
        
       // case KEY_DOWN:

           // longitudinalVelocity = -linearVel * meter_per_second;
          //  transversalVelocity = 0 * meter_per_second;
           // angularVelocity = 0 * radian_per_second;
          //  LOG(info) << "drive backward";
          //  printf("\n");
          //  ROS_INFO("ARIS is moving backward");
         // break;

       // case KEY_UP:

          if(aris_ObstacleAvailable==true){
            longitudinalVelocity = linearVel * meter_per_second;
            transversalVelocity = 0 * meter_per_second;
            angularVelocity = 0 * radian_per_second;
            LOG(info) << "drive forward";
            printf("\n");
            ROS_INFO("moving forward");            
          }else{
            longitudinalVelocity = 0 * meter_per_second;
            transversalVelocity = 0 * meter_per_second;
            angularVelocity = 0 * radian_per_second;
            LOG(info) << "stop";
            printf("\n");
            ROS_INFO("Obstacle detected : STOP\n");
          }
         // break;

      /*  default:

            longitudinalVelocity = 0 * meter_per_second;
            transversalVelocity = 0 * meter_per_second;
            angularVelocity = 0 * radian_per_second;
            LOG(info) << "stop";
            printf("\n");
            ROS_INFO("default command exeuted");
          break;

      }*/

      myYouBotBase.setBaseVelocity(longitudinalVelocity, transversalVelocity, angularVelocity);

    ros::spinOnce();
    loop_rate.sleep(); 
      
  //    refresh();
      //SLEEP_MILLISEC(100);
    }
}


    setVel.angularVelocity = 0 * radian_per_second;
    myYouBotBase.getBaseJoint(1).setData(setVel);
    myYouBotBase.getBaseJoint(2).setData(setVel);
    myYouBotBase.getBaseJoint(3).setData(setVel);
    myYouBotBase.getBaseJoint(4).setData(setVel);

    endwin();
    SLEEP_MILLISEC(500);

  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "unhandled exception" << std::endl;
  }
   
  
  return 0;
}

