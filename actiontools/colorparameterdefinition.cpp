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

#include "colorparameterdefinition.h"
#include "subparameter.h"
#include "coloredit.h"
#include "action.h"

namespace ActionTools
{
	ColorParameterDefinition::ColorParameterDefinition(Category category, const QString &name, const QString &translatedName, QObject *parent)
		: ParameterDefinition(category, name, translatedName, parent),
		mColorEdit(0)
	{
	}

	void ColorParameterDefinition::buildEditors(Script *script, QWidget *parent)
	{
		ParameterDefinition::buildEditors(script, parent);

		mColorEdit = new ColorEdit(parent);
		mColorEdit->setObjectName("value");

		addEditor(mColorEdit);
	}

	void ColorParameterDefinition::load(const Action *action)
	{
		mColorEdit->setFromSubParameter(action->subParameter(name(), "value"));
	}

	void ColorParameterDefinition::save(Action *action)
	{
		action->setSubParameter(name(), "value", mColorEdit->isCode(), mColorEdit->text());
	}

	void ColorParameterDefinition::setDefaultValues(Parameter &data)
	{
		data.subParameters()["value"].setValue(option("default"));
	}
}