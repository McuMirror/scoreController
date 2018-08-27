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
#include "panelconfigurator.h"
#include "utility.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>


/*!
 * \brief PanelConfigurator::PanelConfigurator
 * A Dialog to configure some Panel behaviours
 * \param parent
 */
PanelConfigurator::PanelConfigurator(QWidget *parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;

    pPanelTab = new PanelTab(this);
    connect(pPanelTab, SIGNAL(changeDirection(PanelDirection)),
            this, SLOT(onChangeDirection(PanelDirection)));
    connect(pPanelTab, SIGNAL(changeScoreOnly(bool)),
            this, SLOT(onChangeScoreOnly(bool)));

    pCameraTab = new CameraTab(this);
    connect(pCameraTab, SIGNAL(newTiltValue(int)),
            this, SLOT(onChangeTiltValue(int)));
    connect(pCameraTab, SIGNAL(newPanValue(int)),
            this, SLOT(onChangePanValue(int)));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    tabWidget->addTab(pPanelTab,  tr("Controllo Pannello"));
    iCameraTab = tabWidget->addTab(pCameraTab, tr("Controllo Telecamera"));
    connect(tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(onChangedTab(int)));

    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}


void
PanelConfigurator::setClient(QString sClient) {
    sCurrentClient = sClient;
}


void
PanelConfigurator::SetCurrentPanTilt(int newPan, int newTilt) {
    pCameraTab->SetCurrentPanTilt(newPan, newTilt);
}


void
PanelConfigurator::SetCurrrentOrientaton(PanelDirection newDirection) {
    pPanelTab->setDirection(newDirection);
}


void
PanelConfigurator::SetIsScoreOnly(bool bScoreOnly) {
    pPanelTab->setScoreOnly(bScoreOnly);
}


void
PanelConfigurator::onChangedTab(int iTabIndex) {
    if(iTabIndex == iCameraTab)
        emit startCamera();
    else
        emit stopCamera();
}


void
PanelConfigurator::onChangeDirection(PanelDirection newDirection) {
    emit changeDirection(newDirection);
}


void
PanelConfigurator::onChangeScoreOnly(bool bScoreOnly) {
    #ifdef LOG_VERBOSE
        logMessage(Q_NULLPTR,
                   Q_FUNC_INFO,
                   QString("ScoreOnly: %2")
                   .arg(bScoreOnly));
    #endif
    emit changeScoreOnly(bScoreOnly);
}


void
PanelConfigurator::onChangeTiltValue(int newValue) {
    emit newTiltValue(newValue);

}


void
PanelConfigurator::onChangePanValue(int newValue) {
    emit newPanValue(newValue);
}


