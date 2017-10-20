/*
 * Multicast.cpp
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

#include <Multicast.h>
#include <Errors.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace Sockets {

Multicast::Multicast()
	: ServerSocket(SOCK_DGRAM)
{
}

Multicast::~Multicast() {
}

void Multicast::join(const char *multaddress, unsigned int interface) {
	struct ipv6_mreq group;
	group.ipv6mr_interface = interface;
	inet_pton(AF_INET6, multaddress, &group.ipv6mr_multiaddr);

	if (::setsockopt(getSocket(), IPPROTO_IPV6, IPV6_JOIN_GROUP, &group, sizeof group) < 0){
		throw ERRORS::SOCKET_JOIN_CHANNEL;
	}
}

void Multicast::join(struct in6_addr address, unsigned int interface) {
	struct ipv6_mreq group;
	group.ipv6mr_interface = interface;
	group.ipv6mr_multiaddr = address;

	if (::setsockopt(getSocket(), IPPROTO_IPV6, IPV6_JOIN_GROUP, &group, sizeof group) < 0){
		throw ERRORS::SOCKET_JOIN_CHANNEL;
	}
}


void Multicast::listen(uint16_t limit) {
	getOnAccept()(getSocket());
	(void)limit;
}


} /* namespace Sockets */
