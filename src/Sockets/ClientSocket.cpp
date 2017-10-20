/*
 * CllientSocket.cpp
 *
 *  Created on: 12 okt. 2017
 *      Author: David de Prez
 *
 *
 * Copyright (C) 2017  David de Prez
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ClientSocket.h"

#include <ERRORS.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Sockets {

ClientSocket::ClientSocket() :
		ClientSocket(SOCK_STREAM)
{
}

ClientSocket::ClientSocket(int type) :
		m_address( { 0 })
{
	m_fpSocket = ::socket(AF_INET6, type, 0);
	if (m_fpSocket < 0)
		throw ERRORS::SOCKET_NOT_CREATED;
}

ClientSocket::~ClientSocket() {
	this->close();
}

void ClientSocket::connect(char *ipAddress, uint16_t port) {
	setAddress(ipAddress, port);

	if (::connect(m_fpSocket, (struct sockaddr *) &m_address,
			sizeof m_address)) {
		m_fpSocket = 0;
		throw ERRORS::SOCKET_CONNECT_ERROR;
	}
}

void ClientSocket::send(const void *data, std::size_t size) {
	if (::send(m_fpSocket, data, size, 0) < 0) {
		throw ERRORS::SOCKET_SEND_ERROR;
	}
}

void ClientSocket::setAddress(const char *ipAddress, uint16_t port) {
	m_address.sin6_family = AF_INET6;
	m_address.sin6_port = htons(port);

	if (inet_pton(AF_INET6, ipAddress, &m_address.sin6_addr) <= 0) {
		m_fpSocket = 0;
		throw ERRORS::PARSE_IP_ADDRES_FAILED;
	}
}

void ClientSocket::setAddress(sockaddr_in6 addr) {
	m_address = addr;
}


void ClientSocket::sendtoAddress(const void *data, std::size_t size) {
	if (::sendto(m_fpSocket, data, size, 0, (struct sockaddr *) &m_address,
			sizeof m_address) < 0) {
		throw ERRORS::SOCKET_SEND_ERROR;

	}
}

void ClientSocket::close() {
	if (m_fpSocket) {
		::close(m_fpSocket);

		m_fpSocket = 0;
	}
}

} /* namespace Sockets */
