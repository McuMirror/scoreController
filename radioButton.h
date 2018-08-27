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

#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include <QRadioButton>

class RadioButton : public QRadioButton
{
    Q_OBJECT

public:
    explicit RadioButton(const QString &text, int team, QWidget *parent = 0);

public slots:
    void onClicked(bool bChecked);

signals:
    void buttonClicked(int iTeam, bool bChecked);

protected:
    int myTeam;

};

#endif
