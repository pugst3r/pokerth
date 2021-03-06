/*****************************************************************************
 * PokerTH - The open source texas holdem engine                             *
 * Copyright (C) 2006-2011 Felix Hammer, Florian Thauer, Lothar May          *
 *                                                                           *
 * This program is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU Affero General Public License as            *
 * published by the Free Software Foundation, either version 3 of the        *
 * License, or (at your option) any later version.                           *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU Affero General Public License for more details.                       *
 *                                                                           *
 * You should have received a copy of the GNU Affero General Public License  *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 *****************************************************************************/
#ifndef QTHELPER_H
#define QTHELPER_H

#ifdef POKERTH_DEDICATED_SERVER
#error This file is not for the server.
#endif

#include <string>
/**
	@author FThauer FHammer <webmaster@pokerth.net>
*/
class QtHelper
{

public:
	QtHelper();

	~QtHelper();

	std::string stringToUtf8(const std::string &);
	std::string stringFromUtf8(const std::string &);
	std::string getDefaultLanguage();

	std::string getDataPathStdString(const char *argv0);
};

#endif
