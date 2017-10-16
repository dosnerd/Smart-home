/*
 * Errors.h
 *
 *  Created on: 12 okt. 2017
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

#ifndef INC_ERRORS_H_
#define INC_ERRORS_H_


enum ERRORS{
	SOCKET_NOT_CREATED,
	SOCKET_BIND_FAILED,
	SOCKET_LISTEN_ERROR,
	SOCKET_ACCEPT_REFUSED,
	SOCKET_CONNECT_ERROR,
	SOCKET_SEND_ERROR,
	PARSE_IPv4_ADDRES_FAILED,
	SOCKET_SET_REUSE_FAILED,
	SOCKET_JOIN_CHANNEL,
};


#endif /* INC_ERRORS_H_ */
