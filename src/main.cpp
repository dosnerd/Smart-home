/*
 * main.cpp
 *
 *  Created on: 10 aug. 2017
 *      Author: Acer
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

#define BUFFER_SIZE 		1024
#define DATA				"Hello world"

#ifndef SERVER
#include <arpa/inet.h>
#endif

int main(int argc, char** argv) {

#ifndef SERVER
	int sock;
	struct sockaddr_in serverAddress;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		std::cerr << "Failed to create socket" << std::endl;
		return 1;
	}

	serverAddress.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr.s_addr) <= 0) {
		std::cerr << "Can not parse address" << std::endl;
		return 1;
	}
	serverAddress.sin_port = htons(59105);

	if (connect(sock, (struct sockaddr *)&serverAddress, sizeof serverAddress)) {
		std::cerr << "Connection failed" << std::endl;
		close(sock);
		return 1;
	}

	if (send(sock, DATA, sizeof DATA, 0) < 0) {
		std::cerr << "Send failed" << std::endl;
		close(sock);
		return 1;
	}

	std::cout << "Following data has been send: " << DATA << std::endl;

	std::getwchar();
	close(sock);

#else
	int serverSocket, newSocket, available;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in addr, clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);

	//create socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		std::cerr << "Failed to create socket" << std::endl;
		return 1;
	}

	//bind socket
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(59105);

	if (bind(serverSocket, (struct sockaddr*) &addr, sizeof(addr))) {
		std::cerr << "Bind failed" << std::endl;
		return 1;
	}

	//start server
	listen(serverSocket, 5);
	while (1) {
		newSocket = accept(serverSocket, (struct sockaddr *) &clientAddress,
				&clientAddressLength);

//		newSocket = accept(serverSocket, (struct sockaddr *) 0, 0);
		if (newSocket == -1) {
			std::cerr << "Client not accepted" << std::endl;
			continue;
		}

		std::memset(buffer, 0, sizeof buffer);
		while (1) {
			if ((available = read(newSocket, buffer, sizeof buffer)) < 0) {
				std::cerr << "Reading stream error: " << std::endl;
			} else if (available == 0) {
				std::cout << "Connection lost" << std::endl;
				close(newSocket);
				break;
			} else {
				std::cout << "Message inbound: " << buffer << std::endl;
			}

			std::cout << "AV: " << available << std::endl;
		}
	}

	(void) argv;
#endif

	(void) argc;
	return 0;
}
