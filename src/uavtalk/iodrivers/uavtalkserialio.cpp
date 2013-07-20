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
#include "ros/console.h"

using namespace openpilot;

UAVTalkSerialIO::UAVTalkSerialIO(std::string device, unsigned int baudrate) :
	io_service(),
	serial_dev(io_service, device)
{
	serial_dev.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
	serial_dev.set_option(boost::asio::serial_port_base::character_size(8));
	serial_dev.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
	serial_dev.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
	serial_dev.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

	// give some work to io_service before start
	io_service.post(boost::bind(&UAVTalkSerialIO::do_read, this));

	// run io_service for async io
	boost::thread t(boost::bind(&boost::asio::io_service::run, &this->io_service));
	io_thread.swap(t);
}

UAVTalkSerialIO::~UAVTalkSerialIO()
{
	io_service.stop();
}

void UAVTalkSerialIO::write(const uint8_t *data, size_t length)
{
	{
		boost::recursive_mutex::scoped_lock lock(mutex);
		tx_q.insert(tx_q.end(), data, data + length);
	}
	io_service.post(boost::bind(&UAVTalkSerialIO::do_write, this));
}

void UAVTalkSerialIO::do_read(void)
{
	serial_dev.async_read_some(
			boost::asio::buffer(rx_buf, sizeof(rx_buf)),
			boost::bind(&UAVTalkSerialIO::async_read_end,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
}

void UAVTalkSerialIO::async_read_end(boost::system::error_code error, size_t bytes_transfered)
{
	if (error) {
		if (serial_dev.is_open()) {
			serial_dev.close();
			sig_closed();
			ROS_DEBUG_NAMED("UAVTalk", "async_read_end: error! port closed.");
		}
	} else {
		sig_read(rx_buf, bytes_transfered);
		do_read();
	}
}

void UAVTalkSerialIO::do_write(void)
{
	// if write not in progress
	if (tx_buf == 0) {
		boost::recursive_mutex::scoped_lock lock(mutex);

		tx_buf_size = tx_q.size();
		tx_buf.reset(new uint8_t[tx_buf_size]);
		std::copy(tx_q.begin(), tx_q.end(), tx_buf.get());
		tx_q.clear();

		boost::asio::async_write(serial_dev,
				boost::asio::buffer(tx_buf.get(), tx_buf_size),
				boost::bind(&UAVTalkSerialIO::async_write_end,
					this,
					boost::asio::placeholders::error));
	}
}

void UAVTalkSerialIO::async_write_end(boost::system::error_code error)
{
	if (!error) {
		boost::recursive_mutex::scoped_lock lock(mutex);

		if (tx_q.empty()) {
			tx_buf.reset();
			tx_buf_size = 0;
			return;
		}

		tx_buf_size = tx_q.size();
		tx_buf.reset(new uint8_t[tx_buf_size]);
		std::copy(tx_q.begin(), tx_q.end(), tx_buf.get());
		tx_q.clear();

		boost::asio::async_write(serial_dev,
				boost::asio::buffer(tx_buf.get(), tx_buf_size),
				boost::bind(&UAVTalkSerialIO::async_write_end,
					this,
					boost::asio::placeholders::error));
	} else {
		if (serial_dev.is_open()) {
			serial_dev.close();
			sig_closed();
			ROS_DEBUG_NAMED("UAVTalk", "async_write_end: error! port closed.");
		}
	}
}

