/*
 * ServerSocket.cpp
 *
 *  Created on: 11 okt. 2017
 *      Author: Author: David de Prez
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

#include "ServerSocket.h"
#include <Errors.h>
#include <Log.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <arpa/inet.h>

namespace Sockets {

ServerSocket::ServerSocket() :
		ServerSocket(SOCK_STREAM)
{

}

ServerSocket::ServerSocket(int socketType) :
		m_onAccept(0)
{
	m_fpSocket = ::socket(AF_INET6, socketType, 0);
	if (m_fpSocket < 0)
		throw ERRORS::SOCKET_NOT_CREATED;
}

ServerSocket::~ServerSocket() {
	this->close();
}

void ServerSocket::bind(uint16_t port) {
	struct sockaddr_in6 addr = { AF_INET6, htons(port) };
	int reuseValua = 1;
	if (::setsockopt(m_fpSocket, SOL_SOCKET, SO_REUSEADDR, &reuseValua,
			sizeof reuseValua) < 0) {
		throw ERRORS::SOCKET_SET_REUSE_FAILED;
	}

	if (::bind(m_fpSocket, (struct sockaddr *) &addr, sizeof addr))
		throw ERRORS::SOCKET_BIND_FAILED;
}

void ServerSocket::listen(uint16_t limit) {
	int newSocket;
	struct sockaddr_in6 addr = { 0 };
	char buff[2048];
	socklen_t addressLength = sizeof addr;

	if (::listen(m_fpSocket, limit) < 0)
		throw ERRORS::SOCKET_LISTEN_ERROR;

	while (1) {
		newSocket = ::accept(m_fpSocket, (struct sockaddr *) &addr,
				&addressLength);

		if (newSocket == -1)
			throw ERRORS::SOCKET_ACCEPT_REFUSED;

		if (inet_ntop(AF_INET6, &addr.sin6_addr, buff, 2048) <= 0) {
			ERROR("SERVER SOCKET", "Can not format IPv6 address");
		} else {
			LOG("SERVER SOCKET", "FROM: " << buff);
		}

		m_onAccept(newSocket);
		::close(newSocket);
	}
}

void ServerSocket::close() {
	if (m_fpSocket) {
		::close(m_fpSocket);

		m_fpSocket = 0;
	}
}

} /* namespace Sockets */
