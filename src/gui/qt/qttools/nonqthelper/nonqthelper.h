/***************************************************************************
 *   Copyright (C) 2007 by Lothar May                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef NONQTHELPER_H
#define NONQTHELPER_H

#ifndef POKERTH_DEDICATED_SERVER
#error This file is only for the server.
#endif

#include <string>

class NonQtHelper
{

public:
	NonQtHelper();

	~NonQtHelper();

	std::string stringToUtf8(const std::string &);
	std::string stringFromUtf8(const std::string &);
	std::string getDefaultLanguage();

	std::string getDataPathStdString(const char *argv0);
};


#endif
