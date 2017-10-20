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

#ifdef SERVER
#define BUFFER_SIZE 		1024

#include "Sockets/ServerSocket.h"
#include "Sockets/Multicast.h"
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void server();
#endif

#ifdef CLIENT
#include "Sockets/ClientSocket.h"
#include <sys/socket.h>
#define DATA			"Hello world"

void clientHandler(int socket);
void client(char **argv);
#endif

#ifdef SIGN_UP
#define DATA			"Hello world"

#include "SahomNetwork.h"
#include "Memoryleak.h"

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
void signup() {
	SahomNetwork instance;
	struct SahomNetwork::Message message;
	instance.createMessage(&message, DATA, sizeof DATA);

	message.structure->type = MESSAGE_TYPE_UNDEFINED;

	instance.multicast(message, SahomNetwork::MULTICAST_SIGN_IN_CHANNEL);
	instance.flush();
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

		if ((nBytes = ::recvfrom(socket, buffer, sizeof buffer, 0,
								(struct sockaddr *) &addr, &addressLength)) < 0) {
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

			if (inet_ntop(AF_INET6, &addr.sin6_addr, buffer, BUFFER_SIZE)
					<= 0) {
				std::cerr << "Can not format IPv6 address";
			} else {
				std::cout << "FROM: " << buffer << std::endl;
			}
		}

		std::cout << "AV: " << nBytes << std::endl << std::endl;
		std::memset(buffer, 0, sizeof buffer);
	}
}
void server() {
//		Sockets::ServerSocket server;
	Sockets::Multicast server;
	server.setOnAccept(&clientHandler);
	server.bind(59105);
	server.join(argv[1], 0);
	server.listen();

	(void) argv;
}
#endif
