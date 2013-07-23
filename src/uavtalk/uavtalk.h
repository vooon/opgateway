/**
 ******************************************************************************
 * @file       uavtalk.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @author     Vladimir Ermakov, Copyright (C) 2013.
 * @brief The UAVTalk protocol
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
#ifndef UAVTALK_H
#define UAVTALK_H

#include "uavobjectmanager.h"
#include "uavtalkiobase.h"

namespace openpilot
{

class UAVTalk {
public:
	typedef struct {
		uint32_t txBytes;
		uint32_t rxBytes;
		uint32_t txObjectBytes;
		uint32_t rxObjectBytes;
		uint32_t rxObjects;
		uint32_t txObjects;
		uint32_t txErrors;
		uint32_t rxErrors;
	} ComStats;

	UAVTalk(UAVTalkIOBase *iodev, UAVObjectManager *objMngr);
	~UAVTalk();
	bool sendObject(UAVObject *obj, bool acked, bool allInstances);
	bool sendObjectRequest(UAVObject *obj, bool allInstances);
	void cancelTransaction(UAVObject *obj);
	ComStats getStats();
	void resetStats();

	// signals:
	boost::signals2::signal<void(UAVObject *obj, bool success)> transactionCompleted;

private: // slots:
	void processInputStream(uint8_t *data, size_t lenght);

protected:
	typedef struct {
		UAVObject *obj;
		bool allInstances;
	} Transaction;

	// Constants
	static const int TYPE_MASK    = 0xF8;
	static const int TYPE_VER     = 0x20;
	static const int TYPE_OBJ     = (TYPE_VER | 0x00);
	static const int TYPE_OBJ_REQ = (TYPE_VER | 0x01);
	static const int TYPE_OBJ_ACK = (TYPE_VER | 0x02);
	static const int TYPE_ACK     = (TYPE_VER | 0x03);
	static const int TYPE_NACK    = (TYPE_VER | 0x04);

	static const int MIN_HEADER_LENGTH  = 8; // sync(1), type (1), size(2), object ID(4)
	static const int MAX_HEADER_LENGTH  = 10; // sync(1), type (1), size(2), object ID (4), instance ID(2, not used in single objects)

	static const int CHECKSUM_LENGTH    = 1;

	static const int MAX_PAYLOAD_LENGTH = 256;

	static const int MAX_PACKET_LENGTH  = (MAX_HEADER_LENGTH + MAX_PAYLOAD_LENGTH + CHECKSUM_LENGTH);

	static const uint16_t ALL_INSTANCES  = 0xFFFF;
	static const uint16_t OBJID_NOTFOUND = 0x0000;

	static const int TX_BUFFER_SIZE     = 2 * 1024;
	static const uint8_t crc_table[256];

	// Types
	typedef enum { STATE_SYNC, STATE_TYPE, STATE_SIZE, STATE_OBJID, STATE_INSTID, STATE_DATA, STATE_CS } RxStateType;

	// Variables
	UAVTalkIOBase *io;
	UAVObjectManager *objMngr;
	boost::recursive_mutex mutex;
	std::map<uint32_t, Transaction *> transMap;
	uint8_t rxBuffer[MAX_PACKET_LENGTH];
	uint8_t txBuffer[MAX_PACKET_LENGTH];
	// Variables used by the receive state machine
	uint8_t  rxTmpBuffer[4];
	uint8_t  rxType;
	uint32_t rxObjId;
	uint16_t rxInstId;
	uint16_t rxLength;
	uint16_t rxPacketLength;
	uint8_t  rxInstanceLength;

	uint8_t rxCSPacket, rxCS;
	int32_t rxCount;
	int32_t packetSize;
	RxStateType rxState;
	ComStats stats;

	// Methods
	bool objectTransaction(UAVObject *obj, uint8_t type, bool allInstances);
	bool processInputByte(uint8_t rxbyte);
	virtual bool receiveObject(uint8_t type, uint32_t objId, uint16_t instId, uint8_t *data, size_t length);
	UAVObject *updateObject(uint32_t objId, uint16_t instId, uint8_t *data);
	void updateAck(UAVObject *obj);
	void updateNack(UAVObject *obj);
	bool transmitNack(uint32_t objId);
	bool transmitObject(UAVObject *obj, uint8_t type, bool allInstances);
	bool transmitSingleObject(UAVObject *obj, uint8_t type, bool allInstances);
	uint8_t updateCRC(uint8_t crc, const uint8_t data);
	uint8_t updateCRC(uint8_t crc, const uint8_t *data, size_t length);
};

} // namespace openpilot

#endif // UAVTALK_H
