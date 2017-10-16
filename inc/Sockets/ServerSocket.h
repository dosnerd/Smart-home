/*
 * ServerSocket.h
 *
 *  Created on: 11 okt. 2017
 *      Author: Author: David de Prez
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
#ifndef INC_SERVERSOCKET_H_
#define INC_SERVERSOCKET_H_

#include <stdint.h>

namespace Sockets {

class ServerSocket {
public:
						ServerSocket();
						ServerSocket(int socketType);
	virtual 			~ServerSocket();

public:
	inline void 		setOnAccept(void (*onAccept)(int socket)) { m_onAccept = onAccept; }
	
public:
	void				bind(uint16_t port);
	virtual void		listen(uint16_t limit = 5);
	void				close();

protected:
	inline const int	getSocket() { return m_fpSocket; }
	inline void 		(*getOnAccept())(int socket) { return m_onAccept; };

private:
	int					m_fpSocket;
	void				(*m_onAccept)(int socket);
};

} /* namespace Sockets */

#endif /* INC_SERVERSOCKET_H_ */
