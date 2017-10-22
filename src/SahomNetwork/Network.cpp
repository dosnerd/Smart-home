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

#include <Sockets/ClientSocket.h>
#include <Sockets/Multicast.h>
#include <Errors.h>
#include <cstring>
#include <sys/socket.h>
#include <SahomNetwork/Network.h>

#include <iostream>

namespace SahomNetwork {

Network::Network() {

}

Network::~Network() {

}

void Network::requestSignIn() {
	struct SahomNetwork::CommonHeader header;
	struct SahomNetwork::StandardMessage message;

	CreateMessage((Message *)&message, sizeof(*message.structure));
	message.structure->command = STANDARD_MESSAGE_COMMAND_HELLO;
	message.structure->nParameters = 0;

	InitHeader(&header, message.raw, message.rawSize);
	header.structure->type = MESSAGE_TYPE_STANDARD;
	freeMessage((Message *) &message);

	multicast(header, MULTICAST_SIGN_IN_CHANNEL);
	flush();
}

void Network::multicast(struct CommonHeader message, DESTINATION destination) {
	SendRequest *request = new SendRequest;

	request->destination = destination;
	request->message.raw = message.raw;
	request->message.rawSize = message.rawSize;
	request->message.structure->version = SAHOM_VERSION;

	m_multicastBuffer.push(request);
}

void Network::InitHeader(struct CommonHeader* header, const void* payload, uint16_t size) {
	header->rawSize = sizeof(*header->structure) + size;
	header->raw = new uint8_t[header->rawSize];

	std::memcpy(header->raw, m_networkName, sizeof m_networkName);
	header->structure->version = SAHOM_VERSION;
	header->structure->size = size;
	std::memcpy(header->raw + sizeof(*header->structure), payload, size);
}

void Network::CreateMessage(struct Message *message, uint16_t totalSize) {
	message->rawSize = totalSize;
	message->raw = new uint8_t[message->rawSize];
}

void Network::freeHeader(struct CommonHeader *header) {
	delete header->raw;
}

void Network::freeMessage(struct Message *message) {
	delete message->raw;
}

void Network::flush() {
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
		socket.sendtoAddress(request->message.raw, request->message.rawSize);
		freeHeader(&request->message);
		delete request;
	}
	(void) request;
	(void) addr;
}

} /* namespace SahomNetwork */