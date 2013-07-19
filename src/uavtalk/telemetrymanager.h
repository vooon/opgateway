/**
 ******************************************************************************
 * @file       telemetrymanager.cpp
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

#ifndef TELEMETRYMANAGER_H
#define TELEMETRYMANAGER_H

#include <boost/thread/thread.hpp>
#include "telemetrymonitor.h"
#include "telemetry.h"
#include "uavtalk.h"
#include "uavobjectmanager.h"

namespace openpilot
{

class TelemetryManager {
public:
	TelemetryManager(UAVObjectManager *objMngr);
	~TelemetryManager();

	void start(UAVTalkIOBase *dev);
	void stop();
	bool isConnected();

	// signals:
	boost::signals2::signal<void(void)> connected;
	boost::signals2::signal<void(void)> disconnected;
	//void myStart();
	//void myStop();

private: // slots:
	void onConnect();
	void onDisconnect();
	void onStart();
	void onStop();

private:
	boost::asio::io_service io_service;
	boost::thread io_thread;
	UAVObjectManager *objMngr;
	UAVTalk *utalk;
	Telemetry *telemetry;
	TelemetryMonitor *telemetryMon;
	UAVTalkIOBase *device;
	bool autopilotConnected;
};

} // namespace openpilot

#endif // TELEMETRYMANAGER_H
