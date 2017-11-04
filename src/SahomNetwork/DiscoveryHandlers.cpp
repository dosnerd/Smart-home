/*
 * DiscoveryHandlers.cpp
 *
 *  Created on: 23 okt. 2017
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

#include <SahomNetwork/DiscoveryHandlers.h>
#include <SahomNetwork/Network.h>
#include <Log.h>

namespace SahomNetwork {

DiscoveryHandlers::DiscoveryHandlers()
	: m_networkInstance(Network::getInstance())
{
}

DiscoveryHandlers::~DiscoveryHandlers() {
}

bool DiscoveryHandlers::hasHandler(uint16_t command) {
	return command >= STANDARD_MESSAGE_COMMAND_SCAN && command <= STANDARD_MESSAGE_COMMAND_SIGN_IN;
}

void DiscoveryHandlers::callHandler(struct CommonHeader& header) {
	struct StandardMessage message;
	message.rawSize = header.structure->size;
	message.raw = header.structure->payload;

	switch(message.structure->command){
	case STANDARD_MESSAGE_COMMAND_SCAN:

		if (header.structure->option.received)
			scanHandler();
		else
			scanResponseHandler(header);

		return;
	default:
		return;
	}
}

Handlers* DiscoveryHandlers::clone() const {
	return new DiscoveryHandlers(*this);
}

void DiscoveryHandlers::scanHandler() {
	StandardMessage message;
	CommonHeader header;

	m_networkInstance->CreateMessage((Message *) &message, sizeof message.structure);
	message.structure->command = STANDARD_MESSAGE_COMMAND_SCAN;
	message.structure->nParameters = 0;

	m_networkInstance->InitHeader(&header, message.raw, message.rawSize);
	header.structure->type = MESSAGE_TYPE_STANDARD;
	header.structure->option.received = 1;
	m_networkInstance->freeMessage((Message *) &message);

	m_networkInstance->multicast(header, Network::DESTINATION::MULTICAST_SIGN_IN_CHANNEL);
}

void DiscoveryHandlers::scanResponseHandler(struct CommonHeader &header) {
	LOG("DISCOVERY", "Network found: " << header.structure->networkName);
}

} /* namespace SahomNetwork */
