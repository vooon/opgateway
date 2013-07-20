/**
 * Test uavobjects library
 */

#include <gtest/gtest.h>

#include "uavobjectmanager.h"
#include "uavobjectsinit.h"
#include "telemetrymanager.h"
#include "iodrivers/uavtalkserialio.h"
#include "systemstats.h"
#include "flightstatus.h"
#include "flighttelemetrystats.h"
#include "gcstelemetrystats.h"


using namespace openpilot;


UAVObjectManager *objMngr;
TelemetryManager *telMngr;

boost::recursive_timed_mutex mutex;
int updated, upauto, upmanual, upreq, newobj, newinst, bind_updated;


void objUpdated(UAVObject *obj)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);
	updated++;
	std::cout << "[Updated] " << obj->toString();

	UAVDataObject *dobj = dynamic_cast<UAVDataObject *>(obj);
	if (dobj)
		std::cout << dobj->toStringData();
}

void objUpdatedAuto(UAVObject *obj)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);
	upauto++;
	std::cout << "[Auto] " << obj->toString();

	UAVDataObject *dobj = dynamic_cast<UAVDataObject *>(obj);
	if (dobj)
		std::cout << dobj->toStringData();
}

void objUpdatedManual(UAVObject *obj)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);
	upmanual++;
	std::cout << "[Manual] " << obj->toString();

	UAVDataObject *dobj = dynamic_cast<UAVDataObject *>(obj);
	if (dobj)
		std::cout << dobj->toStringData();
}

void updRequested(UAVObject *obj)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);
	upreq++;
	std::cout << "[Object Requested] " << obj->toString();

	UAVDataObject *dobj = dynamic_cast<UAVDataObject *>(obj);
	if (dobj)
		std::cout << dobj->toStringData();
}

void newObject(UAVObject *obj)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);
	newobj++;
	std::cout << "[NEW Object] " << obj->toString();

	UAVDataObject *dobj = dynamic_cast<UAVDataObject *>(obj);
	if (dobj)
		std::cout << dobj->toStringData();
}

void newInstance(UAVObject *obj)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);
	newinst++;
	std::cout << "[NEW Instance] " << obj->toString();

	UAVDataObject *dobj = dynamic_cast<UAVDataObject *>(obj);
	if (dobj)
		std::cout << dobj->toStringData();
}

void telConnected(void)
{
	std::cout << "[telemetry manager] connected" << std::endl;
}

void telDisconnected(void)
{
	std::cout << "[telemetry manager] disconnected" << std::endl;
}

TEST(UAVTalkManager, init_talk)
{
	boost::system_time t;
	objMngr = new UAVObjectManager();

	UAVObjectsInitialize(objMngr);

	objMngr->newObject.connect(newObject);
	objMngr->newInstance.connect(newInstance);

	SystemStats *sysSts = SystemStats::GetInstance(objMngr, 0);
	sysSts->objectUpdated.connect(objUpdated);

	UAVObject::Metadata mdata = sysSts->getMetadata();
	mdata.gcsTelemetryUpdatePeriod = 500;
	UAVObject::SetGcsTelemetryUpdateMode(mdata, UAVObject::UPDATEMODE_PERIODIC);
	sysSts->setMetadata(mdata);

	FlightStatus *flSt = FlightStatus::GetInstance(objMngr, 0);
	flSt->objectUpdated.connect(objUpdated);

	FlightTelemetryStats *flSts = FlightTelemetryStats::GetInstance(objMngr, 0);
	flSts->objectUpdated.connect(objUpdated);

	GCSTelemetryStats *gcsSts = GCSTelemetryStats::GetInstance(objMngr, 0);
	gcsSts->objectUpdated.connect(objUpdated);

	UAVTalkSerialIO *ser = new UAVTalkSerialIO("/dev/ttyUSB0", 57600);
	//ser->write((const uint8_t *)"some crap for write debugging", 29);
	telMngr = new TelemetryManager(objMngr);
	telMngr->connected.connect(telConnected);
	telMngr->disconnected.connect(telDisconnected);
	telMngr->start(ser);

	while(true) {};

	t = boost::get_system_time() +
		boost::posix_time::milliseconds(10000);
	mutex.timed_lock(t);
	EXPECT_GT(newinst, 0);
}

int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
