/*
 * main.cpp
 *
 *  Created on: 10 aug. 2017
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
#include <Errors.h>
#include <iostream>
#include <errno.h>
#include <config.h>

#ifdef SERVER
#define BUFFER_SIZE 		1024

#include <SahomNetwork/Network.h>
#include "Sockets/ServerSocket.h"
#include "Sockets/Multicast.h"
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void server();
void clientHandler(int socket);
void signInHandler(int socket);
#endif

#ifdef CLIENT
#include "Sockets/ClientSocket.h"
#include <sys/socket.h>
#define DATA			"Hello world"

void client(char **argv);
#endif

#ifdef SIGN_UP
#define DATA			"Hello world"

#include <SahomNetwork/Network.h>
#include "Memoryleak.h"
#include <thread>
#include <unistd.h>

void signup();
#endif

int main(int argc, char** argv) {

	try {
#ifdef CLIENT
		client(argv);
#endif
#ifdef SERVER
		server();
#endif
#ifdef SIGN_UP
		signup();
		if (Memory_leak::current.check()) {
			std::cout << "One or more test has memory leaks" << std::endl;
		} else {
			std::cout << "All runned test passed memory leak test" << std::endl;
		}
#endif

	} catch (ERRORS err) {
		std::cerr << "E" << err << std::endl;
		std::cerr << "errno: " << errno << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "Unexpected error" << std::endl;
		return 1;
	}

	(void) argc;
	(void) argv;
	return 0;
}

#ifdef SIGN_UP
void startListening() {
	SahomNetwork::Network::getInstance()->listen();
}

void signup() {
	SahomNetwork::Network *instance = SahomNetwork::Network::getInstance();
	std::thread server(startListening);
	instance->scan();

	while (true) {
		usleep(5 * 1000);
		instance->flush();
	}

	server.join();
}
#endif

#ifdef CLIENT
void client(char **argv) {
	Sockets::ClientSocket client(SOCK_DGRAM);
	client.setAddress(argv[1], 59105);
	client.sendtoAddress(DATA, sizeof DATA);
//		client.connect(argv[1], 59105);
//		client.send("Hello world", sizeof "Hello world");
	client.close();
	(void)argv;
}
#endif

#ifdef SERVER
void clientHandler(int socket) {
	ssize_t nBytes, i;
	struct sockaddr_in6 addr;
	socklen_t addressLength = sizeof addr;
	char buffer[BUFFER_SIZE];

	while (1) {

		if ((nBytes = ::recvfrom(socket, buffer, sizeof buffer, 0, (struct sockaddr *) &addr, &addressLength)) < 0) {
			std::cerr << "Reading stream error: " << std::endl;
		} else if (nBytes == 0) {
			std::cout << "Connection lost" << std::endl;
			return;
		} else {
			std::cout << "Message inbound: ";

			for (i = 0; i < nBytes; ++i) {
				std::cout << buffer[i];
			}
			std::cout << std::endl;

			if (inet_ntop(AF_INET6, &addr.sin6_addr, buffer, BUFFER_SIZE) <= 0) {
				std::cerr << "Can not format IPv6 address";
			} else {
				std::cout << "FROM: " << buffer << std::endl;
			}
		}

		std::cout << "AV: " << nBytes << std::endl << std::endl;
		std::memset(buffer, 0, sizeof buffer);
	}
}

void signInHandler(int socket) {
	ssize_t nBytes;
	uint8_t i;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in6 addr;
	socklen_t addressLength = sizeof addr;
	SahomNetwork::StandardMessage standardMessage;

	SahomNetwork::CommonHeader header;
	header.rawSize = BUFFER_SIZE;
	header.raw = new uint8_t[header.rawSize];

	while (1) {
		if ((nBytes = ::recvfrom(socket, header.raw, header.rawSize, 0, (struct sockaddr *) &addr, &addressLength)) < 0) {
			std::cerr << "Reading stream error: " << std::endl;
		} else if (nBytes == 0) {
			std::cout << "Connection lost" << std::endl;
			return;
		} else {
			std::cout << "Message inbound from: ";
			if (inet_ntop(AF_INET6, &addr.sin6_addr, buffer, BUFFER_SIZE) <= 0) {
				std::cerr << "E" << ERRORS::PARSE_IP_ADDRES_FAILED;
			} else {
				std::cout << buffer << std::endl;
			}

			std::cout << "Network name:\t" << header.structure->networkName << std::endl;
			std::cout << "Version:\t" << header.structure->version + 0 << std::endl;
			std::cout << "Type:\t\t" << header.structure->type + 0 << std::endl;
			std::cout << "Size:\t\t" << header.structure->size << std::endl << std::endl;

			if (header.structure->type == MESSAGE_TYPE_STANDARD) {
				std::cout << "STANDARD MESSAGE: " << std::endl;
				standardMessage.rawSize = header.structure->size;
				standardMessage.raw = header.structure->payload;

				std::cout << "Command:\t"<< standardMessage.structure->command + 0 << std::endl;
				std::cout << "Count params:\t"<< standardMessage.structure->nParameters + 0 << std::endl;

				for (i = 0; i < standardMessage.structure->nParameters; ++i) {
					std::cout << "\t["<< i + 0 << "]: " << standardMessage.structure->parameters[i] + 0 << std::endl;
				}
			}
		}

		std::cout << std::endl << "AV: " << nBytes << std::endl << std::endl;
		std::memset(header.raw, 0, sizeof header.rawSize);
	}
}

void server() {
//		Sockets::ServerSocket server;
	Sockets::Multicast server;
	server.setOnAccept(&signInHandler);
	server.bind(59105);
	server.join(SIGN_IN_CHANNEL, 0);
	server.listen();
}

#endif
