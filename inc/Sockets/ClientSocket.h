/*
 * CllientSocket.h
 *
 *  Created on: 12 okt. 2017
 *      Author: Acer
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
