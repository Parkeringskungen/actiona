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

#ifndef CROSSPLATFORM_H
#define CROSSPLATFORM_H

#include "actiontools_global.h"

#include <QString>
#include <QList>

class QWidget;

namespace ActionTools
{
	class ACTIONTOOLSSHARED_EXPORT CrossPlatform
	{
	public:
		enum KillProcessMode
		{
			Graceful,
			Forceful,
			GracefulThenForceful
		};
		enum ProcessStatus
		{
			Running,
			Stopped
		};

		//Windows
		static void setForegroundWindow(QWidget *window);

		//Processes
		static bool killProcess(int id, KillProcessMode killMode = GracefulThenForceful, int timeout = 3000);
		static ProcessStatus processStatus(int id);
		static QList<int> runningProcesses();

		static int lastError()						{ return mLastError; }
		static QString lastErrorString()			{ return mLastErrorString; }

	private:
		static void setupLastError();

		static QString mLastErrorString;
		static int mLastError;
	};
}

#endif // CROSSPLATFORM_H