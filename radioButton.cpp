/*
 *
Copyright (C) 2016  Gabriele Salvato

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <QtWidgets>

#include "radioButton.h"


/*!
 * \brief RadioButton::RadioButton
 * \param text
 * \param team
 * \param parent
 */
RadioButton::RadioButton(const QString &text, int team, QWidget *parent)
    : QRadioButton(parent)
    , myTeam(team)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setText(text);
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
}


/*!
 * \brief RadioButton::onClicked
 * \param bChecked
 */
void
RadioButton::onClicked(bool bChecked) {
    emit buttonClicked(myTeam, bChecked);
}
