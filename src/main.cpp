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
#include <SahomNetwork/Messages.h>
#include "Memoryleak.h"
#include <thread>
#include <unistd.h>

Memory_leak Memory_leak::current;

void start(SahomNetwork::DeviceUID uid);
void setup(void *temp);
void loop();

void setup(void *temp) {
	(void)temp;
	SahomNetwork::DiscoveryHandlers *disc = SahomNetwork::DiscoveryHandlers::getLastInstance();
	uint8_t buffer[2];
	SahomNetwork::DeviceUID uid;
	uid.rawSize = 2;
	uid.raw = buffer;
	uid.structure->size = 1;
	uid.structure->UID[0] = 12;


	disc->whoIs(uid, [](struct in6_addr addr){
		LOG("MAIN", "Callback function");
		(void)addr;
	});
}

void loop() {
}

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

void start(SahomNetwork::DeviceUID uid) {
	uint16_t i = 0;
	SahomNetwork::Network *instance = SahomNetwork::Network::getInstance();
	std::thread server(startListening);
	SahomNetwork::DiscoveryHandlers discovery(uid);

	try {
		LOG("MAIN", "Current network: '" << instance->getNetworkName() << "'");

		instance->addMessageHandler(discovery);
		SahomNetwork::DiscoveryHandlers::getLastInstance()->scan();

		//make sure it won't react on it own scan request (faking connecting to system)
		sleep(1);
		instance->setConnected(true);

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
