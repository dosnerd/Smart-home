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
#include <cstring>

namespace SahomNetwork {

DiscoveryHandlers *DiscoveryHandlers::m_sLastInstance = nullptr;

DiscoveryHandlers::DiscoveryHandlers() :
		m_networkInstance(Network::getInstance())
{
	m_uid.raw = nullptr;
}

DiscoveryHandlers::DiscoveryHandlers(DeviceUID UID) :
		m_networkInstance(Network::getInstance())
{
	m_uid.raw = new uint8_t[UID.rawSize];
	m_uid.rawSize = UID.rawSize;
	std::memcpy(m_uid.raw, UID.raw, m_uid.rawSize);
}

DiscoveryHandlers::DiscoveryHandlers(const DiscoveryHandlers& other) :
		m_networkInstance(other.m_networkInstance)
{
	m_uid.raw = new uint8_t[other.m_uid.rawSize];
	m_uid.rawSize = other.m_uid.rawSize;
	std::memcpy(m_uid.raw, other.m_uid.raw, m_uid.rawSize);
	m_sLastInstance = this;
}

DiscoveryHandlers::DiscoveryHandlers(DiscoveryHandlers&& other) :
		m_networkInstance(other.m_networkInstance),
		m_uid(other.m_uid),
		m_aUnknownDevices(other.m_aUnknownDevices),
		m_aCallBacks(other.m_aCallBacks)
{
	other.m_networkInstance = nullptr;
	other.m_uid.raw = nullptr;
}

DiscoveryHandlers::~DiscoveryHandlers() {
	delete[] m_uid.raw;
	if (m_sLastInstance == this)
		m_sLastInstance = nullptr;
}

DiscoveryHandlers *DiscoveryHandlers::getLastInstance() {
	return m_sLastInstance;
}

bool DiscoveryHandlers::hasUID() {
	return m_uid.raw != nullptr;
}

bool DiscoveryHandlers::isUID(DeviceUID &other) {
	if (!hasUID())
		return false;

	if (other.raw == nullptr)
		return false;

	return compareUIDS(m_uid, other);
}

bool DiscoveryHandlers::compareUIDS(DeviceUID &a, DeviceUID &b) {
	uint8_t i;

	//Device UID should always as small as possible, meaning if size isn't same, UID can not be the same
	if (a.structure->size != b.structure->size) {
		return false;
	}

	for (i = 0; i < a.structure->size; i++) {
		if (a.structure->UID[i] != b.structure->UID[i])
			return false;
	}

	return true;
}

bool DiscoveryHandlers::hasHandler(uint16_t command) {
	return command >= STANDARD_MESSAGE_COMMAND_SCAN && command <= STANDARD_MESSAGE_COMMAND_WHO_IS;
}

void DiscoveryHandlers::callHandler(const struct CommonHeader& header, struct in6_addr &addr) {
	struct StandardMessage message;
	message.rawSize = header.structure->size;
	message.raw = header.structure->payload;

	switch (message.structure->command) {
	case STANDARD_MESSAGE_COMMAND_SCAN:
		if (header.structure->option.received)
			scanResponseHandler(header);
		else
			scanHandler();
		return;

	case STANDARD_MESSAGE_COMMAND_WHO_IS:
		if (header.structure->option.received)
			whoIsResponseHandler(header, addr);
		else
			whoIsHandler(header, addr);
		return;
	default:
		WARNING("DISCOVERY", "Unknown command: " << message.structure->command);
		return;
	}
}

Handlers* DiscoveryHandlers::clone() const {
	return new DiscoveryHandlers(*this);
}

void DiscoveryHandlers::whoIs(DeviceUID &uid, void (*callback)(struct in6_addr)) {
	StandardMessage message;
	CommonHeader header;
	uint8_t i;

	if (!m_networkInstance->isConnected()) {
		return;
	}

	for (i = 0; i < WHO_IS_LIMIT; ++i) {
		if (m_aUnknownDevices[i].rawSize == 0 || compareUIDS(m_aUnknownDevices[i], uid)) {
			m_aUnknownDevices[i].raw = new uint8_t[uid.rawSize];
			m_aUnknownDevices[i].rawSize = uid.rawSize;
			std::memcpy(m_aUnknownDevices[i].raw, uid.raw, uid.rawSize);

			m_aCallBacks[i].push(callback);
			break;
		}
	}

	if (i == WHO_IS_LIMIT) {
		WARNING("DISCOVERY", "WHO IS buffer overflow!");
		return;
	}

	m_networkInstance->CreateMessage((Message *) &message, sizeof(message.structure) + uid.structure->size);
	message.structure->command = STANDARD_MESSAGE_COMMAND_WHO_IS;
	std::memcpy(&message.structure->nParameters, &uid.structure->size, sizeof(uid.structure->size) + uid.structure->size);

	m_networkInstance->InitHeader(&header, message.raw, message.rawSize);
	header.structure->type = MESSAGE_TYPE_STANDARD;
	header.structure->option.require_confirm = 1;
	m_networkInstance->freeMessage((Message *) &message);

	m_networkInstance->multicast(header, Network::DESTINATION::MULTICAST_WHO_IS);
}

void DiscoveryHandlers::scanHandler() {
	StandardMessage message;
	CommonHeader header;

	if (!m_networkInstance->isConnected()) {
		return;
	}

	m_networkInstance->CreateMessage((Message *) &message, sizeof message.structure);
	message.structure->command = STANDARD_MESSAGE_COMMAND_SCAN;
	message.structure->nParameters = 0;

	m_networkInstance->InitHeader(&header, message.raw, message.rawSize);
	header.structure->type = MESSAGE_TYPE_STANDARD;
	header.structure->option.received = 1;
	m_networkInstance->freeMessage((Message *) &message);

	m_networkInstance->multicast(header, Network::DESTINATION::MULTICAST_SIGN_IN_CHANNEL);
}

void DiscoveryHandlers::scanResponseHandler(const struct CommonHeader &header) {
	LOG("DISCOVERY", "Network found: " << header.structure->networkName);
}

void DiscoveryHandlers::whoIsHandler(const struct CommonHeader &header, struct in6_addr &addr) {
	StandardMessage message;
	CommonHeader sendHeader;
	DeviceUID uid;
	uint8_t uidSize;

	message.raw = header.structure->payload;
	uidSize = message.structure->nParameters;

	uid.rawSize = sizeof(uid.structure) + uidSize;
	uid.raw = new uint8_t[uid.rawSize];
	std::memcpy(uid.raw, &message.structure->nParameters, sizeof(uid.structure->size) + uidSize);

	if (isUID(uid)) {
		message.raw = nullptr;

		m_networkInstance->CreateMessage((Message *) &message, sizeof(message.structure) + uid.structure->size);
		message.structure->command = STANDARD_MESSAGE_COMMAND_WHO_IS;
		std::memcpy(&message.structure->nParameters, &uid.structure->size, sizeof(uid.structure->size) + uid.structure->size);

		m_networkInstance->InitHeader(&sendHeader, message.raw, message.rawSize);
		sendHeader.structure->type = MESSAGE_TYPE_STANDARD;
		sendHeader.structure->option.received = 1;
		m_networkInstance->freeMessage((Message *) &message);

		m_networkInstance->unicast(sendHeader, addr);
		(void) addr;
	}

	delete[] uid.raw;
}

void DiscoveryHandlers::whoIsResponseHandler(const struct CommonHeader &header, struct in6_addr &addr) {
	(void) header;
	uint8_t i;
	StandardMessage message;
	DeviceUID uid;
	uint8_t uidSize;

	message.raw = header.structure->payload;
	uidSize = message.structure->nParameters;

	uid.rawSize = sizeof(uid.structure) + uidSize;
	uid.raw = new uint8_t[uid.rawSize];
	std::memcpy(uid.raw, &message.structure->nParameters, sizeof(uid.structure->size) + uidSize);

	for (i = 0; i < WHO_IS_LIMIT; ++i) {
		if (m_aUnknownDevices[i].rawSize != 0 && compareUIDS(m_aUnknownDevices[i], uid)) {
			while (m_aCallBacks[i].size() > 0) {
				m_aCallBacks[i].front()(addr);
				m_aCallBacks[i].pop();
			}
			m_aUnknownDevices[i].rawSize = 0;
			delete[] m_aUnknownDevices[i].raw;
		}
	}
}

} /* namespace SahomNetwork */

