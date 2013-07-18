/**
 ******************************************************************************
 * @file       telemetrymonitor.cpp
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

#ifndef TELEMETRYMONITOR_H
#define TELEMETRYMONITOR_H

#include "uavobjectmanager.h"
#include "gcstelemetrystats.h"
#include "flighttelemetrystats.h"
#include "systemstats.h"
#include "telemetry.h"

namespace openpilot
{

class TelemetryMonitor {

public:
	TelemetryMonitor(boost::asio::io_service &io, UAVObjectManager *objMngr, Telemetry *tel);
	~TelemetryMonitor();

	// signals:
	boost::signals2::signal<void(void)> connected;
	boost::signals2::signal<void(void)> disconnected;
	boost::signals2::signal<void(double txRate, double rxRate)> telemetryUpdated;

private:
	static const int STATS_UPDATE_PERIOD_MS  = 4000;
	static const int STATS_CONNECT_PERIOD_MS = 2000;
	static const int CONNECTION_TIMEOUT_MS   = 8000;

	boost::asio::io_service &io_service;
	UAVObjectManager *objMngr;
	Telemetry *tel;
	boost::queue<UAVObject *> queue;
	GCSTelemetryStats *gcsStatsObj;
	FlightTelemetryStats *flightStatsObj;
	boost::asio::deadline_timer statsTimer;
	boost::asio::deadline_timer connectionTimer;
	boost::recursive_mutex mutex;
	UAVObject *objPending;
	bool connectionTimeout;
	boost::posix_time::ptime start_time;
	boost::posix_time::time_duration stats_interval; // desired interval

	void startRetrievingObjects();
	void retrieveNextObject();
	void stopRetrievingObjects();

	// slots:
	void transactionCompleted(UAVObject *obj, bool success);
	void processStatsUpdates(boost::system::error_code error);
	void flightStatsUpdated(UAVObject *obj);
	void connectionTimeoutHandler(boost::system::error_code error);
};

} // namespace openpilot

#endif // TELEMETRYMONITOR_H
