/*
 * SettingsHandler.h
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

#ifndef INC_SAHOMNETWORK_SETTINGSHANDLER_H_
#define INC_SAHOMNETWORK_SETTINGSHANDLER_H_

#include <SahomNetwork/Handlers.h>
#include <SahomNetwork/Network.h>

namespace SahomNetwork {

class SettingsHandler: public Handlers {
public:
						SettingsHandler();
						SettingsHandler(const SettingsHandler &other);
	virtual 			~SettingsHandler();

public:
	bool 				hasHandler(uint16_t command) override;
	void 				callHandler(const struct CommonHeader &header, struct in6_addr &addr) override;

	virtual Handlers	*clone() const override;

private:
	void				setNameHandler(StandardMessage &message);
	void				setConnectedHandler(StandardMessage &message);

	Network				*m_networkInstance;
};

} /* namespace SahomNetwork */

#endif /* INC_SAHOMNETWORK_SETTINGSHANDLER_H_ */
