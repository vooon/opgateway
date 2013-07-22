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
#ifndef UAVTALKRELAY_H
#define UAVTALKRELAY_H

#include "uavtalk.h"

namespace openpilot
{

class UAVTalkRelay : public UAVTalk {
public:
	UAVTalkRelay(UAVTalkIOBase *iodev, UAVObjectManager *objMngr);
	~UAVTalkRelay();

private:
	void sendObjectSlot(UAVObject *obj);
	bool receiveObject(uint8_t type, uint32_t objId, uint16_t instId, uint8_t *data, size_t length);
};

} // namespace openpilot

#endif // UAVTALKRELAY_H
