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

#define MESSAGE_TYPE_UNDEFINED		0
#define MESSAGE_TYPE_STANDARD		1

namespace SahomNetwork {

struct CommonHeader {
	union {
		struct {
			uint8_t networkName[30] = { 0 }; 	// ASCII null terminated string
			uint8_t version = SAHOM_VERSION; 	// Version of SaHoM that sender runs
			uint8_t type;						// Type of payload
			uint16_t size;						// Length payload
			uint8_t payload[];					// Payload
		}*structure;
		uint8_t *raw;
	};
	uint16_t rawSize;
};

struct Message {
	union {
		struct {
		} *structure;
		uint8_t *raw;
	};
	uint16_t rawSize;
};

#define STANDARD_MESSAGE_COMMAND_NONE			0;
#define STANDARD_MESSAGE_COMMAND_HELLO			1;
#define STANDARD_MESSAGE_COMMAND_SIGN_IN		2;

struct StandardMessage {
	union {
		struct {
			uint16_t command;
			uint8_t nParameters;
			uint8_t parameters[0];
		}*structure;
		uint8_t *raw;
	};
	uint16_t rawSize;
};

} /* namespace SahomNetwork */

#endif /* INC_MESSAGES_H_ */
