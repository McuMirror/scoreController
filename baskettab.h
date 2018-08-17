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
#ifndef BASKETTAB_H
#define BASKETTAB_H

#include <QWidget>
#include <QLineEdit>

QT_FORWARD_DECLARE_CLASS(QSettings)


class BasketTab : public QWidget
{
    Q_OBJECT
public:
    explicit BasketTab(QWidget *parent = nullptr);
    void GetSettings();
    void StoreSettings();
    int getNumTimeout1();
    int getNumTimeout2();
    int getNumTimeout3();
    int getGamePeriods();
    int getBonusTarget();
    int getRegularTime();
    int getOverTime();

signals:

public slots:

private:
    QSettings *pSettings;
    QLineEdit numTimeout1Edit;
    QLineEdit numTimeout2Edit;
    QLineEdit numTimeout3Edit;
    QLineEdit BonusTargetEdit;
    QLineEdit GamePeriodsEdit;
    QLineEdit regularTimeEdit;
    QLineEdit overTimeEdit;
};

#endif // BASKETTAB_H
