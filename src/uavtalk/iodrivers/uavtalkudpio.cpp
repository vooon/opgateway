/**
 ******************************************************************************
 * @file       uavtalkudpio.cpp
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

#include "uavtalkudpio.h"
#include "ros/console.h"

using namespace openpilot;

UAVTalkUDPIO::UAVTalkUDPIO(std::string server_addr, unsigned int server_port) :
	io_service(),
	socket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), server_port))
{
	// give some work to io_service before start
	io_service.post(boost::bind(&UAVTalkUDPIO::do_read, this));

	// run io_service for async io
	boost::thread t(boost::bind(&boost::asio::io_service::run, &this->io_service));
	io_thread.swap(t);
}

UAVTalkUDPIO::~UAVTalkUDPIO()
{
	io_service.stop();
}

void UAVTalkUDPIO::write(const uint8_t *data, size_t length)
{
	{
		boost::recursive_mutex::scoped_lock lock(mutex);
		tx_q.insert(tx_q.end(), data, data + length);
	}
	io_service.post(boost::bind(&UAVTalkUDPIO::do_write, this));
}

void UAVTalkUDPIO::do_read(void)
{
	socket.async_receive_from(
			boost::asio::buffer(rx_buf, sizeof(rx_buf)),
			sender_endpoint,
			boost::bind(&UAVTalkUDPIO::async_read_end,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
}

void UAVTalkUDPIO::async_read_end(boost::system::error_code error, size_t bytes_transfered)
{
	if (error) {
		if (socket.is_open()) {
			socket.close();
			sig_closed();
			ROS_DEBUG_NAMED("UAVTalk", "async_read_end:udp: error! port closed.");
		}
	} else {
		sig_read(rx_buf, bytes_transfered);
		do_read();
	}
}

void UAVTalkUDPIO::do_write(void)
{
	// if write not in progress
	if (tx_buf == 0) {
		boost::recursive_mutex::scoped_lock lock(mutex);

		tx_buf_size = tx_q.size();
		tx_buf.reset(new uint8_t[tx_buf_size]);
		std::copy(tx_q.begin(), tx_q.end(), tx_buf.get());
		tx_q.clear();

		socket.async_send_to(
				boost::asio::buffer(tx_buf.get(), tx_buf_size),
				sender_endpoint,
				boost::bind(&UAVTalkUDPIO::async_write_end,
					this,
					boost::asio::placeholders::error));
	}
}

void UAVTalkUDPIO::async_write_end(boost::system::error_code error)
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

		socket.async_send_to(
				boost::asio::buffer(tx_buf.get(), tx_buf_size),
				sender_endpoint,
				boost::bind(&UAVTalkUDPIO::async_write_end,
					this,
					boost::asio::placeholders::error));
	} else {
		if (socket.is_open()) {
			socket.close();
			sig_closed();
			ROS_DEBUG_NAMED("UAVTalk", "async_write_end:udp: error! port closed.");
		}
	}
}

