/*
 * Multicast.h
 *
 *  Created on: 16 okt. 2017
 *      Author: Acer
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
