/*****************************************************************************
 * PokerTH - The open source texas holdem engine                             *
 * Copyright (C) 2006-2011 Felix Hammer, Florian Thauer, Lothar May          *
 *                                                                           *
 * This program is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU Affero General Public License as            *
 * published by the Free Software Foundation, either version 3 of the        *
 * License, or (at your option) any later version.                           *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU Affero General Public License for more details.                       *
 *                                                                           *
 * You should have received a copy of the GNU Affero General Public License  *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 *****************************************************************************/

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <net/receivebuffer.h>
#include <net/sessiondata.h>
#include <core/loghelper.h>
#include <boost/swap.hpp>

using namespace std;


ReceiveBuffer::ReceiveBuffer()
	: recvBufUsed(0)
{
	recvBuf[0] = 0;
}

void
ReceiveBuffer::StartAsyncRead(boost::shared_ptr<SessionData> session)
{
	session->GetAsioSocket()->async_read_some(
		boost::asio::buffer(recvBuf + recvBufUsed, RECV_BUF_SIZE - recvBufUsed),
		boost::bind(
			&ReceiveBuffer::HandleRead,
			shared_from_this(),
			session,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void
ReceiveBuffer::HandleRead(boost::shared_ptr<SessionData> session, const boost::system::error_code &error, size_t bytesRead)
{
	if (error != boost::asio::error::operation_aborted) {
		try {
			if (!error) {
				recvBufUsed += bytesRead;
				ScanPackets();
				ProcessPackets(session);
				StartAsyncRead(session);
			} else if (error == boost::asio::error::interrupted || error == boost::asio::error::try_again) {
				LOG_ERROR("Session " << session->GetId() << " - recv interrupted: " << error);
				StartAsyncRead(session);
			} else {
				LOG_ERROR("Session " << session->GetId() << " - Connection closed: " << error);
				// On error: Close this session.
				session->Close();
			}
		} catch (const exception &e) {
			LOG_ERROR("Session " << session->GetId() << " - unhandled exception in HandleRead: " << e.what());
			throw;
		}
	}
}

void
ReceiveBuffer::ScanPackets()
{
	bool dataAvailable = true;
	do {
		boost::shared_ptr<NetPacket> tmpPacket;
		// This is necessary, because we use TCP.
		// Packets may be received in multiple chunks or
		// several packets may be received at once.
		if (recvBufUsed >= NET_HEADER_SIZE) {
			// Read the size of the packet (first 4 bytes in network byte order).
			size_t packetSize = ntohl(*((uint32_t *)&recvBuf[0]));
			if (recvBufUsed >= packetSize + NET_HEADER_SIZE)
			{
				try {
					tmpPacket = NetPacket::Create(&recvBuf[NET_HEADER_SIZE], packetSize);
					if (tmpPacket) {
						recvBufUsed -= (packetSize + NET_HEADER_SIZE);
						if (recvBufUsed) {
							memmove(recvBuf, recvBuf + packetSize + NET_HEADER_SIZE, recvBufUsed);
						}
					}
				} catch (const exception &e) {
					// Reset buffer on error.
					recvBufUsed = 0;
					LOG_ERROR(e.what());
				}
			}
		}
		if (tmpPacket) {
			// TODO check constraints.
			receivedPackets.push_back(tmpPacket);
		} else
			dataAvailable = false;
	} while(dataAvailable);
}

void
ReceiveBuffer::ProcessPackets(boost::shared_ptr<SessionData> session)
{
	while (!receivedPackets.empty()) {
		boost::shared_ptr<NetPacket> p = receivedPackets.front();
		receivedPackets.pop_front();
		session->HandlePacket(p);
	}
	if (recvBufUsed >= RECV_BUF_SIZE) {
		LOG_ERROR("Session " << session->GetId() << " - Receive buf full: " << recvBufUsed);
		recvBufUsed = 0;
	}
}

