/**
 ******************************************************************************
 * @file       uavtalkserialio.cpp
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

#include "uavtalkserialio.h"

using namespace openpilot;

UAVTalkSerialIO::UAVTalkSerialIO(std::string device, unsigned int baudrate) :
	io_service(),
	serial_dev(io_service, device)
{
	// run io_service for async io
	io_service_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, &this->io_service));

	serial_dev.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
	serial_dev.set_option(boost::asio::serial_port_base::character_size(8));
	serial_dev.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
	serial_dev.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
	serial_dev.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

	/* start recv */
	if (is_open()) {
		serial_dev.async_read_some(
				boost::asio::buffer(rx_buf, sizeof(rx_buf)),
				boost::bind(&UAVTalkSerialIO::async_read_end,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
	}
}

void UAVTalkSerialIO::write(const uint8_t *data, size_t length)
{
	boost::asio::async_write(serial_dev,
			boost::asio::buffer(data, length),
			boost::bind(&UAVTalkSerialIO::async_write_end,
				this,
				boost::asio::placeholders::error));
}

void UAVTalkSerialIO::async_read_end(boost::system::error_code error, size_t bytes_transfered)
{
	if (error)
		return;

	sig_read(rx_buf, bytes_transfered);

	serial_dev.async_read_some(
			boost::asio::buffer(rx_buf, sizeof(rx_buf)),
			boost::bind(&UAVTalkSerialIO::async_read_end,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
}

void UAVTalkSerialIO::async_write_end(boost::system::error_code error)
{
	if (!error) { } // TODO
}

