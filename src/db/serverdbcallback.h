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
/* Server database callback for async database operations. */

#ifndef _SERVERDBCALLBACK_H_
#define _SERVERDBCALLBACK_H_

#include <db/dbdefs.h>
#include <string>

// Callback operations are posted using the io service,
// and will therefore be executed in the io service thread.
class ServerDBCallback
{
public:
	virtual ~ServerDBCallback();

	virtual void ConnectSuccess() = 0;
	virtual void ConnectFailed(const std::string &error) = 0;

	virtual void QueryError(const std::string &error) = 0;

	virtual void PlayerLoginSuccess(unsigned requestId, const DBPlayerData &dbPlayerData) = 0;
	virtual void PlayerLoginFailed(unsigned requestId) = 0;
	virtual void PlayerLoginBlocked(unsigned requestId) = 0;

	virtual void AvatarIsBlacklisted(unsigned requestId) = 0;
	virtual void AvatarIsOK(unsigned requestId) = 0;

	virtual void CreateGameSuccess(unsigned requestId) = 0;
	virtual void CreateGameFailed(unsigned requestId) = 0;

	virtual void ReportAvatarSuccess(unsigned requestId, unsigned replyId) = 0;
	virtual void ReportAvatarFailed(unsigned requestId, unsigned replyId) = 0;

	virtual void ReportGameSuccess(unsigned requestId, unsigned replyId) = 0;
	virtual void ReportGameFailed(unsigned requestId, unsigned replyId) = 0;
};

#endif
