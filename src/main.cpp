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

#ifdef SIGN_UP
#define DATA			"Hello world"

#include <SahomNetwork/Network.h>
#include <SahomNetwork/DiscoveryHandlers.h>
#include "Memoryleak.h"
#include <thread>
#include <unistd.h>

void signup();
#endif

int main() {

	try {
		signup();

		if (Memory_leak::current.check()) {
			LOG("MAIN", "One or more test has memory leaks");
		} else {
			LOG("MAIN", "All runned test passed memory leak test" << std::endl);
		}

	} catch (ERRORS &err) {
		ERROR("MAIN", \
				"E" << err << std::endl <<
				"errno: " << errno
		);
		return 1;
	} catch (...) {
		ERROR("MAIN", "Unexpected error");
		return 1;
	}
	return 0;
}

void startListening() {
	SahomNetwork::Network::getInstance()->listen();
}

void signup() {
	uint16_t i = 0;
	SahomNetwork::Network *instance = SahomNetwork::Network::getInstance();
	std::thread server(startListening);
	SahomNetwork::DiscoveryHandlers discovery;
	instance->addMessageHandler(&discovery);

	instance->scan();

	while (++i < 10) {
		usleep(500 * 1000);
		instance->flush();
	}
	LOG("MAIN", "Last receive");

	instance->stopListening();
	server.join();

	usleep(500 * 1000);

	instance->flush();
	instance->destroyInstance();
}
