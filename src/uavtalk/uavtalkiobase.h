/**
 ******************************************************************************
 * @file       uavtalkiobase.h
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
#ifndef UAVTALKIOBASE_H
#define UAVTALKIOBASE_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <boost/thread/thread.hpp>

namespace openpilot
{

class UAVTalkIOBase
{
public:
	UAVTalkIOBase();
	~UAVTalkIOBase();

	boost::signals2::signal<void(uint8_t *data, size_t lenght)> sig_read;
	boost::signals2::signal<void()> sig_closed;

	virtual void write(const uint8_t *data, size_t length) = 0;
	//ssize_t read(uint8_t *data, size_t length);
	//size_t available();
	virtual bool is_open() = 0;

private:
};

} // namespace openpilot

#endif // UAVTALKIOBASE_H

