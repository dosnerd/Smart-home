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
#include <Log.h>

#include <SahomNetwork/Network.h>
#include <SahomNetwork/DiscoveryHandlers.h>
#include <SahomNetwork/SettingsHandler.h>
#include <SahomNetwork/Messages.h>
#include "Memoryleak.h"
#include <thread>
#include <unistd.h>

#ifdef MANAGER
#include <string>
#include <cstring>
#endif

Memory_leak Memory_leak::current;

void start(SahomNetwork::DeviceUID uid);
void setup(void *temp);
void loop();

int main(int nArgs, char* args[]) {
	SahomNetwork::DeviceUID uid;

	try {
		if (nArgs != 2) {
			ERROR("MAIN", "unknown UID");
			return 1;
		}

		uid.rawSize = sizeof *uid.structure + 1;
		uid.raw = new uint8_t[uid.rawSize];
		uid.structure->size = 1;
		uid.structure->UID[0] = std::stoi(args[1]);

		start(uid);

		delete[] uid.raw;

		if (Memory_leak::current.check()) {
			LOG("MAIN", "One or more test has memory leaks");
		} else {
			LOG("MAIN", "All runned test passed memory leak test" << std::endl);
		}

	} catch (ERRORS &err) {
		ERROR("MAIN", "E" << err << std::endl << "errno: " << errno);
		return 1;
	} catch (...) {
		ERROR("MAIN", "Unexpected error");
		return 1;
	}
	return 0;
}

void startListening() {
	try {
		SahomNetwork::Network::getInstance()->listen();
	} catch (ERRORS &err) {
		ERROR("MAIN", "E" << err << std::endl << "errno: " << errno);
		return;
	} catch (...) {
		ERROR("MAIN", "Unexpected error");
		return;
	}
}

#ifdef SIGN_UP

void start(SahomNetwork::DeviceUID uid) {
	uint16_t i = 0;
	SahomNetwork::Network *instance = SahomNetwork::Network::getInstance();
	std::thread server(startListening);
	SahomNetwork::DiscoveryHandlers discovery(uid);
	SahomNetwork::SettingsHandler settings;

	try {
		LOG("MAIN", "Current network: '" << instance->getNetworkName() << "'");

		instance->addMessageHandler(discovery);
		instance->addMessageHandler(settings);

		while (!instance->isConnected()) {
			sleep(1);
			//WARNING: this period not able to send multicast message!
		}

		setup(nullptr);
		while (instance->isConnected()) {
			loop();

			if (i++ >= (5000 * 1)) {
				instance->flush();
				i = 0;
			}

#ifdef CYGWIN
			sleep(2);
			i = 6000;
#endif
#ifdef LINUX
			usleep(200);
#endif

		}
		LOG("MAIN", "Last receive");

		instance->stopListening();

	} catch (ERRORS &err) {
		ERROR("MAIN", "E" << err << std::endl << "errno: " << errno);
	} catch (...) {
		ERROR("MAIN", "Unexpected error");
	}

	server.join();

	usleep(500 * 1000);

	instance->flush();
	instance->destroyInstance();
}

void setup(void *temp) {
	(void) temp;
	SahomNetwork::DiscoveryHandlers *disc = SahomNetwork::DiscoveryHandlers::getLastInstance();
	uint8_t buffer[2];
	SahomNetwork::DeviceUID uid;
	uid.rawSize = 2;
	uid.raw = buffer;
	uid.structure->size = 1;
	uid.structure->UID[0] = 12;

	disc->whoIs(uid, [](struct in6_addr addr) {
				LOG("MAIN", "Callback function");
				(void)addr;
			});
}

void loop() {
}

#endif

#ifdef MANAGER

void start(SahomNetwork::DeviceUID uid) {
	SahomNetwork::Network *instance = SahomNetwork::Network::getInstance();
	std::thread server(startListening);
	SahomNetwork::DiscoveryHandlers discovery(uid);
	SahomNetwork::SettingsHandler settings;

	try {
		LOG("MAIN", "Current network: '" << instance->getNetworkName() << "'");

		instance->addMessageHandler(discovery);
		instance->setConnected(true);

		setup(nullptr);
		while (instance->isConnected()) {
			loop();
		}
		LOG("MAIN", "Last receive");

		instance->flush();
		instance->stopListening();

	} catch (ERRORS &err) {
		ERROR("MAIN", "E" << err << std::endl << "errno: " << errno);
	} catch (...) {
		ERROR("MAIN", "Unexpected error");
	}

	server.join();

	usleep(500 * 1000);

	instance->flush();
	instance->destroyInstance();
}

void setup(void *temp) {
	(void) temp;
}

void setName() {
	SahomNetwork::Network *instance = SahomNetwork::Network::getInstance();
	struct SahomNetwork::CommonHeader header;
	struct SahomNetwork::StandardMessage message;
	std::string name;

	std::cout << "Name: ";
	std::cin >> name;

	instance->CreateMessage((SahomNetwork::Message *) &message, sizeof(*message.structure) + 2 + name.length());
	message.structure->command = STANDARD_MESSAGE_COMMAND_SETTINGS;
	message.structure->nParameters = 2 + name.length();
	message.structure->parameters[0] = 1;
	std::memcpy(message.structure->parameters + 1, name.c_str(), name.length() + 1);

	instance->InitHeader(&header, message.raw, message.rawSize);
	header.structure->type = MESSAGE_TYPE_STANDARD;
	instance->freeMessage((SahomNetwork::Message *) &message);
	instance->multicast(header, SahomNetwork::Network::DESTINATION::MULTICAST_SIGN_IN_CHANNEL);
	LOG("MAIN", "Request in queue");
}

void setConnected() {
	SahomNetwork::Network *instance = SahomNetwork::Network::getInstance();
	struct SahomNetwork::CommonHeader header;
	struct SahomNetwork::StandardMessage message;
	uint16_t val = 2;

	while (val > 1) {
		std::cout << "0/1: ";
		std::cin >> val;
	}

	instance->CreateMessage((SahomNetwork::Message *) &message, sizeof(*message.structure) + 2);
	message.structure->command = STANDARD_MESSAGE_COMMAND_SETTINGS;
	message.structure->nParameters = 2;
	message.structure->parameters[0] = 2;
	message.structure->parameters[1] = val;

	instance->InitHeader(&header, message.raw, message.rawSize);
	header.structure->type = MESSAGE_TYPE_STANDARD;
	instance->freeMessage((SahomNetwork::Message *) &message);
	instance->multicast(header, SahomNetwork::Network::DESTINATION::MULTICAST_SIGN_IN_CHANNEL);
	LOG("MAIN", "Request in queue");
}

void settings() {
	std::string command;
	LOG("MAIN", "SETTINGS");

	std::cin >> command;

	if (command == "set_name") {
		setName();
	} else if (command == "set_connected") {
		setConnected();
	} else {
		WARNING("MAIN", "UNKNOWN COMMAND");
	}
}

void loop() {
	SahomNetwork::Network *instance = SahomNetwork::Network::getInstance();
	std::string command;

	std::cin >> command;

	if (command == "stop") {
		instance->setConnected(false);
	} else if (command == "flush") {
		instance->flush();
		LOG("MAIN", "FLUSHED");
	} else if (command == "scan") {
		SahomNetwork::DiscoveryHandlers::getLastInstance()->scan();
		LOG("MAIN", "SCANNED");
	} else if (command == "settings") {
		settings();
	} else {
		WARNING("MAIN", "UNKNOWN COMMAND");
	}
}

#endif
