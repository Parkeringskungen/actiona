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

#include "parameterdefinition.h"

#include <QWidget>

namespace ActionTools
{
	ParameterDefinition::ParameterDefinition(Category category, const QString &name, const QString &translatedName, QObject *parent)
		: ElementDefinition(category, name, translatedName, parent)
	{
	}

	void ParameterDefinition::buildEditors(Script *script, QWidget *parent)
	{
		Q_UNUSED(script)
		Q_UNUSED(parent)

		mEditors.clear();
	}

	QVariant ParameterDefinition::option(QString name, QVariant defaultValue) const
	{
		return mOptions.value(name, defaultValue);
	}

	void ParameterDefinition::addEditor(QWidget *editor)
	{
		editor->setToolTip(tooltip());

		mEditors.append(editor);
	}
}