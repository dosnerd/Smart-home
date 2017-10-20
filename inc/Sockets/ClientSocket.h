/*
 * CllientSocket.h
 *
 *  Created on: 12 okt. 2017
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

#ifndef INC_CLLIENTSOCKET_H_
#define INC_CLLIENTSOCKET_H_

#include <stdint.h>
#include <cstddef>
#include <arpa/inet.h>

namespace Sockets {

class ClientSocket {
public:
					ClientSocket();
					ClientSocket(int type);
	virtual 		~ClientSocket();

public:
	void virtual	connect(char *ipAddress, uint16_t port);
	void 			send(const void *data, std::size_t size);
	void			setAddress(const char *ipAddress, uint16_t port);
	void			setAddress(sockaddr_in6 addr);
	void			sendtoAddress(const void *data, std::size_t size);
	void			close();

private:
	int				m_fpSocket;
	struct 			sockaddr_in6 m_address;
};

} /* namespace Sockets */

#endif /* INC_CLLIENTSOCKET_H_ */
