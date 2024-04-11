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
#include "paneltab.h"
//#include "utility.h"

#include <QGridLayout>


PanelTab::PanelTab(QWidget *parent)
    : QWidget(parent)
{
    labelDirection.setText(tr("Orientamento"));
    directionCombo.addItem(tr("Normale"));
    directionCombo.addItem(tr("Riflesso"));
    directionCombo.setDisabled(true);
    connect(&directionCombo, SIGNAL(currentTextChanged(QString)),
            this, SLOT(onChangePanelDirection(QString)));

    scoreOnlyCheckBox.setText(tr("Mostra solo il punteggio"));
    scoreOnlyCheckBox.setCheckState(Qt::Unchecked);
    scoreOnlyCheckBox.setDisabled(true);
    connect(&scoreOnlyCheckBox, SIGNAL(checkStateChanged(Qt::CheckState)),
            this, SLOT(onChangeScoreOnly(Qt::CheckState)));

    auto *mainLayout = new QGridLayout;
    mainLayout->addWidget(&labelDirection, 0, 0, 1, 2);
    mainLayout->addWidget(&directionCombo, 0, 2, 1, 3);
    mainLayout->addWidget(&scoreOnlyCheckBox, 1, 1, 1, 3);
    setLayout(mainLayout);
}


void
PanelTab::setDirection(PanelDirection newDirection) {
    directionCombo.setEnabled(true);
    if(newDirection == PanelDirection::Normal) {
        directionCombo.setCurrentText(tr("Normale"));
    }
    else {
        directionCombo.setCurrentText(tr("Riflesso"));
    }
}


void
PanelTab::setScoreOnly(bool bScoreOnly) {
    scoreOnlyCheckBox.setEnabled(true);
    if(bScoreOnly)
        scoreOnlyCheckBox.setCheckState(Qt::Checked);
    else
        scoreOnlyCheckBox.setCheckState(Qt::Unchecked);
}


void
PanelTab::onChangePanelDirection(QString sDirection) {
    PanelDirection newDirection = PanelDirection::Normal;
    if(sDirection == tr("Riflesso"))
        newDirection = PanelDirection::Reflected;
    emit changeDirection(newDirection);
}


void
PanelTab::onChangeScoreOnly(Qt::CheckState newState) {
    #ifdef LOG_VERBOSE
        logMessage(Q_NULLPTR,
                   Q_FUNC_INFO,
                   QString("ScoreOnly: %2")
                   .arg(newState));
    #endif
    if(newState == Qt::Unchecked)
        emit changeScoreOnly(false);
    else
        emit changeScoreOnly(true);
}

