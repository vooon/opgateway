/**
 ******************************************************************************
 * @file       uavobjectmanager.h
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
#ifndef UAVOBJECTMANAGER_H
#define UAVOBJECTMANAGER_H

#include "uavobject.h"
#include "uavdataobject.h"
#include "uavmetaobject.h"
#include <vector>
#include <map>

namespace openpilot
{

class UAVObjectManager {
public:
	UAVObjectManager();
	~UAVObjectManager();

	typedef std::vector<UAVObject *> inst_vec;
	typedef std::map<uint32_t, inst_vec> objects_map;

	bool registerObject(UAVDataObject *obj);
	//objects_map getObjects();
	//std::map<uint32_t, std::vector<UAVDataObject *> > getDataObjects();
	//std::map<uint32_t, std::vector<UAVMetaObject *> > getMetaObjects();
	UAVObject *getObject(const std::string &name, uint32_t instId = 0);
	UAVObject *getObject(uint32_t objId, uint32_t instId = 0);
	inst_vec getObjectInstances(const std::string &name);
	inst_vec getObjectInstances(uint32_t objId);
	ssize_t getNumInstances(const std::string &name);
	ssize_t getNumInstances(uint32_t objId);

	// signals:
	boost::signals2::signal<void(UAVObject *)> newObject;
	boost::signals2::signal<void(UAVObject *)> newInstance;

private:
	static const uint32_t MAX_INSTANCES = 1000;

	objects_map objects;
	std::map<std::string, uint32_t> name_to_objid;
	boost::recursive_mutex mutex;

	void addObject(UAVObject *obj);
};

} // namespace openpilot

#endif // UAVOBJECTMANAGER_H
