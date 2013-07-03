/**
 * Test uavobjects library
 */

#include <gtest/gtest.h>

#include "uavobjectmanager.h"
#include "uavobjectsinit.h"
#include "accessorydesired.h"


using namespace openpilot;


UAVObjectManager *objMngr;
int updated, upauto, upmanual, updreq, newobj, newinst;


void objUpdated(UAVObject *obj)
{
	updated++;
	std::cout << "[Updated] " << obj->toString() << std::endl;
}

void objUpdatedAuto(UAVObject *obj)
{
	upauto++;
	std::cout << "[Auto] " << obj->toString() << std::endl;
}

void objUpdatedManual(UAVObject *obj)
{
	upmanual++;
	std::cout << "[Manual] " << obj->toString() << std::endl;
}

void updRequested(UAVObject *obj)
{
	newinst++;
	std::cout << "[Object Requested] " << obj->toString() << std::endl;
}

void newObject(UAVObject *obj)
{
	newobj++;
	std::cout << "[NEW Object] " << obj->toString() << std::endl;
}

void newInstance(UAVObject *obj)
{
	std::cout << "[NEW Instance] " << obj->toString() << std::endl;
}


TEST(UAVObjManager, registerTwo)
{
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

	//EXPECT_EQ(newobj, 1);

	AccessoryDesired *obj2 = new AccessoryDesired();
	obj2->objectUpdated.connect(objUpdated);
	obj2->objectUpdatedAuto.connect(objUpdatedAuto);
	obj2->objectUpdatedManual.connect(objUpdatedManual);
	obj2->updateRequested.connect(updRequested);

	objMngr->registerObject(obj2);

	//EXPECT_EQ(newinst, 1);

	delete obj1;
	delete obj2;
	delete objMngr;
}

int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
