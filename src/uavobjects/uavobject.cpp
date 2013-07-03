/**
 ******************************************************************************
 * @file       uavobject.cpp
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

#include "uavobject.h"

// Macros
#define SET_BITS(var, shift, value, mask) var = (var & ~(mask << shift)) | (value << shift);

using namespace openpilot;

/** Constructor
 * @param objID The object ID
 * @param isSingleInst True if this object can only have a single instance
 * @param name Object name
 */
UAVObject::UAVObject(uint32_t objID, bool isSingleInst, const std::string &name)
{
	this->objID  = objID;
	this->instID = 0;
	this->isSingleInst = isSingleInst;
	this->name   = name;
}

/** Initialize object with its instance ID
 */
void UAVObject::initialize(uint32_t instID)
{
	boost::recursive_timed_mutex::scoped_lock lock(mutex);

	this->instID = instID;
}

/** Get the object ID
 */
uint32_t UAVObject::getObjID()
{
	return objID;
}

/** Get the instance ID
 */
uint32_t UAVObject::getInstID()
{
	return instID;
}

/** Returns true if this is a single instance object
*/
bool UAVObject::isSingleInstance()
{
	return isSingleInst;
}

/** Get the name of the object
*/
std::string UAVObject::getName()
{
	return name;
}

/** Get the total number of bytes of the object's data
*/
size_t UAVObject::getNumBytes()
{
	return numBytes;
}

/** Request that this object is updated with the latest values from the autopilot
*/
void UAVObject::requestUpdate()
{
	updateRequested(this);
}

/** Signal that the object has been updated
*/
void UAVObject::updated()
{
	objectUpdatedManual(this);
	objectUpdated(this);
}

#if 0
/** Lock mutex of this object
*/
void UAVObject::lock()
{
	mutex.lock();
}

/** Lock mutex of this object
*/
bool UAVObject::lock(int timeoutMs)
{
	boost::system_time t = boost::get_system_time() +
		boost::posix_time::milliseconds(timeoutMs);

	return mutex.timed_lock(t);
}

/** Unlock mutex of this object
*/
void UAVObject::unlock()
{
	mutex.unlock();
}

/** Get object's mutex
*/
boost::recursive_timed_mutex *UAVObject::getMutex()
{
	return &mutex;
}
#endif

/** Return a string with the object information (only the header)
 */
std::string UAVObject::toString()
{
	std::ostringstream sout;

	sout << getName() <<
		" (ID: 0x" << std::hex << getObjID() << std::dec <<
		", InstID: " << getInstID() <<
		", NumBytes: " << getNumBytes() <<
		", SInst: " << isSingleInstance() <<
		")" << std::endl;

	return sout.str();
}

/** Initialize a UAVObjMetadata object.
 * \param[in] metadata The metadata object
 */
void UAVObject::MetadataInitialize(UAVObject::Metadata &metadata)
{
    metadata.flags =
        ACCESS_READWRITE << UAVOBJ_FLT_ACCESS_SHIFT |
        ACCESS_READWRITE << UAVOBJ_GCS_ACCESS_SHIFT |
        1 << UAVOBJ_FLT_TELEMETRY_ACKED_SHIFT |
        1 << UAVOBJ_GCS_TELEMETRY_ACKED_SHIFT |
        UPDATEMODE_ONCHANGE << UAVOBJ_FLT_TELEMETRY_UPDATE_MODE_SHIFT |
        UPDATEMODE_ONCHANGE << UAVOBJ_GCS_TELEMETRY_UPDATE_MODE_SHIFT;
    metadata.flightTelemetryUpdatePeriod = 0;
    metadata.gcsTelemetryUpdatePeriod    = 0;
    metadata.loggingUpdatePeriod = 0;
}

/** Get the UAVObject metadata access member
 * \param[in] metadata The metadata object
 * \return the access type
 */
UAVObject::AccessMode UAVObject::GetFlightAccess(const UAVObject::Metadata &metadata)
{
    return UAVObject::AccessMode((metadata.flags >> UAVOBJ_FLT_ACCESS_SHIFT) & 1);
}

/** Set the UAVObject metadata access member
 * \param[in] metadata The metadata object
 * \param[in] mode The access mode
 */
void UAVObject::SetFlightAccess(UAVObject::Metadata &metadata, UAVObject::AccessMode mode)
{
    SET_BITS(metadata.flags, UAVOBJ_FLT_ACCESS_SHIFT, mode, 1);
}

/** Get the UAVObject metadata GCS access member
 * \param[in] metadata The metadata object
 * \return the GCS access type
 */
UAVObject::AccessMode UAVObject::GetGcsAccess(const UAVObject::Metadata &metadata)
{
    return UAVObject::AccessMode((metadata.flags >> UAVOBJ_GCS_ACCESS_SHIFT) & 1);
}

/** Set the UAVObject metadata GCS access member
 * \param[in] metadata The metadata object
 * \param[in] mode The access mode
 */
void UAVObject::SetGcsAccess(UAVObject::Metadata &metadata, UAVObject::AccessMode mode)
{
    SET_BITS(metadata.flags, UAVOBJ_GCS_ACCESS_SHIFT, mode, 1);
}

/** Get the UAVObject metadata telemetry acked member
 * \param[in] metadata The metadata object
 * \return the telemetry acked boolean
 */
bool UAVObject::GetFlightTelemetryAcked(const UAVObject::Metadata &metadata)
{
    return (metadata.flags >> UAVOBJ_FLT_TELEMETRY_ACKED_SHIFT) & 1;
}

/** Set the UAVObject metadata telemetry acked member
 * \param[in] metadata The metadata object
 * \param[in] val The telemetry acked boolean
 */
void UAVObject::SetFlightTelemetryAcked(UAVObject::Metadata &metadata, bool val)
{
    SET_BITS(metadata.flags, UAVOBJ_FLT_TELEMETRY_ACKED_SHIFT, val, 1);
}

/** Get the UAVObject metadata GCS telemetry acked member
 * \param[in] metadata The metadata object
 * \return the telemetry acked boolean
 */
bool UAVObject::GetGcsTelemetryAcked(const UAVObject::Metadata &metadata)
{
    return (metadata.flags >> UAVOBJ_GCS_TELEMETRY_ACKED_SHIFT) & 1;
}

/** Set the UAVObject metadata GCS telemetry acked member
 * \param[in] metadata The metadata object
 * \param[in] val The GCS telemetry acked boolean
 */
void UAVObject::SetGcsTelemetryAcked(UAVObject::Metadata &metadata, bool val)
{
    SET_BITS(metadata.flags, UAVOBJ_GCS_TELEMETRY_ACKED_SHIFT, val, 1);
}

/** Get the UAVObject metadata telemetry update mode
 * \param[in] metadata The metadata object
 * \return the telemetry update mode
 */
UAVObject::UpdateMode UAVObject::GetFlightTelemetryUpdateMode(const UAVObject::Metadata &metadata)
{
    return UAVObject::UpdateMode((metadata.flags >> UAVOBJ_FLT_TELEMETRY_UPDATE_MODE_SHIFT) & UAVOBJ_UPDATE_MODE_MASK);
}

/** Set the UAVObject metadata telemetry update mode member
 * \param[in] metadata The metadata object
 * \param[in] val The telemetry update mode
 */
void UAVObject::SetFlightTelemetryUpdateMode(UAVObject::Metadata &metadata, UAVObject::UpdateMode val)
{
    SET_BITS(metadata.flags, UAVOBJ_FLT_TELEMETRY_UPDATE_MODE_SHIFT, val, UAVOBJ_UPDATE_MODE_MASK);
}

/** Get the UAVObject metadata GCS telemetry update mode
 * \param[in] metadata The metadata object
 * \return the GCS telemetry update mode
 */
UAVObject::UpdateMode UAVObject::GetGcsTelemetryUpdateMode(const UAVObject::Metadata &metadata)
{
    return UAVObject::UpdateMode((metadata.flags >> UAVOBJ_GCS_TELEMETRY_UPDATE_MODE_SHIFT) & UAVOBJ_UPDATE_MODE_MASK);
}

/** Set the UAVObject metadata GCS telemetry update mode member
 * \param[in] metadata The metadata object
 * \param[in] val The GCS telemetry update mode
 */
void UAVObject::SetGcsTelemetryUpdateMode(UAVObject::Metadata &metadata, UAVObject::UpdateMode val)
{
    SET_BITS(metadata.flags, UAVOBJ_GCS_TELEMETRY_UPDATE_MODE_SHIFT, val, UAVOBJ_UPDATE_MODE_MASK);
}

