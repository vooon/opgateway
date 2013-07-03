/**
 ******************************************************************************
 * @file       uavdataobject.cpp
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
#include "uavdataobject.h"

using namespace openpilot;

/** Constructor
 */
UAVDataObject::UAVDataObject(uint32_t objID, bool isSingleInst, bool isSet, const std::string &name) :
	UAVObject(objID, isSingleInst, name)
{
	mobj = NULL;
	this->isSet = isSet;
}

/** Initialize instance ID and assign a metaobject
 */
void UAVDataObject::initialize(uint32_t instID, UAVMetaObject *mobj)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);

	this->mobj = mobj;
	UAVObject::initialize(instID);
}

/** Assign a metaobject
 */
void UAVDataObject::initialize(UAVMetaObject *mobj)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);

	this->mobj = mobj;
}

/** Returns true if this is a data object holding module settings
 */
bool UAVDataObject::isSettings()
{
	return isSet;
}

/** Set the object's metadata
 */
void UAVDataObject::setMetadata(const Metadata &mdata)
{
	if (mobj != NULL) {
		mobj->setData(mdata);
	}
}

/** Get the object's metadata
 */
UAVObject::Metadata UAVDataObject::getMetadata(void)
{
	if (mobj != NULL) {
		return mobj->getData();
	} else {
		return getDefaultMetadata();
	}
}

/** Get the metaobject
 */
UAVMetaObject *UAVDataObject::getMetaObject()
{
	return mobj;
}

