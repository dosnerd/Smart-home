/*
 * DiscoveryHandlers.h
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

#ifndef INC_SAHOMNETWORK_DISCOVERYHANDLERS_H_
#define INC_SAHOMNETWORK_DISCOVERYHANDLERS_H_

#include <SahomNetwork/Handlers.h>
#include <SahomNetwork/Messages.h>

#include <queue>

namespace SahomNetwork {

class Network;
class DiscoveryHandlers: public Handlers {
public:
												DiscoveryHandlers();
												DiscoveryHandlers(DeviceUID UID);
												DiscoveryHandlers(const DiscoveryHandlers& other);
												DiscoveryHandlers(DiscoveryHandlers&& other);
	virtual 									~DiscoveryHandlers();

public:
	static DiscoveryHandlers					*getLastInstance();

public:
	bool										hasUID();
	bool										isUID(DeviceUID &other);
	virtual bool								hasHandler(uint16_t command) override;
	virtual void								callHandler(const struct CommonHeader &header, struct in6_addr &addr) override;

	virtual Handlers							*clone() const override;

public:
	void										scan();
	bool										lookup(DeviceUID &uid, struct in6_addr *buffer);
	void										whoIs(DeviceUID &uid, void (*callback)(struct in6_addr));

private:
	void										scanHandler();
	void										scanResponseHandler(const struct CommonHeader &header);
	void										whoIsHandler(const struct CommonHeader &header, struct in6_addr &addr);
	void										whoIsResponseHandler(const struct CommonHeader &header, struct in6_addr &addr);

private:
	static DiscoveryHandlers					*m_sLastInstance;

private:
	Network										*m_networkInstance;
	DeviceUID									m_uid;

	DeviceUID									m_aUnknownDevices[WHO_IS_LIMIT];
	std::queue<void (*)(struct in6_addr)>		m_aCallBacks[WHO_IS_LIMIT];
};

} /* namespace SahomNetwork */

#endif /* INC_SAHOMNETWORK_DISCOVERYHANDLERS_H_ */
