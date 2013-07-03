/**
 ******************************************************************************
 * @file       uavdataobject.h
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
#ifndef UAVDATAOBJECT_H
#define UAVDATAOBJECT_H

#include "uavobject.h"
#include "uavmetaobject.h"

namespace openpilot
{

class UAVDataObject : public UAVObject {
public:
	UAVDataObject(uint32_t objID, bool isSingleInst, bool isSet, const std::string &name);
	void initialize(uint32_t instID, UAVMetaObject *mobj);
	void initialize(UAVMetaObject *mobj);
	bool isSettings();
	void setMetadata(const Metadata &mdata);
	Metadata getMetadata();
	UAVMetaObject *getMetaObject();
	virtual UAVDataObject *clone(uint32_t instID = 0) = 0;
	virtual UAVDataObject *dirtyClone() = 0;

	//ssize_t serialize(uint8_t *dataOut);
	//ssize_t deserialize(uint8_t *dataIn);

private:
	UAVMetaObject *mobj;
	bool isSet;
};

} // namespace openpilot

#endif // UAVDATAOBJECT_H
