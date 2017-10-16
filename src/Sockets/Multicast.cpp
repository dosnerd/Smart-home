/*
 * Multicast.cpp
 *
 *  Created on: 16 okt. 2017
 *      Author: Acer
 */

#include <Multicast.h>
#include <Errors.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
	if (::setsockopt(getSocket(), IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &group, sizeof group) < 0){
		throw ERRORS::SOCKET_JOIN_CHANNEL;
	}
}


void Multicast::listen(uint16_t limit) {
	getOnAccept()(getSocket());
	(void)limit;
}


} /* namespace Sockets */
