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


boost::shared_ptr<UAVObjectManager> g_objMngr;
static boost::shared_ptr<TelemetryManager> m_telMngr;
static boost::shared_ptr<UAVTalkRelay> m_relay;


static void telem_connected(void)
{
	ROS_INFO("Telemetry connected");
}

static void telem_disconnected(void)
{
	ROS_INFO("Telemetry disconnected");
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "opgateway");
	ros::NodeHandle nh;
	ros::NodeHandle priv_nh("~");

	std::string serial_port;
	int serial_baudrate;
	std::string relay_bind;
	int relay_port;

	priv_nh.param<std::string>("serial_port", serial_port, "/dev/ttyUSB0");
	priv_nh.param<int>("serial_baudrate", serial_baudrate, 57600);
	priv_nh.param<std::string>("relay_bind", relay_bind, "0.0.0.0");
	priv_nh.param<int>("relay_port", relay_port, 9000);

	// Initialize UAVObject storage
	g_objMngr.reset(new UAVObjectManager());
	UAVObjectsInitialize(g_objMngr.get());

	// Initialize IO devices
	UAVTalkSerialIO *serial_io = new UAVTalkSerialIO(serial_port, serial_baudrate);
	UAVTalkUDPIO *relay_io = new UAVTalkUDPIO(relay_bind, relay_port);

	// Start device IO
	m_telMngr.reset(new TelemetryManager(g_objMngr.get()));
	m_telMngr->connected.connect(telem_connected);
	m_telMngr->disconnected.connect(telem_disconnected);
	m_telMngr->start(serial_io);

	// Relay server
	ROS_INFO_STREAM("UAVTalk Relay listen on " << relay_bind << " port " << relay_port);
	m_relay.reset(new UAVTalkRelay(relay_io, g_objMngr.get()));

	while (ros::ok()) {
		ros::spin();
	}

	return 0;
}

