/*
 * SahomNetwork.h
 *
 *  Created on: 16 okt. 2017
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

#ifndef INC_SAHOMNETWORK_H_
#define INC_SAHOMNETWORK_H_

#include <stdint.h>
#include <queue>
#include <vector>
#include <Sockets/Multicast.h>
#include "Messages.h"

namespace SahomNetwork {

class Handlers;
class Network {
public:
	static Network				*getInstance();
	static void					destroyInstance();
public:
	typedef enum {
		MULTICAST_SIGN_IN_CHANNEL,
		MULTICAST_WHO_IS,
	} DESTINATION;

public:
	void						InitHeader(struct CommonHeader *header, const void *payload, uint16_t size);
	void						CreateMessage(struct Message *message, uint16_t size);
	void						freeHeader(struct CommonHeader *header);
	void						freeMessage(struct Message *message);

public:
	void						scan();
	void						multicast(struct CommonHeader &header, DESTINATION destination);
	void						unicast(struct CommonHeader &header, in6_addr &destination);
	void						addMessageHandler(Handlers &handler);
	void						flush();
	void						listen();
	void						stopListening();
	
public:
	bool						isStayListening();
	bool						isConnected();

	void						setConnected(bool connected);


private:
								Network();
	virtual 					~Network();

private:
	static void					listener(int socket);

private:
	static Network				*m_sInstance;
	struct						SendRequest {
		DESTINATION					destination;
		struct CommonHeader				message;
	};

	union {
		struct {
			uint8_t					stayListening	: 1;
			uint8_t					connected		: 1;
		} 						m_stSettings;
		uint8_t					m_settings;
	};
	uint8_t 					m_networkName[30] = {'M', 'y', ' ', 'S', 'a', 'h', 'o', 'm', 0};
	std::queue<SendRequest *>	m_multicastBuffer;
	Sockets::Multicast			m_multicastListener;

	std::vector<Handlers *>		m_handlers;
};

} /* namespace SahomNetwork */

#endif /* INC_SAHOMNETWORK_H_ */
