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

#ifndef NOTIFYDEFINITION_H
#define NOTIFYDEFINITION_H

#include "actiondefinition.h"
#include "actions/notifyinstance.h"
#include "textparameterdefinition.h"
#include "numberparameterdefinition.h"
#include "fileparameterdefinition.h"

namespace ActionTools
{
	class ActionPack;
	class ActionInstance;
}

namespace Actions
{
	class NotifyDefinition : public QObject, public ActionTools::ActionDefinition
	{
	   Q_OBJECT

	public:
		explicit NotifyDefinition(ActionTools::ActionPack *pack)
		: ActionDefinition(pack)
		{
			ActionTools::TextParameterDefinition *title = new ActionTools::TextParameterDefinition("title", tr("Title"), this);
			title->setTooltip(tr("The notification title"));
			addElement(title);

			ActionTools::TextParameterDefinition *text = new ActionTools::TextParameterDefinition("text", tr("Text"), this);
			text->setTooltip(tr("The notification text"));
			addElement(text);

			ActionTools::NumberParameterDefinition *timeout = new ActionTools::NumberParameterDefinition("timeout", tr("Timeout"), this);
			timeout->setTooltip(tr("The notification timeout"));
			timeout->setMinimum(0);
			timeout->setMaximum(INT_MAX);
			timeout->setDefaultValue(3000);
			addElement(timeout, 1);

			ActionTools::FileParameterDefinition *icon = new ActionTools::FileParameterDefinition("icon", tr("Icon"), this);
			icon->setTooltip(tr("The notification icon"));
			addElement(icon, 1);

			addException(NotifyInstance::UnableToShowNotificationException, tr("Show notification failure"));
		}

		QString name() const													{ return QObject::tr("Notify"); }
		QString id() const														{ return "ActionNotify"; }
		Flag flags() const														{ return WorksOnGnuLinux | Official; }
		QString description() const												{ return QObject::tr("Shows a message using the notify system"); }
		ActionTools::ActionInstance *newActionInstance() const					{ return new NotifyInstance(this); }
		Category category() const												{ return System; }
		QPixmap icon() const													{ return QPixmap(":/icons/clipboard.png"); }
		QStringList tabs() const												{ return ActionDefinition::StandardTabs; }

	private:
		Q_DISABLE_COPY(NotifyDefinition)
	};

	#endif // NOTIFYDEFINITION_H
}