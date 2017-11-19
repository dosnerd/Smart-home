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
#include <SahomNetwork/Network.h>
#include <SahomNetwork/Handlers.h>
#include <Errors.h>
#include <config.h>
#include <Log.h>

#include <cstring>
#include <sys/socket.h>

namespace SahomNetwork {

Network *Network::m_sInstance = nullptr;

Network* Network::getInstance() {
	if (m_sInstance == nullptr) {
		m_sInstance = new Network;
	}

	return m_sInstance;
}

void Network::destroyInstance() {
	delete m_sInstance;
	m_sInstance = nullptr;
}

Network::Network() :
		m_settings(0),
		m_multicastBuffer(),
		m_multicastListener(),
		m_handlers()
{
	m_multicastListener.setOnAccept(&Network::listener);
}

Network::~Network() {
	uint16_t i;
	for (i = 0; i < m_handlers.size(); ++i) {
		delete m_handlers[i];
	}
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
	delete[] header->raw;
}

void Network::freeMessage(struct Message *message) {
	delete[] message->raw;
}

void Network::multicast(struct CommonHeader &header, DESTINATION destination) {
	SendRequest *request = new SendRequest;

	request->destination = destination;
	request->message.raw = header.raw;
	request->message.rawSize = header.rawSize;
	request->message.UID = header.UID;
	request->message.structure->version = SAHOM_VERSION;

	m_multicastBuffer.push(request);
}

void Network::unicast(struct CommonHeader &header, in6_addr &destination) {
	sockaddr_in6 addr = { 0 };
	Sockets::ClientSocket socket(SOCK_DGRAM);

	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(SAHOM_PORT);
	addr.sin6_addr = destination;

	socket.setAddress(addr);
	socket.sendtoAddress(header.raw, header.rawSize);
	freeHeader(&header);
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

			case DESTINATION::MULTICAST_WHO_IS:
			addr.sin6_addr = WHO_IS_CHANNEL;
			break;

			default:
			continue;
		}

		socket.setAddress(addr);
		socket.sendtoAddress(request->message.raw, request->message.rawSize);
		freeHeader(&request->message);

		//TODO: push message into buffer again if it require a confirm message
		delete request;
	}
	(void) request;
	(void) addr;
}

void Network::listen() {
	m_multicastListener.bind(SAHOM_PORT);

	m_multicastListener.join(SIGN_IN_CHANNEL, 0);
	m_multicastListener.join(WHO_IS_CHANNEL, 0);

	m_stSettings.stayListening = NETWORK_STAY_LISTENING;
	m_multicastListener.listen();
}

void Network::listener(int socket) {
	ssize_t nBytes;
	uint8_t i;
	struct sockaddr_in6 addr;
	socklen_t addressLength = sizeof addr;
	struct SahomNetwork::StandardMessage standardMessage;

	struct SahomNetwork::CommonHeader header;
	header.rawSize = GENERAL_BUFFER_SIZE;
	header.raw = new uint8_t[header.rawSize];

	do {
		if ((nBytes = ::recvfrom(socket, header.raw, header.rawSize, 0, (struct sockaddr *) &addr, &addressLength)) < 0) {
			ERROR("NETWORK", "Reading stream error: ");
			getInstance()->stopListening();
			continue;
		} else if (nBytes == 0) {
			getInstance()->stopListening();
			continue;
		} else {
			if (
					!header.structure->option.all_networks &&
					getInstance()->isConnected() &&
					!Messages::compareNetworkName(header.structure->networkName, getInstance()->getNetworkName())) {
				continue;
			}

			if (header.structure->type == MESSAGE_TYPE_STANDARD) {
				standardMessage.rawSize = header.structure->size;
				standardMessage.raw = header.structure->payload;

				for (i = 0; i < getInstance()->m_handlers.size(); ++i) {
					if (getInstance()->m_handlers[i]->hasHandler(standardMessage.structure->command)) {
						getInstance()->m_handlers[i]->callHandler(header, addr.sin6_addr);
					}
				}
			}
		}
	} while (getInstance()->m_stSettings.stayListening);
	delete[] header.raw;
}

void Network::addMessageHandler(Handlers &handler) {
	m_handlers.push_back(handler.clone());
}

void Network::stopListening() {
	m_stSettings.stayListening = 0;
}

bool Network::isStayListening() {
	return m_stSettings.stayListening;
}

bool Network::isConnected() {
	return m_stSettings.connected;
}

const uint8_t *Network::getNetworkName() const {
	return m_networkName;
}

void Network::setConnected(bool connected) {
	m_stSettings.connected = connected;
}

} /* namespace SahomNetwork */

