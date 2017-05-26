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
#include <QGridLayout>
#include <QGroupBox>
#include <QDir>

#include "handballcontroller.h"
#include "fileserver.h"

#define MAX_TIMEOUTS  2 // Numero massimo di sospensioni
#define MAX_PERIODS   2 //
#define REGULAR_TIME 30 // 30 Minuti è la durata di un tempo regolare


HandballController::HandballController()
    : ScoreController(HANDBALL_PANEL, Q_NULLPTR)
{
    QString sFunctionName = QString(" HandballController::BasketController ");
    Q_UNUSED(sFunctionName)

    GetSettings();
    PrepareDirectories();
    pSlideUpdaterServer->setDir(sSlideDir,"*.jpg *.jpeg *.png");
    pSpotUpdaterServer->setDir(sSpotDir, "*.mp4");
    emit startSpotServer();
    emit startSlideServer();

    QGridLayout *mainLayout = new QGridLayout();
    int gamePanelWidth   = 15;
    int gamePanelHeight  = 13;
    int gameBoxHeight    = 3;
    int gameButtonHeight = 3;
    mainLayout->addLayout(CreateGamePanel(),
                          0,
                          0,
                          gamePanelHeight,
                          gamePanelWidth);

    mainLayout->addWidget(CreateGameBox(),
                          gamePanelHeight,
                          0,
                          gameBoxHeight,
                          gamePanelWidth);

    mainLayout->addWidget(CreateGameButtonBox(),
                          gamePanelHeight+gameBoxHeight,
                          0,
                          gameButtonHeight,
                          gamePanelWidth);

    mainLayout->addWidget(CreateSpotButtonBox(),
                          0,
                          gamePanelWidth,
                          gamePanelHeight+gameBoxHeight+gameButtonHeight,
                          2);
    setLayout(mainLayout);
}


void
HandballController::PrepareDirectories() {
    QString sFunctionName = QString(" HandballController::PrepareDirectories ");
    QDir slideDir(sSlideDir);
    QDir spotDir(sSpotDir);

    if(!slideDir.exists() || !spotDir.exists()) {
        onButtonSetupClicked();
        slideDir.setPath(sSlideDir);
        if(!slideDir.exists()) sSlideDir = QDir::homePath();
        spotDir.setPath(sSpotDir);
        if(!spotDir.exists()) sSpotDir = QDir::homePath();
        pSettings->setValue("directories/slides", sSlideDir);
        pSettings->setValue("directories/spots", sSpotDir);
    }
    else {
        QStringList filter(QStringList() << "*.jpg" << "*.jpeg" << "*.png");
        slideDir.setNameFilters(filter);
        slideList = slideDir.entryInfoList();
        logMessage(logFile,
                   sFunctionName,
                   QString("Slides directory: %1 Found %2 Slides")
                   .arg(sSlideDir)
                   .arg(slideList.count()));
        QStringList nameFilter(QStringList() << "*.mp4");
        spotDir.setNameFilters(nameFilter);
        spotDir.setFilter(QDir::Files);
        spotList = spotDir.entryInfoList();
        logMessage(logFile,
                   sFunctionName,
                   QString("Spot directory: %1 Found %2 Spots")
                   .arg(sSpotDir)
                   .arg(spotList.count()));
    }
    if(!sSlideDir.endsWith(QString("/"))) sSlideDir+= QString("/");
    if(!sSpotDir.endsWith(QString("/")))  sSpotDir+= QString("/");
}


void
HandballController::GetSettings() {
}


void
HandballController::closeEvent(QCloseEvent *event) {
    QString sFunctionName = " HandballController::closeEvent ";
    Q_UNUSED(sFunctionName)
    SaveStatus();
    ScoreController::closeEvent(event);// Propagate the event
}


void
HandballController::SaveStatus() {
}


QGroupBox*
HandballController::CreateTeamBox(int iTeam) {
    QGroupBox* teamBox      = new QGroupBox();
    return teamBox;
}


QGroupBox*
HandballController::CreateGameBox() {
    QGroupBox* gameBox      = new QGroupBox();
    return gameBox;
}


QGroupBox*
HandballController::CreateGameButtonBox() {
    QGroupBox* gameButtonBox = new QGroupBox();
    return gameButtonBox;
}


QGridLayout*
HandballController::CreateGamePanel() {
    QGridLayout* gamePanel = new QGridLayout();
    gamePanel->addWidget(CreateTeamBox(0),      0, 0, 1, 1);
    gamePanel->addWidget(CreateTeamBox(1),      0, 1, 1, 1);
    return gamePanel;
}


QString
HandballController::FormatStatusMsg() {
}



// =========================
// Event management routines
// =========================


void
HandballController::onTimeOutIncrement(int iTeam) {
}


void
HandballController::onTimeOutDecrement(int iTeam) {
}


void
HandballController::onScoreIncrement(int iTeam) {
}


void
HandballController::onScoreDecrement(int iTeam) {
}


void
HandballController::onTeamTextChanged(QString sText, int iTeam) {
}


void
HandballController::onButtonNewPeriodClicked() {
}


void
HandballController::onButtonNewGameClicked() {
}


void
HandballController::onPeriodIncrement(int iDummy) {
}


void
HandballController::onPeriodDecrement(int iDummy) {
}


