/*
 * ServerSocket.cpp
 *
 *  Created on: 11 okt. 2017
 *      Author: Acer
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
