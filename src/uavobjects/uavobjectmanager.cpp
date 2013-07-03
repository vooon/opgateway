/**
 ******************************************************************************
 * @file       uavobjectmanager.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @author     Vladimir Ermakov, Copyright (C) 2013.
 * @see        The GNU Public License (GPL) Version 3
 * @brief      The UAVUObjects
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include "uavobjectmanager.h"

using namespace openpilot;

/** Constructor
 */
UAVObjectManager::UAVObjectManager()
{
}

UAVObjectManager::~UAVObjectManager()
{
}

/** Register an object with the manager. This function must be called for all newly created instances.
 * A new instance can be created directly by instantiating a new object or by calling clone() of
 * an existing object. The object will be registered and will be properly initialized so that it can accept
 * updates.
 */
bool UAVObjectManager::registerObject(UAVDataObject *obj)
{
	boost::recursive_mutex::scoped_lock lock(mutex);

	objects_map::iterator it;

	// Check if object type alredy in map
	it = objects.find(obj->getObjID());
	if (it != objects.end()) {
		inst_vec &objs = it->second;

		assert(objs.size());
		assert(objs[0]->getObjID() == obj->getObjID()); // map[id] != objID

		// check single instance
		if (obj->isSingleInstance())
			return true;

		// The object type has alredy been added, so now we need to initialize the new instance with the appropriate id
		// There is a single metaobject for all object instances of this type, so no need to create a new one
		// Get object type metaobject from existing instance
		UAVDataObject *refObj = dynamic_cast<UAVDataObject *>(objs[0]);
		if (refObj == NULL)
			return false;

		UAVMetaObject *mobj = refObj->getMetaObject();

		// If the instance ID is specified and not at the default value (0) then we need to make sure
		// that there are no gaps in the instance list. If gaps are found then then additional instances
		// will be created.
		if ((obj->getInstID() > 0) && (obj->getInstID() < MAX_INSTANCES)) {
			for (int instidx = 0; instidx < objs.size(); ++instidx) {
				if (objs[instidx]->getInstID() == obj->getInstID()) {
					// Instance conflict, do not add
					return false;
				}
			}

			// Check if there are any gaps between the requested instance ID and the ones in the list,
			// if any then create the missing instances.
			for (uint32_t instidx = objs.size(); instidx < obj->getInstID(); ++instidx) {
				UAVDataObject *cobj = obj->clone(instidx);
				cobj->initialize(mobj);
				objs.push_back(cobj);
				getObject(cobj->getObjID())->newInstance(cobj);

				newInstance(cobj); // emit signal
			}

			// Finally, initialize the actual object instance
			obj->initialize(mobj);

		} else if (obj->getInstID() == 0) {
			// Assign the next available ID and initialize the object instance
			obj->initialize(objs.size(), mobj);

		} else {
			return false;
		}

		// Add the actual object instance in the list
		objs.push_back(obj);
		// emit signals
		getObject(obj->getObjID())->newInstance(obj);
		newInstance(obj);

		return true;

	}

	// If this point is reached then this is the first time this object type (ID) is added in the list
	// create a new list of the instances, add in the object collection and create the object's metaobject
	// Create metaobject
	std::string mname = obj->getName();
	mname.append("Meta");
	UAVMetaObject *mobj = new UAVMetaObject(obj->getObjID() + 1, mname, obj);
	// Initialize object
	obj->initialize(0, mobj);
	// Add to list
	addObject(obj);
	addObject(mobj);

	return true;
}

void UAVObjectManager::addObject(UAVObject *obj)
{
	// Add to list
	inst_vec vec;
	vec.push_back(obj);
	objects[obj->getObjID()] = vec;
	name_to_objid[obj->getName()] = obj->getObjID();

	newObject(obj); // emit signal
}

/*
 *
 */


/** Get a specific object given its name and instance ID
 * @returns The object is found or NULL if not
 */
UAVObject *UAVObjectManager::getObject(const std::string &name, uint32_t instId)
{
	std::map<std::string, uint32_t>::iterator it;

	it = name_to_objid.find(name);
	if (it == name_to_objid.end())
		return NULL;

	return getObject(it->second, instId);
}

/** Get a specific object given its object and instance ID
 * @returns The object is found or NULL if not
 */
UAVObject *UAVObjectManager::getObject(uint32_t objId, uint32_t instId)
{
	boost::recursive_mutex::scoped_lock lock(mutex);
	objects_map::iterator it;

	it = objects.find(objId);
	if (it != objects.end()) {
		inst_vec &objs = it->second;

		for (inst_vec::iterator inst_it = objs.begin(); inst_it != objs.end(); ++inst_it) {
			if ((*inst_it)->getInstID() == instId)
				return *inst_it;
		}
	}

	return NULL;
}

/** Get all the instances of the object specified by name
 */
UAVObjectManager::inst_vec UAVObjectManager::getObjectInstances(const std::string &name)
{
	std::map<std::string, uint32_t>::iterator it;

	it = name_to_objid.find(name);
	if (it == name_to_objid.end())
		return inst_vec();

	return getObjectInstances(it->second);
}

/** Get all the instances of the object specified by its ID
 */
UAVObjectManager::inst_vec UAVObjectManager::getObjectInstances(uint32_t objId)
{
	boost::recursive_mutex::scoped_lock lock(mutex);
	objects_map::iterator it;

	it = objects.find(objId);
	if (it != objects.end())
		return it->second;

	return inst_vec();
}

/** Get the number of instances for an object given its name
 */
ssize_t UAVObjectManager::getNumInstances(const std::string &name)
{
	std::map<std::string, uint32_t>::iterator it;

	it = name_to_objid.find(name);
	if (it == name_to_objid.end())
		return -1;

	return getNumInstances(it->second);
}

/** Get the number of instances for an object given its ID
 */
ssize_t UAVObjectManager::getNumInstances(uint32_t objId)
{
	boost::recursive_mutex::scoped_lock lock(mutex);
	objects_map::iterator it;

	it = objects.find(objId);
	if (it != objects.end())
		return it->second.size();

	return -1;
}



