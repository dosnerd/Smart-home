/*
 * SahomNetwork.h
 *
 *  Created on: 16 okt. 2017
 *      Author: Acer
 */

#ifndef INC_SAHOMNETWORK_H_
#define INC_SAHOMNETWORK_H_

#define MESSAGE_TYPE_UNDEFINED		0

#include <stdint.h>
#include <queue>
#include <config.h>

class SahomNetwork {
public:
								SahomNetwork();
	virtual 					~SahomNetwork();

public:

	struct Message {
		union {
			struct {
				uint8_t					networkName[30] = { 0 };	// ASCII null terminated string
				uint8_t					version = SAHOM_VERSION;	// Version of SaHoM that sender runs
				uint8_t					type;						// Type of payload
				uint16_t 				size;						// Length payload
				uint8_t					payload[];					// Payload
			} 						*structure;
			uint8_t					*raw;
		};
		uint16_t					nMessage;
	};

	typedef enum {
		MULTICAST_SIGN_IN_CHANNEL,
	} DESTINATION;

public:
	void						createMessage(struct Message *message, const void *payload, uint16_t size);
	void						freeMessage(struct Message *message);
	void						multicast(struct Message message, DESTINATION destination);
	void						flush();

private:
	struct						SendRequest {
		DESTINATION					destination;
		struct Message				message;
	};

	uint8_t 					m_networkName[30] = {'M', 'y', ' ', 'S', 'a', 'h', 'o', 'm', 0};
	std::queue<SendRequest *>	m_multicastBuffer;
};

#endif /* INC_SAHOMNETWORK_H_ */
