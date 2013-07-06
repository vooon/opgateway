/**
 ******************************************************************************
 *
 * @file       uavmetaobject.cpp
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
#include "uavmetaobject.h"

using namespace openpilot;

/** Constructor
 */
UAVMetaObject::UAVMetaObject(uint32_t objID, const std::string &name, UAVObject *parent) :
	UAVObject(objID, true, name)
{
	this->parent = parent;
	// Setup default metadata of metaobject (can not be changed)
	UAVObject::MetadataInitialize(ownMetadata);
	// Initialize parent
	UAVObject::initialize(0);
	// Setup metadata of parent
	parentMetadata = parent->getDefaultMetadata();

	numBytes = sizeof(Metadata);
}

/** Get the parent object
 */
UAVObject *UAVMetaObject::getParentObject()
{
	return parent;
}

/** Set the metadata of the metaobject, this function will
 * do nothing since metaobjects have read-only metadata.
 */
void UAVMetaObject::setMetadata(const Metadata &mdata)
{
}

/** Get the metadata of the metaobject
 */
UAVObject::Metadata UAVMetaObject::getMetadata()
{
	return ownMetadata;
}

/** Get the default metadata
 */
UAVObject::Metadata UAVMetaObject::getDefaultMetadata()
{
	return ownMetadata;
}

/** Set the metadata held by the metaobject
 */
void UAVMetaObject::setData(const Metadata &mdata)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);

	parentMetadata = mdata;

	objectUpdatedAuto(this); // trigger object updated event
	objectUpdated(this);
}

/** get the metadata held by the metaobject
 */
UAVObject::Metadata UAVMetaObject::getData()
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);

	return parentMetadata;
}

/** serialize metadata
 */
ssize_t UAVMetaObject::serialize(uint8_t *dataOut)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);

	memcpy(dataOut, &parentMetadata, sizeof(parentMetadata));
	return sizeof(parentMetadata);
}

/** deserialize metadata
 */
ssize_t UAVMetaObject::deserialize(const uint8_t *dataIn)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);

	memcpy(&parentMetadata, dataIn, sizeof(parentMetadata));

	objectUnpacked(this); // emit unpacked event
	objectUpdated(this); // emit updated event

	return sizeof(parentMetadata);
}

/** Convert UAVObj data to string
 */
std::string UAVMetaObject::toStringData()
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);
	std::ostringstream sout;

	sout << "Data: flags: ";

	if (UAVObject::GetFlightAccess(parentMetadata) == UAVObject::ACCESS_READONLY)
		sout << "FLT:RO ";
	else
		sout << "FLT:RW ";

	if (UAVObject::GetGcsAccess(parentMetadata) == UAVObject::ACCESS_READONLY)
		sout << "GCS:RO ";
	else
		sout << "GCS:RW ";

	if (UAVObject::GetFlightTelemetryAcked(parentMetadata))
		sout << "FLT:Acked ";

	if (UAVObject::GetGcsTelemetryAcked(parentMetadata))
		sout << "GCS:Acked ";

	switch (UAVObject::GetFlightTelemetryUpdateMode(parentMetadata)) {
	case UAVObject::UPDATEMODE_MANUAL:
		sout << "FLT:Manual ";
		break;
	case UAVObject::UPDATEMODE_PERIODIC:
		sout << "FLT:Periodic ";
		break;
	case UAVObject::UPDATEMODE_ONCHANGE:
		sout << "FLT:OnChange ";
		break;
	case UAVObject::UPDATEMODE_THROTTLED:
		sout << "FLT:Throttled ";
		break;
	}

	switch (UAVObject::GetGcsTelemetryUpdateMode(parentMetadata)) {
	case UAVObject::UPDATEMODE_MANUAL:
		sout << "GCS:Manual";
		break;
	case UAVObject::UPDATEMODE_PERIODIC:
		sout << "GCS:Periodic";
		break;
	case UAVObject::UPDATEMODE_ONCHANGE:
		sout << "GCS:OnChange";
		break;
	case UAVObject::UPDATEMODE_THROTTLED:
		sout << "GCS:Throttled";
		break;
	}

	sout << ", flightTelemetryUpdatePeriod: " << parentMetadata.flightTelemetryUpdatePeriod;
	sout << ", gcsTelemetryUpdatePeriod: " << parentMetadata.gcsTelemetryUpdatePeriod;
	sout << ", loggingUpdatePeriod: " << parentMetadata.loggingUpdatePeriod;

	sout << std::endl;
	return sout.str();
}

