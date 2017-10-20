/*
 * CllientSocket.cpp
 *
 *  Created on: 12 okt. 2017
 *      Author: Acer
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
