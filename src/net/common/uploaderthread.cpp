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

#include <net/uploaderthread.h>
#include <net/uploadhelper.h>
#include <net/netexception.h>
#include <boost/filesystem.hpp>
#include <core/loghelper.h>

#define UPLOAD_DELAY_MSEC					100

using namespace std;
using namespace boost::filesystem;


UploaderThread::UploaderThread()
	: m_uploadInProgress(false)
{
	m_uploadHelper.reset(new UploadHelper());
}

UploaderThread::~UploaderThread()
{
}

void
UploaderThread::QueueUpload(const string &url, const string &user, const string &pwd, const string &filename, size_t filesize)
{
	boost::mutex::scoped_lock lock(m_uploadQueueMutex);
	m_uploadQueue.push(UploadData(url, user, pwd, filename, filesize));
}

void
UploaderThread::Main()
{
	while (!ShouldTerminate()) {
		try {
			if (m_uploadInProgress) {
				m_uploadInProgress = !m_uploadHelper->Process();
			}

			if (!m_uploadInProgress) {
				Msleep(UPLOAD_DELAY_MSEC);
				// The upload needs only local state, as no value needs to be returned.
				UploadData data;
				{
					boost::mutex::scoped_lock lock(m_uploadQueueMutex);
					if (!m_uploadQueue.empty()) {
						data = m_uploadQueue.front();
						m_uploadQueue.pop();
					}
				}
				if (!data.filename.empty() && data.filesize > 0) {
					path filepath(data.filename);
#if BOOST_FILESYSTEM_VERSION != 3
					m_uploadHelper->Init(data.address + filepath.leaf(), filepath.file_string(), data.user, data.pwd, data.filesize);
#else
					m_uploadHelper->Init(data.address + filepath.filename().string(), filepath.file_string(), data.user, data.pwd, data.filesize);
#endif
					m_uploadInProgress = true;
				}
			}
		} catch (const NetException &e) {
			LOG_ERROR("Upload failed: " << e.what());
			m_uploadInProgress = false;
		}
	}
}

