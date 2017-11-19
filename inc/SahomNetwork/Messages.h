/*
 * Messages.h
 *
 *  Created on: 22 okt. 2017
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

#ifndef INC_MESSAGES_H_
#define INC_MESSAGES_H_

#include <config.h>
#include <stdint.h>

#define MESSAGE_TYPE_UNDEFINED		0
#define MESSAGE_TYPE_STANDARD		1

namespace SahomNetwork {

struct DeviceUID {
	union {
		struct {
			uint8_t	size;
			uint8_t	UID[];
		}		*structure;
		uint8_t	*raw;
	};
	uint16_t rawSize = 0;
};

struct CommonHeader {
	union {
		struct {
			uint8_t		networkName[30] = { 0 }; 	// ASCII null terminated string
			uint8_t		version = SAHOM_VERSION; 	// Version of SaHoM that sender runs
			struct {
				uint8_t		all_networks	: 1;
				uint8_t		require_confirm : 1;
				uint8_t		received		: 1;
			} 			option = { 0 };
			uint8_t		type;						// Type of payload
			uint16_t	size;						// Length payload
			uint8_t		payload[];					// Payload

		}			*structure;
		uint8_t 	*raw;
	};
	uint16_t 	rawSize = 0;
	DeviceUID	*UID = nullptr;
};

struct Message {
	union {
		struct {
		} *structure;
		uint8_t *raw;
	};
	uint16_t rawSize = 0;
};

#define STANDARD_MESSAGE_COMMAND_NONE			0
#define STANDARD_MESSAGE_COMMAND_SCAN			1
#define STANDARD_MESSAGE_COMMAND_SIGN_IN		2
#define STANDARD_MESSAGE_COMMAND_WHO_IS			3

#define STANDARD_MESSAGE_COMMAND_SETTINGS		4

struct StandardMessage {
	union {
		struct {
			uint16_t 	command;
			uint8_t 	nParameters;
			uint8_t 	parameters[0];
		}			*structure;
		uint8_t 	*raw;
	};
	uint16_t rawSize = 0;
};

class Messages {
public:
	static bool		compareNetworkName(const uint8_t a[30], const uint8_t b[30]);
	static bool 	compareDeviceUIDS(struct DeviceUID &a, struct DeviceUID &b);

private:
					Messages();
	virtual 		~Messages();
};

} /* namespace SahomNetwork */


#endif /* INC_MESSAGES_H_ */
