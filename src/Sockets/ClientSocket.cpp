/*
 * CllientSocket.cpp
 *
 *  Created on: 12 okt. 2017
 *      Author: Acer
 */

#include "ClientSocket.h"

#include <ERRORS.h>
#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace Sockets {

ClientSocket::ClientSocket() {
	m_fpSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_fpSocket < 0)
		throw ERRORS::SOCKET_NOT_CREATED;
}

ClientSocket::~ClientSocket() {
	this->close();
}

void ClientSocket::connect(char *ipAddress, uint16_t port) {
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	if (inet_pton(AF_INET, ipAddress, &address.sin_addr.s_addr) <= 0) {
		throw ERRORS::PARSE_IPv4_ADDRES_FAILED;
	}

	if (::connect(m_fpSocket, (struct sockaddr *) &address, sizeof address)) {
		throw ERRORS::SOCKET_CONNECT_ERROR;
	}
}

void ClientSocket::send(const void *data, std::size_t size) {
	if (::send(m_fpSocket, data, size, 0) < 0) {
		throw ERRORS::SOCKET_SEND_ERROR;
	}
}

void ClientSocket::close() {
	if (m_fpSocket) {
		send("CLOSE", sizeof "CLOSE");
		::close(m_fpSocket);

		m_fpSocket = 0;
	}
}

} /* namespace Sockets */
