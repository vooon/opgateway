/**
 ******************************************************************************
 *
 * @file       uavtalkrelay.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @addtogroup GCSPlugins GCS Plugins
 * @brief The UAVTalk protocol relay
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
#include <ros/console.h>
#include "uavtalkrelay.h"
#include "gcstelemetrystats.h"

using namespace openpilot;

/** Constructor
 */
UAVTalkRelay::UAVTalkRelay(UAVTalkIOBase *iodev, UAVObjectManager *objMngr) :
	UAVTalk(iodev, objMngr)
{
	UAVObjectManager::objects_map uavos = objMngr->getObjects();
	for (UAVObjectManager::objects_map::iterator it = uavos.begin(); it != uavos.end(); ++it) {
		for (UAVObjectManager::inst_vec::iterator inst_it = it->second.begin(); inst_it != it->second.end(); ++inst_it) {
			(*inst_it)->objectUpdated.connect(boost::bind(&UAVTalkRelay::sendObjectSlot, this, _1));
		}
	}
}

UAVTalkRelay::~UAVTalkRelay()
{
	UAVObjectManager::objects_map uavos = objMngr->getObjects();
	for (UAVObjectManager::objects_map::iterator it = uavos.begin(); it != uavos.end(); ++it) {
		for (UAVObjectManager::inst_vec::iterator inst_it = it->second.begin(); inst_it != it->second.end(); ++inst_it) {
			(*inst_it)->objectUpdated.disconnect(boost::bind(&UAVTalkRelay::sendObjectSlot, this, _1));
		}
	}
}

void UAVTalkRelay::sendObjectSlot(UAVObject *obj)
{
	// Do not forward GCSTelemetryStats
	if (obj->getObjID() == GCSTelemetryStats::OBJID)
		return;

	sendObject(obj, false, false);
}

/** Receive an object. This function process objects received through the telemetry stream.
 * \param[in] type Type of received message (TYPE_OBJ, TYPE_OBJ_REQ, TYPE_OBJ_ACK, TYPE_ACK, TYPE_NACK)
 * \param[in] obj Handle of the received object
 * \param[in] instId The instance ID of UAVOBJ_ALL_INSTANCES for all instances.
 * \param[in] data Data buffer
 * \param[in] length Buffer length
 * \return Success (true), Failure (false)
 */
bool UAVTalkRelay::receiveObject(uint8_t type, uint32_t objId, uint16_t instId, uint8_t *data, size_t length)
{
	//UNUSED(length);
	UAVObject *obj    = NULL;
	bool error        = false;
	bool allInstances = (instId == ALL_INSTANCES);

	// Do not forward GCSTelemetryStats
	if (objId == GCSTelemetryStats::OBJID)
		return false;

	// Process message type
	switch (type) {
	case TYPE_OBJ:
		// All instances, not allowed for OBJ messages
		if (!allInstances) {
			UAVObject *tobj = objMngr->getObject(objId);
			if (tobj != NULL)
				tobj->objectUpdated.disconnect(boost::bind(&UAVTalkRelay::sendObjectSlot, this, _1));

			// Get object and update its data
			obj = updateObject(objId, instId, data);

			if (tobj != NULL)
				tobj->objectUpdated.connect(boost::bind(&UAVTalkRelay::sendObjectSlot, this, _1));
			if (dynamic_cast<UAVMetaObject *>(tobj) != NULL)
				tobj->updated();

			// Check if an ack is pending
			if (obj != NULL) {
				updateAck(obj);
			} else {
				error = true;
			}
		} else {
			error = true;
		}
		break;

	case TYPE_OBJ_ACK:
		// All instances, not allowed for OBJ_ACK messages
		if (!allInstances) {
			UAVObject *tobj = objMngr->getObject(objId);
			if (tobj != NULL)
				tobj->objectUpdated.disconnect(boost::bind(&UAVTalkRelay::sendObjectSlot, this, _1));

			// Get object and update its data
			obj = updateObject(objId, instId, data);

			if (tobj != NULL)
				tobj->objectUpdated.connect(boost::bind(&UAVTalkRelay::sendObjectSlot, this, _1));
			if (dynamic_cast<UAVMetaObject *>(tobj) != NULL)
				tobj->updated();

			// Transmit ACK
			if (obj != NULL) {
				transmitObject(obj, TYPE_ACK, false);
			} else {
				error = true;
			}
		} else {
			error = true;
		}
		break;

	case TYPE_OBJ_REQ:
		// Get object, if all instances are requested get instance 0 of the object
		if (allInstances) {
			obj = objMngr->getObject(objId);
		} else {
			obj = objMngr->getObject(objId, instId);
		}
		// If object was found transmit it
		if (obj != NULL) {
			transmitObject(obj, TYPE_OBJ, allInstances);
		} else {
			// Object was not found, transmit a NACK with the
			// objId which was not found.
			transmitNack(objId);
			error = true;
		}
		break;

	case TYPE_NACK:
		// All instances, not allowed for NACK messages
		if (!allInstances) {
			// Get object
			obj = objMngr->getObject(objId, instId);
			// Check if object exists:
			if (obj != NULL) {
				updateNack(obj);
			} else {
				error = true;
			}
		}
		break;

	case TYPE_ACK:
		// All instances, not allowed for ACK messages
		if (!allInstances) {
			// Get object
			obj = objMngr->getObject(objId, instId);
			// Check if an ack is pending
			if (obj != NULL) {
				updateAck(obj);
			} else {
				error = true;
			}
		}
		break;

	default:
		error = true;
	}

	// Done
	return !error;
}

