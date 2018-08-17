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
#include "baskettab.h"

#include <QLabel>
#include <QGridLayout>
#include <QSettings>


BasketTab::BasketTab(QWidget *parent)
    : QWidget(parent)
{
    QLabel *numTimeout1Label = new QLabel(tr("Timeouts on 1st Half:"));
    QLabel *numTimeout2Label = new QLabel(tr("Timeouts on 2nd Half:"));
    QLabel *numTimeout3Label = new QLabel(tr("Timeouts on Overtimes:"));
    QLabel *BonusTargetLabel = new QLabel(tr("Bonus Fauls:"));
    QLabel *GamePeriodsLabel = new QLabel(tr("Game Periods:"));
    QLabel *regularTimeLabel = new QLabel(tr("Regular Time [min]:"));
    QLabel *overTimeLabel    = new QLabel(tr("Over Time [min]:"));

    numTimeout1Edit.setMaxLength(1);
    numTimeout2Edit.setMaxLength(1);
    numTimeout3Edit.setMaxLength(1);
    BonusTargetEdit.setMaxLength(1);
    regularTimeEdit.setMaxLength(2);
    overTimeEdit.setMaxLength(1);

    auto *mainLayout = new QGridLayout;
    mainLayout->addWidget(numTimeout1Label,  0, 0, 1, 3);
    mainLayout->addWidget(&numTimeout1Edit,  0, 3, 1, 1);
    mainLayout->addWidget(numTimeout2Label,  1, 0, 1, 3);
    mainLayout->addWidget(&numTimeout2Edit,  1, 3, 1, 1);
    mainLayout->addWidget(numTimeout3Label,  2, 0, 1, 3);
    mainLayout->addWidget(&numTimeout3Edit,  2, 3, 1, 1);

    mainLayout->addWidget(GamePeriodsLabel,  3, 0, 1, 3);
    mainLayout->addWidget(&GamePeriodsEdit,  3, 3, 1, 1);
    mainLayout->addWidget(BonusTargetLabel,  4, 0, 1, 3);
    mainLayout->addWidget(&BonusTargetEdit,  4, 3, 1, 1);

    mainLayout->addWidget(regularTimeLabel, 5, 0, 1, 3);
    mainLayout->addWidget(&regularTimeEdit, 5, 3, 1, 1);
    mainLayout->addWidget(overTimeLabel,    6, 0, 1, 3);
    mainLayout->addWidget(&overTimeEdit,    6, 3, 1, 1);

    setLayout(mainLayout);
}


void
BasketTab::GetSettings() {
    pSettings = new QSettings("Gabriele Salvato", "Basket Parameters");
    numTimeout1Edit.setText(pSettings->value("basket/maxTimeout1", 2).toString());
    numTimeout2Edit.setText(pSettings->value("basket/maxTimeout2", 3).toString());
    numTimeout3Edit.setText(pSettings->value("basket/maxTimeout3", 1).toString());
    GamePeriodsEdit.setText(pSettings->value("basket/gamePeriods", 4).toString());
    BonusTargetEdit.setText(pSettings->value("basket/bonusTarget", 4).toString());
    regularTimeEdit.setText(pSettings->value("basket/regularTime", 10).toString());
    overTimeEdit.setText(pSettings->value("basket/overTime", 5).toString());
}


void
BasketTab::StoreSettings() {
    if(pSettings) {
        pSettings->setValue("basket/maxTimeout1", numTimeout1Edit.text().toInt());
        pSettings->setValue("basket/maxTimeout2", numTimeout2Edit.text().toInt());
        pSettings->setValue("basket/maxTimeout3", numTimeout3Edit.text().toInt());
        pSettings->setValue("basket/gamePeriods", GamePeriodsEdit.text().toInt());
        pSettings->setValue("basket/bonusTarget", BonusTargetEdit.text().toInt());
        pSettings->setValue("basket/regularTime", regularTimeEdit.text().toInt());
        pSettings->setValue("basket/overTime", overTimeEdit.text().toInt());
    }
}


int
BasketTab::getNumTimeout1() {
    return numTimeout1Edit.text().toInt();
}


int BasketTab::getNumTimeout2() {
    return numTimeout2Edit.text().toInt();
}


int
BasketTab::getNumTimeout3() {
    return numTimeout3Edit.text().toInt();
}


int
BasketTab::getGamePeriods() {
    return GamePeriodsEdit.text().toInt();
}


int BasketTab::getBonusTarget() {
    return BonusTargetEdit.text().toInt();
}


int
BasketTab::getRegularTime() {
    return regularTimeEdit.text().toInt();
}


int
BasketTab::getOverTime() {
    return overTimeEdit.text().toInt();
}
