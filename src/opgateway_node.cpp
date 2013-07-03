/**
 * OpenPilot to ROS gateway and uavtalk relay
 */

#include "ros/ros.h"

int main(int argc, char **argv)
{
	ros::init(argc, argv, "opgateway");
	ros::NodeHandle nh;


	ros::spin();
	return 0;
}
