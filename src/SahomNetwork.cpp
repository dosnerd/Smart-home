/*
 * SahomNetwork.cpp
 *
 *  Created on: 16 okt. 2017
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

#include <SahomNetwork.h>
#include <Sockets/ClientSocket.h>
#include <Errors.h>
#include <cstring>
#include <sys/socket.h>

#include <iostream>

SahomNetwork::SahomNetwork() {

}

SahomNetwork::~SahomNetwork() {

}

void SahomNetwork::multicast(struct Message message, DESTINATION destination) {
	SendRequest *request = new SendRequest;

	request->destination = destination;
	request->message.raw = message.raw;
	request->message.nMessage = message.nMessage;
	request->message.structure->version = SAHOM_VERSION;

	m_multicastBuffer.push(request);
}

void SahomNetwork::createMessage(struct Message* message, const void* payload,
		uint16_t size)
{
	message->nMessage = sizeof(*message->structure) + size;
	message->raw = new uint8_t[message->nMessage];

	std::memcpy(message->raw, m_networkName, sizeof m_networkName);
	message->structure->version = SAHOM_VERSION;
	message->structure->size = size;
	std::memcpy(message->raw + sizeof(*message->structure), payload, size);
}

void SahomNetwork::freeMessage(struct Message* message) {
	delete message->raw;
}

void SahomNetwork::flush() {
	Sockets::ClientSocket socket(SOCK_DGRAM);
	SendRequest *request;
	sockaddr_in6 addr = { 0 };

	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(SAHOM_PORT);

	while (!m_multicastBuffer.empty()) {
		request = m_multicastBuffer.front();
		m_multicastBuffer.pop();

		switch (request->destination) {
		case DESTINATION::MULTICAST_SIGN_IN_CHANNEL:
			addr.sin6_addr = SIGN_IN_CHANNEL;
			break;
			default:
			continue;
		}

		socket.setAddress(addr);
		socket.sendtoAddress(request->message.raw, request->message.nMessage);
		freeMessage(&request->message);
		delete request;
	}
	(void)request;
	(void)addr;
}
