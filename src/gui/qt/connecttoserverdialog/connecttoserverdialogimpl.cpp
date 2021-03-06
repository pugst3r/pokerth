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
#include "connecttoserverdialogimpl.h"
#include <net/socket_msg.h>

connectToServerDialogImpl::connectToServerDialogImpl(QWidget *parent)
	: QDialog(parent)
{
#ifdef __APPLE__
	setWindowModality(Qt::ApplicationModal);
	setWindowFlags(Qt::WindowSystemMenuHint | Qt::CustomizeWindowHint | Qt::Dialog);
#endif
	setupUi(this);
}

void connectToServerDialogImpl::exec()
{
	label_actionMessage->setText("");
	progressBar->setValue(0);

	QDialog::exec();
}

void connectToServerDialogImpl::refresh(int actionID)
{

	bool skip = false;
	switch (actionID) {

	case MSG_SOCK_INIT_DONE: {
		label_actionMessage->setText(tr("Resolving address..."));
	}
	break;
	case MSG_SOCK_SERVER_LIST_DONE : {
		label_actionMessage->setText(tr("Reading server list..."));
	}
	break;
	case MSG_SOCK_RESOLVE_DONE: {
		label_actionMessage->setText(tr("Connecting to server..."));
	}
	break;
	case MSG_SOCK_CONNECT_DONE: {
		label_actionMessage->setText(tr("Starting session..."));
	}
	break;
	case MSG_SOCK_SESSION_DONE: {
		label_actionMessage->setText(tr("Connection established!"));
	}
	break;
	default:
		skip = true;
	}

	if (!skip) {
		progressBar->setValue(actionID*(100/MSG_SOCK_LIMIT_CONNECT));

		if (actionID == MSG_SOCK_LIMIT_CONNECT)
			QTimer::singleShot(1000, this, SLOT(accept()));
	}
}

