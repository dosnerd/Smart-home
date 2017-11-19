/*
 * config.h
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

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

////////////GENERAL////////////
#define SAHOM_VERSION				0x00			//MAX 0xFF
#define ALLOW_LOGS					1
#define ALLOW_WARNINGS				1
#define ALLOW_ERRORS				1
#define GENERAL_BUFFER_SIZE			1024

////////////PORTS & CHANNELS////////////
#define SAHOM_PORT					59105
#define	SIGN_IN_CHANNEL				{ { { 0xFF,0x12,0,0,0,0,0,0,0,0,0,0,0,0,0xBE,0xEF } } }
#define	WHO_IS_CHANNEL				{ { { 0xFF,0x12,0,0,0,0,0,0,0,0,0,0,0,0,0xBE,0xEB } } }

////////////NETWORK SETTINGS////////////
#define NETWORK_STAY_LISTENING		0x1

////////////DISCOVERY SETTINGS////////////
#define WHO_IS_LIMIT				5

#endif /* INC_CONFIG_H_ */
