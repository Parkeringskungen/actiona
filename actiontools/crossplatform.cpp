/*
	Actionaz
	Copyright (C) 2008-2010 Jonathan Mercier-Ganady

	Actionaz is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Actionaz is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.

	Contact : jmgr@jmgr.info
*/

#include "crossplatform.h"
#include "windowhandle.h"

#include <QWidget>
#include <QTime>
#ifndef Q_WS_MAC
#include <QxtWindowSystem>
#endif
#include <QDebug>
#include <QDir>

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <QX11Info>
#endif

#ifdef Q_WS_WIN
#include <windows.h>
#include <Tlhelp32.h>
#endif

namespace ActionTools
{
	QString CrossPlatform::mLastErrorString;
	int CrossPlatform::mLastError = -1;

	void CrossPlatform::setForegroundWindow(QWidget *window)
	{
#ifdef Q_WS_X11
		XRaiseWindow(QX11Info::display(), window->winId());
#endif
#ifdef Q_WS_WIN
		if(IsIconic(window->winId()))
			ShowWindow(window->winId(), SW_RESTORE);
		else
		{
			if(!SetForegroundWindow(window->winId()))
				setupLastError();

			if(!SetWindowPos(window->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE))
				setupLastError();
		}
#endif
	}

	bool CrossPlatform::killProcess(int id, KillProcessMode killMode, int timeout)
	{
#ifdef Q_WS_X11
		switch(killMode)
		{
		case Graceful:
			{
				int ret = kill(id, SIGTERM);

				if(ret != 0)
				{
					setupLastError();
					return false;
				}

				return true;
			}
		case Forceful:
			{
				int ret = kill(id, SIGKILL);

				if(ret != 0)
				{
					setupLastError();
					return false;
				}

				return true;
			}
		case GracefulThenForceful:
			{
				if(kill(id, SIGTERM) != 0)
				{
					setupLastError();
					return false;
				}

				QTime time;
				time.start();

				while(true)
				{
					if(processStatus(id) == Stopped)
						return true;

					if(time.elapsed() >= timeout)
					{
						int ret = kill(id, SIGKILL);
						if(ret != 0)
						{
							if(ret == ESRCH) //No such process
								return true;

							setupLastError();
							return false;
						}

						usleep(10000); //10 msec

						return (processStatus(id) == Stopped);
					}

					usleep(100000); //100 msec
				}
			}
		}

		return false;
#endif
#ifdef Q_WS_WIN
		HANDLE process = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, id);
		if(!process)
		{
			setupLastError();
			return false;
		}

		if(killMode == Forceful)
		{
			CloseHandle(process);
			bool ret = TerminateProcess(process, 0);
			if(!ret)
				setupLastError();

			return ret;
		}

		const WindowList &windows = QxtWindowSystem::windows();
		foreach(const WId wid, windows)
		{
			WindowHandle handle = wid;
			if(handle.processId() == id)
				handle.kill();
		}

		QTime time;
		time.start();

		DWORD exitCode;
		do
		{
			if(!GetExitCodeProcess(process, &exitCode))
			{
				CloseHandle(process);
				setupLastError();
				return false;
			}

			if(time.elapsed() >= timeout)
				break;

			Sleep(100);
		}
		while(exitCode == STILL_ACTIVE);

		if(exitCode == STILL_ACTIVE)
		{
			if(killMode == Graceful)
			{
				CloseHandle(process);
				setupLastError();
				return false;
			}

			if(!TerminateProcess(process, 0))
			{
				CloseHandle(process);
				setupLastError();
				return false;
			}
		}

		CloseHandle(process);

		return true;
#endif
	}

	CrossPlatform::ProcessStatus CrossPlatform::processStatus(int id)
	{
#ifdef Q_WS_X11
		return (kill(id, 0) == 0) ? Running : Stopped;
#endif
#ifdef Q_WS_WIN
		HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, id);
		if(process)
		{
			CloseHandle(process);
			return Running;
		}

		return Stopped;
#endif
	}

	QList<int> CrossPlatform::runningProcesses()
	{
#ifdef Q_WS_X11
		QDir procDir("/proc");
		QList<int> back;

		if(!procDir.exists())
			return back;

		QStringList processes = procDir.entryList(QDir::Dirs);
		foreach(const QString &processId, processes)
		{
			bool success;
			int id = processId.toInt(&success);

			if(success)
				back.append(id);
		}

		return back;
#endif
#ifdef Q_WS_WIN
		QList<int> back;
		HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(!handle)
		{
			setupLastError();
			return back;
		}

		PROCESSENTRY32 info = {0};
		info.dwSize = sizeof(PROCESSENTRY32);

		bool first = Process32First(handle, &info);
		if(!first)
		{
			CloseHandle(handle);
			setupLastError();
			return back;
		}

		do
		{
			back.append(info.th32ProcessID);
		}
		while(Process32Next(handle, &info));

		CloseHandle(handle);

		return back;
#endif
	}

	void CrossPlatform::setupLastError()
	{
#ifdef Q_WS_X11
		mLastError = errno;

		char *errorStr = strerror(errno);
		mLastErrorString = QString::fromUtf8(errorStr);
#endif
#ifdef Q_WS_WIN
		mLastError = GetLastError();
		LPTSTR message;

		if(!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					  0, mLastError, 0, (LPTSTR)&message, 0, 0))
		{
			qDebug() << "Error : " << mLastError << ", and error " << GetLastError() << " while trying to get the error message";

			mLastErrorString = QString();
			return;
		}

		mLastErrorString = QString::fromWCharArray(message).trimmed();

		LocalFree(message);
#endif

		qDebug() << "Error : " << mLastErrorString << " (" << mLastError << ")";
	}
}