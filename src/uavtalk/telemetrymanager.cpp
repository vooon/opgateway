/**
 ******************************************************************************
 *
 * @file       telemetrymanager.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup UAVTalkPlugin UAVTalk Plugin
 * @{
 * @brief The UAVTalk protocol plugin
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

#include "telemetrymanager.h"

using namespace openpilot;

TelemetryManager::TelemetryManager(UAVObjectManager *objMngr_) :
	autopilotConnected(false),
	objMngr(objMngr_)
{
	// run io_service for uavtalk && telemetry timers
	io_service_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, &this->io_service));
}

TelemetryManager::~TelemetryManager()
{
	io_service.stop();
}

bool TelemetryManager::isConnected()
{
	return autopilotConnected;
}

void TelemetryManager::start(UAVTalkIOBase *dev)
{
	device = dev;
	onStart();
}

void TelemetryManager::onStart()
{
	utalk        = new UAVTalk(device, objMngr);
	telemetry    = new Telemetry(io_service, utalk, objMngr);
	telemetryMon = new TelemetryMonitor(io_service, objMngr, telemetry);

	telemetryMon->connected.connect(boost::bind(&TelemetryManager::onConnect, this));
	telemetryMon->disconnected.connect(boost::bind(&TelemetryManager::onDisconnect, this));
}


void TelemetryManager::stop()
{
	onStop();
}

void TelemetryManager::onStop()
{
	telemetryMon->connected.disconnect(boost::bind(&TelemetryManager::onConnect, this));
	telemetryMon->disconnected.disconnect(boost::bind(&TelemetryManager::onDisconnect, this));

	delete telemetryMon;
	delete telemetry;
	delete utalk;

	onDisconnect();
}

void TelemetryManager::onConnect()
{
	autopilotConnected = true;
	connected(); // emit signal
}

void TelemetryManager::onDisconnect()
{
	autopilotConnected = false;
	disconnected(); // emit signal
}

