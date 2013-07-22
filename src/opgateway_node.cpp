/**
 * OpenPilot to ROS gateway and uavtalk relay
 */

#include "ros/ros.h"
#include "ros/console.h"

#include "uavobjectmanager.h"
#include "uavobjectsinit.h"
#include "telemetrymanager.h"
#include "uavtalkrelay.h"
#include "iodrivers/uavtalkserialio.h"
#include "iodrivers/uavtalkudpio.h"


using namespace openpilot;


UAVObjectManager *g_objMngr = NULL;


int main(int argc, char **argv)
{
	ros::init(argc, argv, "opgateway");
	ros::NodeHandle nh;
	ros::NodeHandle priv_nh("~");

	std::string serial_port;
	int serial_baudrate;
	std::string server_bind;
	int server_port;

	priv_nh.param<std::string>("serial_port", serial_port, "/dev/ttyUSB0");
	priv_nh.param<int>("serial_baudrate", serial_baudrate, 57600);
	priv_nh.param<std::string>("relay_bind", server_bind, "any TODO");
	priv_nh.param<int>("relay_port", server_port, 9000);

	// Initialize UAVObject storage
	g_objMngr = new UAVObjectManager();
	UAVObjectsInitialize(g_objMngr);

	// Initialize IO devices
	UAVTalkSerialIO *serial_io = new UAVTalkSerialIO(serial_port, serial_baudrate);
	UAVTalkUDPIO *relay_io = new UAVTalkUDPIO(server_bind, server_port);

	// Start device IO
	TelemetryManager *telMngr = new TelemetryManager(g_objMngr);
	telMngr->start(serial_io);

	// Relay server
	UAVTalkRelay *relay = new UAVTalkRelay(relay_io, g_objMngr);

	while (ros::ok()) {
		ros::spin();
	}

	return 0;
}
