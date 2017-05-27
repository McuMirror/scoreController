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
#include <QLabel>
#include <QGuiApplication>
#include <QScreen>
#include <QPushButton>

#include "handballcontroller.h"
#include "fileserver.h"
#include "edit.h"
#include "button.h"

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
    QString sString;
    QGridLayout* teamLayout = new QGridLayout();
    QLabel* labelSpacer = new QLabel(QString(""));

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width();
    int rW;

    // Team
    int nextRow = 0;
    teamName[iTeam] = new Edit(sTeam[iTeam], iTeam);
    teamName[iTeam]->setAlignment(Qt::AlignHCenter);
    teamName[iTeam]->setMaxLength(15);
    connect(teamName[iTeam], SIGNAL(textChanged(QString, int)),
            this, SLOT(onTeamTextChanged(QString, int)));

    QFont font(teamName[iTeam]->font());
    int iTeamFontSize = font.pointSize();
    for(int i=iTeamFontSize; i<100; i++) {
        font.setPointSize(i);
        QFontMetrics f(font);
        rW = teamName[iTeam]->maxLength()*f.width("W");
        if(rW > width/3) {
            iTeamFontSize = i-1;
            break;
        }
    }
    font.setPointSize(iTeamFontSize);
    teamName[iTeam]->setFont(font);
    teamLayout->addWidget(teamName[iTeam], nextRow, 0, 2, 10);
    nextRow += 2;

    teamLayout->addWidget(labelSpacer, nextRow, 0, 1, 10);
    nextRow += 1;

    // Timeout
    QLabel *timeoutLabel;
    timeoutLabel = new QLabel(tr("Timeout"));
    timeoutLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);

    font = timeoutLabel->font();
    int iTimeoutLabelFontSize = font.pointSize();
    for(int i=iTimeoutLabelFontSize; i<100; i++) {
        font.setPointSize(i);
        QFontMetrics f(font);
        rW = f.width(timeoutLabel->text());
        if(rW > width/10) {
            iTimeoutLabelFontSize = i-1;
            break;
        }
    }
    font.setPointSize(iTimeoutLabelFontSize);
    timeoutLabel->setFont(font);

    sString.sprintf("%1d", iTimeout[iTeam]);
    timeoutEdit[iTeam] = new Edit(sString);
    timeoutEdit[iTeam]->setMaxLength(1);
    timeoutEdit[iTeam]->setAlignment(Qt::AlignHCenter);
    timeoutEdit[iTeam]->setReadOnly(true);

    font = timeoutEdit[iTeam]->font();
    font.setPointSize(iTimeoutLabelFontSize);
    timeoutEdit[iTeam]->setFont(font);

    timeoutIncrement[iTeam] = new Button(tr("+"), iTeam);
    timeoutDecrement[iTeam] = new Button(tr("-"), iTeam);

    connect(timeoutIncrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onTimeOutIncrement(int)));
    connect(timeoutIncrement[iTeam], SIGNAL(clicked()),
            &buttonClick, SLOT(play()));
    connect(timeoutDecrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onTimeOutDecrement(int)));
    connect(timeoutDecrement[iTeam], SIGNAL(clicked()),
            &buttonClick, SLOT(play()));

    if(iTimeout[iTeam] == 0)
        timeoutDecrement[iTeam]->setEnabled(false);

    teamLayout->addWidget(timeoutLabel,            nextRow, 0, 2, 3, Qt::AlignRight|Qt::AlignVCenter);
    teamLayout->addWidget(timeoutDecrement[iTeam], nextRow, 3, 2, 2, Qt::AlignRight);
    teamLayout->addWidget(timeoutEdit[iTeam],      nextRow, 5, 2, 3, Qt::AlignHCenter|Qt::AlignVCenter);
    teamLayout->addWidget(timeoutIncrement[iTeam], nextRow, 8, 2, 2, Qt::AlignLeft);
    nextRow+= 2;
    teamLayout->addWidget(labelSpacer, nextRow, 0, 1, 10);
    nextRow += 1;

    // Score
    QLabel *scoreLabel;
    scoreLabel = new QLabel(tr("Score"));
    scoreLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);

    font = scoreLabel->font();
    font.setPointSize(iTimeoutLabelFontSize);
    scoreLabel->setFont(font);

    scoreEdit[iTeam] = new Edit();
    scoreEdit[iTeam]->setMaxLength(3);
    scoreEdit[iTeam]->setReadOnly(true);
    scoreEdit[iTeam]->setAlignment(Qt::AlignRight);
    sString.sprintf("%3d", iScore[iTeam]);
    scoreEdit[iTeam]->setText(sString);

    font = scoreEdit[iTeam]->font();
    font.setPointSize(iTimeoutLabelFontSize);
    scoreEdit[iTeam]->setFont(font);

    scoreIncrement[iTeam] = new Button(tr("+"), iTeam);
    scoreDecrement[iTeam] = new Button(tr("-"), iTeam);

    connect(scoreIncrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onScoreIncrement(int)));
    connect(scoreIncrement[iTeam], SIGNAL(clicked()),
            &buttonClick, SLOT(play()));
    connect(scoreDecrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onScoreDecrement(int)));
    connect(scoreDecrement[iTeam], SIGNAL(clicked()),
            &buttonClick, SLOT(play()));

    if(iScore[iTeam] == 0)
        scoreDecrement[iTeam]->setEnabled(false);

    teamLayout->addWidget(scoreLabel,            nextRow, 0, 2, 3, Qt::AlignRight|Qt::AlignVCenter);
    teamLayout->addWidget(scoreDecrement[iTeam], nextRow, 3, 2, 2, Qt::AlignRight);
    teamLayout->addWidget(scoreEdit[iTeam],      nextRow, 5, 2, 3, Qt::AlignHCenter|Qt::AlignVCenter);
    teamLayout->addWidget(scoreIncrement[iTeam], nextRow, 8, 2, 2, Qt::AlignLeft);

    teamBox->setLayout(teamLayout);
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
    QHBoxLayout* gameButtonLayout = new QHBoxLayout();

    newPeriodButton   = new QPushButton(tr("Nuovo\nPeriodo"));
    newGameButton     = new QPushButton(tr("Nuova\nPartita"));
    changeFieldButton = new QPushButton(tr("Cambio\nCampo"));

    connect(newPeriodButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonNewPeriodClicked()));
    connect(newPeriodButton, SIGNAL(clicked()),
            &buttonClick, SLOT(play()));
    connect(newGameButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonNewGameClicked()));
    connect(newGameButton, SIGNAL(clicked()),
            &buttonClick, SLOT(play()));
    connect(changeFieldButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonChangeFieldClicked()));
    connect(changeFieldButton, SIGNAL(clicked()),
            &buttonClick, SLOT(play()));

    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(newPeriodButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(newGameButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(changeFieldButton);
    gameButtonLayout->addStretch();
    gameButtonBox->setLayout(gameButtonLayout);
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


