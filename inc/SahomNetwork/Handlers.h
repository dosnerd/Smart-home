/*
 * Handler.h
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

#ifndef INC_SAHOMNETWORK_HANDLERS_H_
#define INC_SAHOMNETWORK_HANDLERS_H_

#include <stdint.h>
#include "Messages.h"

namespace SahomNetwork {

class Handlers {
public:
						Handlers();
	virtual 			~Handlers();

public:
	virtual bool		hasHandler(uint16_t command) = 0;
	virtual void		callHandler(struct CommonHeader &header) = 0;

	virtual Handlers	*clone() const = 0;
};

} /* namespace SahomNetwork */

#endif /* INC_SAHOMNETWORK_HANDLERS_H_ */
