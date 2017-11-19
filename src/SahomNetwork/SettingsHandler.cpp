/*
 * SettingsHandler.cpp
 *
 *  Created on: 19 nov. 2017
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

#include <SettingsHandler.h>
#include <Log.h>

#define SET_NAME			1
#define SET_CONNECTED		2

namespace SahomNetwork {

SettingsHandler::SettingsHandler() :
		m_networkInstance(Network::getInstance())
{
}

SettingsHandler::SettingsHandler(const SettingsHandler& other) :
		m_networkInstance(other.m_networkInstance)
{
}

SettingsHandler::~SettingsHandler() {
	m_networkInstance = nullptr;
}

bool SettingsHandler::hasHandler(uint16_t command) {
	return command == STANDARD_MESSAGE_COMMAND_SETTINGS;
}

void SettingsHandler::callHandler(const struct CommonHeader& header, struct in6_addr& addr) {
	struct StandardMessage message;
	message.rawSize = header.structure->size;
	message.raw = header.structure->payload;

	if (message.structure->nParameters == 0) {
		WARNING("SETTINGS", "Message has to few parameters");
		return;
	}

	switch (message.structure->parameters[0]) {
	case SET_NAME:
		setNameHandler(message);
		break;
	case SET_CONNECTED:
		setConnectedHandler(message);
		break;

	default:
		WARNING("SETTINGS", "Unknown command: " << message.structure->command);
		return;
	}

	(void) addr;
}

Handlers* SettingsHandler::clone() const {
	return new SettingsHandler(*this);
}

void SettingsHandler::setNameHandler(StandardMessage &message) {
	if (message.structure->nParameters < 2) {
		WARNING("SETTINGS", "Message has to few parameters");
		return;
	}

	m_networkInstance->setNetworkName(message.structure->parameters + 1);
	LOG("SETTINGS", "Network name changed to: " << m_networkInstance->getNetworkName());
}

void SettingsHandler::setConnectedHandler(StandardMessage &message) {
	if (message.structure->nParameters < 2) {
		WARNING("SETTINGS", "Message has to few parameters");
		return;
	}

	m_networkInstance->setConnected(message.structure->parameters[1] > 0);

	if (m_networkInstance->isConnected())
		LOG("SETTINGS", "Change connection status to \'connected\'");
	else
		LOG("SETTINGS", "Change connection status to \'disconnected\'");
}

} /* namespace SahomNetwork */
