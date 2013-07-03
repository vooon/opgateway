/**
 ******************************************************************************
 * @file       uavobject.h
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

#ifndef UAVOBJECT_H
#define UAVOBJECT_H

#include <string>
#include <sstream>
#include <boost/signals2.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>

namespace openpilot
{

#define UAVOBJ_FLT_ACCESS_SHIFT				0
#define UAVOBJ_GCS_ACCESS_SHIFT				1
#define UAVOBJ_FLT_TELEMETRY_ACKED_SHIFT		2
#define UAVOBJ_GCS_TELEMETRY_ACKED_SHIFT		3
#define UAVOBJ_FLT_TELEMETRY_UPDATE_MODE_SHIFT		4
#define UAVOBJ_GCS_TELEMETRY_UPDATE_MODE_SHIFT		6
#define UAVOBJ_UPDATE_MODE_MASK				0x3


class UAVObject {
public:

	/** Object update mode
	 */
	typedef enum {
		UPDATEMODE_MANUAL    = 0,	/** Manually update object, by calling the updated() function */
		UPDATEMODE_PERIODIC  = 1,	/** Automatically update object at periodic intervals */
		UPDATEMODE_ONCHANGE  = 2,	/** Only update object when its data changes */
		UPDATEMODE_THROTTLED = 3	/** Object is updated on change, but not more often than the interval time */
	} UpdateMode;

	/** Access mode
	 */
	typedef enum {
		ACCESS_READWRITE = 0,
		ACCESS_READONLY  = 1
	} AccessMode;

	/**
	 * Object metadata, each object has a meta object that holds its metadata. The metadata define
	 * properties for each object and can be used by multiple modules (e.g. telemetry and logger)
	 *
	 * The object metadata flags are packed into a single 16 bit integer.
	 * The bits in the flag field are defined as:
	 *
	 *   Bit(s)  Name                       Meaning
	 *   ------  ----                       -------
	 *      0    access                     Defines the access level for the local transactions (readonly=0 and readwrite=1)
	 *      1    gcsAccess                  Defines the access level for the local GCS transactions (readonly=0 and readwrite=1), not used in the flight s/w
	 *      2    telemetryAcked             Defines if an ack is required for the transactions of this object (1:acked, 0:not acked)
	 *      3    gcsTelemetryAcked          Defines if an ack is required for the transactions of this object (1:acked, 0:not acked)
	 *    4-5    telemetryUpdateMode        Update mode used by the telemetry module (UAVObjUpdateMode)
	 *    6-7    gcsTelemetryUpdateMode     Update mode used by the GCS (UAVObjUpdateMode)
	 */
	typedef struct {
		uint8_t flags; /** Defines flags for update and logging modes and whether an update should be ACK'd (bits defined above) */
		uint16_t flightTelemetryUpdatePeriod; /** Update period used by the telemetry module (only if telemetry mode is PERIODIC) */
		uint16_t gcsTelemetryUpdatePeriod; /** Update period used by the GCS (only if telemetry mode is PERIODIC) */
		uint16_t loggingUpdatePeriod; /** Update period used by the logging module (only if logging mode is PERIODIC) */
	} __attribute__((packed)) Metadata;


	UAVObject(uint32_t objID, bool isSingleInstance, const std::string &name);
	void initialize(uint32_t instID);
	uint32_t getObjID();
	uint32_t getInstID();
	bool isSingleInstance();
	std::string getName();
	size_t getNumBytes();
	virtual ssize_t serialize(uint8_t *dataOut) = 0;
	virtual ssize_t deserialize(const uint8_t *dataIn) = 0;
	//virtual bool save() = 0;
	//virtual bool load() = 0;
	virtual void setMetadata(const Metadata &mdata) = 0;
	virtual Metadata getMetadata() = 0;
	virtual Metadata getDefaultMetadata() = 0;
	//void lock();
	//bool lock(int timeoutMs);
	//void unlock();
	//boost::recursive_timed_mutex *getMutex();

	std::string toString();
	virtual std::string toStringData() = 0;

	// Metadata accessors
	static void MetadataInitialize(Metadata &meta);
	static AccessMode GetFlightAccess(const Metadata &meta);
	static void SetFlightAccess(Metadata &meta, AccessMode mode);
	static AccessMode GetGcsAccess(const Metadata &meta);
	static void SetGcsAccess(Metadata &meta, AccessMode mode);
	static bool GetFlightTelemetryAcked(const Metadata &meta);
	static void SetFlightTelemetryAcked(Metadata &meta, bool val);
	static bool GetGcsTelemetryAcked(const Metadata &meta);
	static void SetGcsTelemetryAcked(Metadata &meta, bool val);
	static UpdateMode GetFlightTelemetryUpdateMode(const Metadata &meta);
	static void SetFlightTelemetryUpdateMode(Metadata &meta, UpdateMode val);
	static UpdateMode GetGcsTelemetryUpdateMode(const Metadata &meta);
	static void SetGcsTelemetryUpdateMode(Metadata &meta, UpdateMode val);

	// slots
	void requestUpdate();
	void updated();

	boost::signals2::signal<void(UAVObject *)> objectUpdated;
	boost::signals2::signal<void(UAVObject *)> objectUpdatedAuto;
	boost::signals2::signal<void(UAVObject *)> objectUpdatedManual;
	boost::signals2::signal<void(UAVObject *)> objectUpdatedPeriodic;
	boost::signals2::signal<void(UAVObject *)> updateRequested;
	boost::signals2::signal<void(UAVObject *)> transactionCompleted;
	boost::signals2::signal<void(UAVObject *)> newInstance;

protected:
	uint32_t objID;
	uint32_t instID;
	bool isSingleInst;
	std::string name;
	boost::recursive_timed_mutex mutex;
	uint8_t *data;
	size_t numBytes;
};

} // namespace openpilot

#endif // UAVOBJECT_H
