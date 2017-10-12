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

#ifdef SERVER

#include "Sockets/ServerSocket.h"
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

#else

#include "Sockets/ClientSocket.h"

#endif

void clientHandler(int socket);

int main(int argc, char** argv) {

#ifndef SERVER
	Sockets::ClientSocket client;
	client.connect(argv[1], 59105);
	client.send("Hello world", sizeof "Hello world");
	client.close();

#else
	Sockets::ServerSocket server;
	server.setOnAccept(&clientHandler);
	server.bind(59105);
	server.listen();

	(void) argv;
#endif

	(void) argc;
	(void) argv;
	return 0;
}


#ifdef SERVER
void clientHandler(int socket) {
	ssize_t nBytes;
	char buffer[BUFFER_SIZE];

	while (1) {
		if ((nBytes = read(socket, buffer, sizeof buffer)) < 0) {
			std::cerr << "Reading stream error: " << std::endl;
		} else if (nBytes == 0) {
			std::cout << "Connection lost" << std::endl;
			return;
		} else {
			std::cout << "Message inbound: " << buffer << std::endl;
		}

		std::cout << "AV: " << nBytes << std::endl;
		std::memset(buffer, 0, sizeof buffer);
	}
}
#endif
