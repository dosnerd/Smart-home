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

namespace Sockets {

class ClientSocket {
public:
					ClientSocket();
	virtual 		~ClientSocket();

public:
	void			connect(char *ipAddress, uint16_t port);
	void			send(const void *data, std::size_t size);
	void			close();

private:
	int				m_fpSocket;
};

} /* namespace Sockets */

#endif /* INC_CLLIENTSOCKET_H_ */
