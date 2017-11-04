/*
 * Log.h
 *
 *  Created on: 23 okt. 2017
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

#ifndef INC_LOG_H_
#define INC_LOG_H_

#include <config.h>

#if ALLOW_LOG || ALLOW_ERRORS
#include <iostream>
#endif

#if ALLOW_LOG
#define LOG(FROM, MESSAGE) 					\
	std::cout << "LOG (" << FROM << "):" << \
	std::endl << MESSAGE << std::endl << 	\
	std::endl
#else
#define LOG(FROM, MESSAGE)
#endif

#if ALLOW_ERRORS
#define ERROR(FROM, MESSAGE) 				\
	std::cerr << "ERROR (" << FROM << "):" <<	\
	std::endl << MESSAGE << std::endl << 	\
	std::endl
#else
#define ERROR(FROM, MESSAGE)
#endif

#endif /* INC_LOG_H_ */
