/**
 * Test uavobjects library
 */

#include <gtest/gtest.h>

#include "uavobjectmanager.h"
#include "uavobjectsinit.h"
#include "accessorydesired.h"


using namespace openpilot;


UAVObjectManager *objMngr;

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


TEST(UAVObjManager, registerTwo)
{
	boost::system_time t;
	objMngr = new UAVObjectManager();

	newobj = 0;
	newinst = 0;

	objMngr->newObject.connect(newObject);
	objMngr->newInstance.connect(newInstance);

	AccessoryDesired *obj1 = new AccessoryDesired();
	obj1->objectUpdated.connect(objUpdated);
	obj1->objectUpdatedAuto.connect(objUpdatedAuto);
	obj1->objectUpdatedManual.connect(objUpdatedManual);
	obj1->updateRequested.connect(updRequested);

	objMngr->registerObject(obj1);

	t = boost::get_system_time() +
		boost::posix_time::milliseconds(100);
	mutex.timed_lock(t);
	EXPECT_GT(newobj, 0);

	AccessoryDesired *obj2 = new AccessoryDesired();
	obj2->objectUpdated.connect(objUpdated);
	obj2->objectUpdatedAuto.connect(objUpdatedAuto);
	obj2->objectUpdatedManual.connect(objUpdatedManual);
	obj2->updateRequested.connect(updRequested);

	objMngr->registerObject(obj2);

	t = boost::get_system_time() +
		boost::posix_time::milliseconds(100);
	mutex.timed_lock(t);
	EXPECT_GT(newinst, 0);
}

TEST(UAVObjManager, registerAll)
{
	//objMngr = new UAVObjectManager();

	UAVObjectsInitialize(objMngr);
}

TEST(UAVObjManager, signals)
{
	//objMngr = new UAVObjectManager();

	AccessoryDesired *obj1 = AccessoryDesired::GetInstance(objMngr, 0);
	AccessoryDesired *obj2 = AccessoryDesired::GetInstance(objMngr, 1);

	ASSERT_NE(obj1, (void*)NULL);
	ASSERT_NE(obj2, (void*)NULL);

	AccessoryDesired::DataFields data = obj1->getData();
	UAVObject::Metadata mdata = obj1->getMetadata();

        mdata.gcsTelemetryUpdatePeriod = 123;
        obj1->setMetadata(mdata);

	std::cout << "[Meta of obj1]\n";
	std::cout << obj1->getMetaObject()->toString();
	std::cout << obj1->getMetaObject()->toStringData();

	std::cout << "[Meta of obj2]\n";
	std::cout << obj2->getMetaObject()->toString();
	std::cout << obj2->getMetaObject()->toStringData();
}

void bind_objUpdated(UAVObject *obj)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);
	bind_updated++;
	std::cout << "[BIND Updated] " << obj->toString();

	UAVDataObject *dobj = dynamic_cast<UAVDataObject *>(obj);
	if (dobj)
		std::cout << dobj->toStringData();

}

TEST(UAVObjManager, bind_connect_disconneect)
{
	boost::system_time t;
	AccessoryDesired::DataFields data;
	AccessoryDesired *obj1 = AccessoryDesired::GetInstance(objMngr, 0);

	ASSERT_NE(obj1, (void*)NULL);

	bind_updated = 0;
	obj1->objectUpdated.connect(boost::bind(bind_objUpdated, _1));

	data = obj1->getData();
	data.AccessoryVal++;
	obj1->setData(data);

	t = boost::get_system_time() +
		boost::posix_time::milliseconds(100);
	mutex.timed_lock(t);

	EXPECT_EQ(bind_updated, 1);

	obj1->objectUpdated.disconnect(boost::bind(bind_objUpdated, _1));

	data = obj1->getData();
	data.AccessoryVal++;
	obj1->setData(data);

	t = boost::get_system_time() +
		boost::posix_time::milliseconds(100);
	mutex.timed_lock(t);

	EXPECT_EQ(bind_updated, 1);
}

int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
