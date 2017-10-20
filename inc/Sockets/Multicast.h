/*
 * Multicast.h
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

#ifndef INC_MULTICAST_H_
#define INC_MULTICAST_H_

#include <netinet/in.h>
#include "ServerSocket.h"


namespace Sockets {

class Multicast : public ServerSocket {
public:
					Multicast();
	virtual 		~Multicast();

public:
	void			join(const char *multaddress, unsigned int interface = 0);
	void 			join(struct in6_addr address, unsigned int interface = 0);
	void			listen(uint16_t limit = 5) override;

};

} /* namespace Sockets */

#endif /* INC_MULTICAST_H_ */
