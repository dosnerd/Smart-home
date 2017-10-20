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
#include "Errors.h"
#include <sys/socket.h>
#include <netinet/in.h>

ServerSocket::ServerSocket() :
		onAccept(0) {
	m_fpServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_fpServerSocket < 0)
		throw ERRORS::SOCKET_NOT_CREATED;
}

ServerSocket::~ServerSocket() {
}

void ServerSocket::bind(uint16_t port) {
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if (::bind(m_fpServerSocket, (struct sockaddr *) &addr, sizeof addr))
		throw ERRORS::SOCKET_BIND_FAILED;
}

void ServerSocket::listen(uint16_t limit) {
	int newSocket;
	struct sockaddr_in addr;
	socklen_t addressLength = sizeof addr;

	if (::listen(m_fpServerSocket, limit) < 0)
		throw ERRORS::SOCKET_LISTEN_ERROR;

	while (1) {
		newSocket = ::accept(m_fpServerSocket, (struct sockaddr *) &addr,
				&addressLength);

		if (newSocket == -1)
			throw ERRORS::SOCKET_ACCEPT_REFUSED;

		onAccept(newSocket);
	}
}

void ServerSocket::setOnAccept(void (*onAccept)(int socket)) {
	this->onAccept = onAccept;
}
