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
#include <config.h>

#include <iostream>

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
}

Network::Network() :
		m_multicastBuffer(), m_multicastListener()
{
	m_multicastListener.setOnAccept(&Network::listener);
}

Network::~Network() {

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

void Network::scan() {
	struct SahomNetwork::CommonHeader header;
	struct SahomNetwork::StandardMessage message;

	CreateMessage((Message *) &message, sizeof(*message.structure));
	message.structure->command = STANDARD_MESSAGE_COMMAND_SCAN;
	message.structure->nParameters = 0;

	InitHeader(&header, message.raw, message.rawSize);
	header.structure->type = MESSAGE_TYPE_STANDARD;
	freeMessage((Message *) &message);

	multicast(header, MULTICAST_SIGN_IN_CHANNEL);
	flush();
}

void Network::multicast(struct CommonHeader header, DESTINATION destination) {
	SendRequest *request = new SendRequest;

	request->destination = destination;
	request->message.raw = header.raw;
	request->message.rawSize = header.rawSize;
	request->message.structure->version = SAHOM_VERSION;

	m_multicastBuffer.push(request);
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

void Network::listen() {
	m_multicastListener.bind(SAHOM_PORT);
	m_multicastListener.join(SIGN_IN_CHANNEL, 0);
	m_multicastListener.listen();
}

void Network::listener(int socket) {
	ssize_t nBytes;
	uint8_t i;
	struct sockaddr_in6 addr;
	socklen_t addressLength = sizeof addr;
	SahomNetwork::StandardMessage standardMessage;

	SahomNetwork::CommonHeader header;
	header.rawSize = GENERAL_BUFFER_SIZE;
	header.raw = new uint8_t[header.rawSize];

	while (1) {
		if ((nBytes = ::recvfrom(socket, header.raw, header.rawSize, 0, (struct sockaddr *) &addr, &addressLength)) < 0) {
			std::cerr << "Reading stream error: " << std::endl;
			return;
		} else if (nBytes == 0) {
			return;
		} else {
			std::cout << "Network name:\t" << header.structure->networkName << std::endl;
			std::cout << "Version:\t" << header.structure->version + 0 << std::endl;
			std::cout << "Type:\t\t" << header.structure->type + 0 << std::endl;
			std::cout << "Size:\t\t" << header.structure->size << std::endl << std::endl;

			if (header.structure->type == MESSAGE_TYPE_STANDARD) {
				std::cout << "STANDARD MESSAGE: " << std::endl;
				standardMessage.rawSize = header.structure->size;
				standardMessage.raw = header.structure->payload;

				std::cout << "Command:\t" << standardMessage.structure->command + 0 << std::endl;
				std::cout << "Count params:\t" << standardMessage.structure->nParameters + 0 << std::endl;

				for (i = 0; i < standardMessage.structure->nParameters; ++i) {
					std::cout << "\t[" << i + 0 << "]: " << standardMessage.structure->parameters[i] + 0 << std::endl;
				}

				switch (standardMessage.structure->command) {
				case STANDARD_MESSAGE_COMMAND_SCAN:
					getInstance()->scanHandler();
					break;

				default:
					break;
				}
			}
		}
	}
}

void Network::scanHandler() {
	StandardMessage message;
	CommonHeader header;

	CreateMessage((Message *) &message, sizeof message.structure);
	message.structure->command = STANDARD_MESSAGE_COMMAND_SCAN_REACT;
	message.structure->nParameters = 0;

	InitHeader(&header, message.raw, message.rawSize);
	header.structure->type = MESSAGE_TYPE_STANDARD;
	freeMessage((Message *) &message);

	multicast(header, DESTINATION::MULTICAST_SIGN_IN_CHANNEL);
}

} /* namespace SahomNetwork */

