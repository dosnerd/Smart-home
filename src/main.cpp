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

#define BUFFER_SIZE 		1024
#define DATA				"Hello world"

#include <Errors.h>
#include <iostream>
#include <errno.h>

#ifdef SERVER

#include "Sockets/ServerSocket.h"
#include "Sockets/Multicast.h"
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#else

#include "Sockets/ClientSocket.h"
#include <sys/socket.h>

#endif

void clientHandler(int socket);

int main(int argc, char** argv) {

	try {

#ifndef SERVER
		Sockets::ClientSocket client(SOCK_DGRAM);
		client.setAddress(argv[1], 59105);
		client.sendtoAddress("Hello world", sizeof "Hello world");
//		client.connect(argv[1], 59105);
//		client.send("Hello world", sizeof "Hello world");
		client.close();
#else
//		Sockets::ServerSocket server;
		Sockets::Multicast server;
		server.setOnAccept(&clientHandler);
		server.bind(59105);
		server.join("ff12::1234", 0);
		server.listen();

		(void) argv;
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
#endif
