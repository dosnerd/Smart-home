/*
 * ServerSocket.h
 *
 *  Created on: 11 okt. 2017
 *      Author: Acer
 */

#ifndef INC_SERVERSOCKET_H_
#define INC_SERVERSOCKET_H_

#include <stdint.h>

class ServerSocket {
public:
					ServerSocket();
	virtual 		~ServerSocket();

public:
	void 			setOnAccept(void (*onAccept)(int socket));
	
public:
	void			bind(uint16_t port);
	void			listen(uint16_t limit = 5);

private:
	int				m_fpServerSocket;
	
	void			(*onAccept)(int socket);
};

#endif /* INC_SERVERSOCKET_H_ */
