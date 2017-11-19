/*
 * Messages.cpp
 *
 *  Created on: 19 nov. 2017
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

#include "SahomNetwork/Messages.h"

namespace SahomNetwork {

bool Messages::compareDeviceUIDS(DeviceUID &a, DeviceUID &b) {
	uint8_t i;

	//Device UID should always as small as possible, meaning if size isn't same, UID can not be the same
	if (a.structure->size != b.structure->size) {
		return false;
	}

	for (i = 0; i < a.structure->size; i++) {
		if (a.structure->UID[i] != b.structure->UID[i])
			return false;
	}

	return true;
}

bool Messages::compareNetworkName(const uint8_t a[30], const uint8_t b[30]) {
	uint8_t i;
	for (i = 0; i < 30; ++i) {
		if (a[i] != b[i])
			return false;

		if (a[i] == '\0')
			return true;
	}

	return false;
}

} /* namespace SahomNetwork */
