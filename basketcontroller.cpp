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
#include <QSettings>
#include <QDir>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>

#include "basketcontroller.h"
#include "utility.h"
#include "edit.h"
#include "button.h"
#include "radioButton.h"
#include "fileserver.h"


#define MAX_TIMEOUTS  3
#define MAX_FAULS    99 // Da definire il comportamento dopo il numer max di falli !
#define MAX_PERIODS  99 // Da definire il comportamento dopo il numer max di periodi !
#define BONUS_TARGET  5 // After this value the Bonus is triggered for the team


BasketController::BasketController()
    : ScoreController(BASKET_PANEL, Q_NULLPTR)
{
    QString sFunctionName = QString(" BasketController::BasketController ");
    GetSettings();

    QDir slideDir(sSlideDir);
    QDir spotDir(sSpotDir);
    if(!slideDir.exists() || !spotDir.exists()) {
        onButtonSetupClicked();
    }
    else {
        QStringList filter(QStringList() << "*.jpg" << "*.jpeg" << "*.png");
        slideDir.setNameFilters(filter);
        slideList = slideDir.entryList();
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

    pFileUpdaterServer->setDirs(sSlideDir, sSpotDir);
    emit startFileServer();

    QGridLayout *mainLayout = new QGridLayout();

    int gamePanelWidth  = 15;
    int gamePanelHeigth =  8;
    mainLayout->addLayout(CreateGamePanel(),     0,                  0,              gamePanelHeigth,   gamePanelWidth);
    mainLayout->addWidget(CreateGameBox(),       gamePanelHeigth,    0,              5,                 gamePanelWidth);
    mainLayout->addWidget(CreateGameButtonBox(), gamePanelHeigth+5,  0,              1,                 gamePanelWidth);
    mainLayout->addWidget(CreateSpotButtonBox(), 0,                  gamePanelWidth, gamePanelHeigth+1, 1);
    setLayout(mainLayout);

    possess[iPossess ? 1 : 0]->setChecked(true);
    possess[iPossess ? 0 : 1]->setChecked(false);
}


void
BasketController::GetSettings() {
    QString sFunctionName = QString(" BasketController::GetSettings ");
    Q_UNUSED(sFunctionName)
    pSettings = new QSettings("Gabriele Salvato", "Basket Controller");

    sTeam[0]    = pSettings->value("team1/name", QString("Locali")).toString();
    sTeam[1]    = pSettings->value("team2/name", QString("Ospiti")).toString();
    iTimeout[0] = pSettings->value("team1/timeouts", 0).toInt();
    iTimeout[1] = pSettings->value("team2/timeouts", 0).toInt();
    iScore[0]   = pSettings->value("team1/score", 0).toInt();
    iScore[1]   = pSettings->value("team2/score", 0).toInt();
    iFauls[0]   = pSettings->value("team1/fauls", 0).toInt();
    iFauls[1]   = pSettings->value("team2/fauls", 0).toInt();

    iPeriod     = pSettings->value("game/period", 0).toInt();
    iPossess    = pSettings->value("game/possess", 0).toInt();

    sSlideDir   = pSettings->value("directories/slides", sSlideDir).toString();
    sSpotDir    = pSettings->value("directories/spots", sSpotDir).toString();

    for(int iTeam=0; iTeam<2; iTeam++) {
        if(iFauls[iTeam] >= BONUS_TARGET) {
            iBonus[iTeam] = 1;
        }
        else {
            iBonus[iTeam] = 0;
        }
    }
}


void
BasketController::closeEvent(QCloseEvent *event) {
    QString sFunctionName = " BasketController::closeEvent ";
    Q_UNUSED(sFunctionName)
    SaveStatus();
    ScoreController::closeEvent(event);// Propagate the event
}


void
BasketController::SaveStatus() {
    pSettings->setValue("team1/name", sTeam[0]);
    pSettings->setValue("team2/name", sTeam[1]);
    pSettings->setValue("team1/timeouts", iTimeout[0]);
    pSettings->setValue("team2/timeouts", iTimeout[1]);
    pSettings->setValue("team1/score", iScore[0]);
    pSettings->setValue("team2/score", iScore[1]);
    pSettings->setValue("team1/fauls", iFauls[0]);
    pSettings->setValue("team2/fauls", iFauls[1]);

    pSettings->setValue("game/period", iPeriod);
    pSettings->setValue("game/possess", iPossess);

    pSettings->setValue("directories/slides", sSlideDir);
    pSettings->setValue("directories/spots", sSpotDir);
}


QGroupBox*
BasketController::CreateTeamBox(int iTeam) {
    QString sString;
    QGroupBox* teamBox      = new QGroupBox();
    QGridLayout* teamLayout = new QGridLayout();

    // Team
    teamName[iTeam] = new Edit(sTeam[iTeam], iTeam);
    teamName[iTeam]->setAlignment(Qt::AlignHCenter);
    teamName[iTeam]->setMaxLength(15);
    connect(teamName[iTeam], SIGNAL(textChanged(QString, int)),
            this, SLOT(onTeamTextChanged(QString, int)));
    teamLayout->addWidget(teamName[iTeam], 0, 0, 1, 10);

    // Timeout
    QLabel *timeoutLabel;
    timeoutLabel = new QLabel(tr("Timeout"));
    timeoutLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);

    sString.sprintf("%1d", iTimeout[iTeam]);
    timeoutEdit[iTeam] = new Edit(sString);
    timeoutEdit[iTeam]->setMaxLength(1);
    timeoutEdit[iTeam]->setAlignment(Qt::AlignHCenter);
    timeoutEdit[iTeam]->setReadOnly(true);

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
    if(iTimeout[iTeam] == MAX_TIMEOUTS) {
        timeoutIncrement[iTeam]->setEnabled(false);
        timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }

    teamLayout->addWidget(timeoutLabel,            2, 0, 3, 2, Qt::AlignRight|Qt::AlignVCenter);
    teamLayout->addWidget(timeoutEdit[iTeam],      2, 2, 3, 6, Qt::AlignHCenter|Qt::AlignVCenter);
    teamLayout->addWidget(timeoutIncrement[iTeam], 1, 8, 2, 3, Qt::AlignLeft);
    teamLayout->addWidget(timeoutDecrement[iTeam], 4, 8, 2, 3, Qt::AlignLeft);

    // Team Fauls
    QLabel *faulsLabel;
    faulsLabel = new QLabel(tr("Team Fauls"));
    faulsLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);
    sString.sprintf("%1d", iFauls[iTeam]);

    faulsEdit[iTeam] = new Edit(sString);
    faulsEdit[iTeam]->setMaxLength(2);
    faulsEdit[iTeam]->setAlignment(Qt::AlignHCenter);
    faulsEdit[iTeam]->setReadOnly(true);

    faulsIncrement[iTeam] = new Button(tr("+"), iTeam);
    faulsDecrement[iTeam] = new Button(tr("-"), iTeam);

    connect(faulsIncrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onFaulsIncrement(int)));
    connect(faulsIncrement[iTeam], SIGNAL(clicked()),
            &buttonClick, SLOT(play()));
    connect(faulsDecrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onFaulsDecrement(int)));
    connect(faulsDecrement[iTeam], SIGNAL(clicked()),
            &buttonClick, SLOT(play()));

    if(iFauls[iTeam] == 0)
        faulsDecrement[iTeam]->setEnabled(false);
    if(iFauls[iTeam] == MAX_FAULS)
        faulsIncrement[iTeam]->setEnabled(false);

    teamLayout->addWidget(faulsLabel,            7, 0, 3, 2, Qt::AlignRight|Qt::AlignVCenter);
    teamLayout->addWidget(faulsEdit[iTeam],      7, 2, 3, 6, Qt::AlignHCenter|Qt::AlignVCenter);
    teamLayout->addWidget(faulsIncrement[iTeam], 6, 8, 2, 2, Qt::AlignLeft);
    teamLayout->addWidget(faulsDecrement[iTeam], 9, 8, 2, 2, Qt::AlignLeft);

    // Possess
    possess[iTeam] = new RadioButton(tr("Possess"), iTeam);
    if(iTeam == 0) {
        teamLayout->addWidget(possess[iTeam],   11, 4, 1, 4, Qt::AlignLeft|Qt::AlignVCenter);
    } else {
        teamLayout->addWidget(possess[iTeam],   11, 4, 1, 4, Qt::AlignLeft|Qt::AlignVCenter);
    }
    connect(possess[iTeam], SIGNAL(buttonClicked(int, bool)), this, SLOT(onPossessClicked(int, bool)));

    // Score
    QLabel *scoreLabel;
    scoreLabel = new QLabel(tr("Score"));
    scoreLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);

    scoreEdit[iTeam] = new Edit();
    scoreEdit[iTeam]->setMaxLength(3);
    scoreEdit[iTeam]->setReadOnly(true);
    scoreEdit[iTeam]->setAlignment(Qt::AlignRight);
    sString.sprintf("%3d", iScore[iTeam]);
    scoreEdit[iTeam]->setText(sString);

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

    teamLayout->addWidget(scoreLabel,            13, 0, 3, 2, Qt::AlignRight|Qt::AlignVCenter);
    teamLayout->addWidget(scoreEdit[iTeam],      13, 2, 3, 6, Qt::AlignHCenter|Qt::AlignVCenter);
    teamLayout->addWidget(scoreIncrement[iTeam], 12, 8, 2, 2, Qt::AlignLeft);
    teamLayout->addWidget(scoreDecrement[iTeam], 15, 8, 2, 2, Qt::AlignLeft);

    teamBox->setLayout(teamLayout);
    return teamBox;
}


QGroupBox*
BasketController::CreateGameBox() {
    QString sString;
    QGroupBox* gameBox      = new QGroupBox();
    QGridLayout* gameLayout = new QGridLayout();

    // Bonus
    for(int iTeam=0; iTeam<2; iTeam++) {
        bonusEdit[iTeam] = new Edit(QString("Bonus"));
        bonusEdit[iTeam]->setAlignment(Qt::AlignHCenter);
        bonusEdit[iTeam]->setReadOnly(true);
        if(iFauls[iTeam] >= BONUS_TARGET) {
            bonusEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        else {
            bonusEdit[iTeam]->setStyleSheet("background:white;color:white;");
        }
    }

    // Period
    QLabel *periodLabel;
    periodLabel = new QLabel(tr("Period"));
    periodLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);

    periodEdit = new Edit();
    periodEdit->setMaxLength(2);
    periodEdit->setReadOnly(true);
    periodEdit->setAlignment(Qt::AlignRight);
    sString.sprintf("%2d", iPeriod);
    periodEdit->setText(sString);

    periodIncrement = new Button(tr("+"), 0);
    periodDecrement = new Button(tr("-"), 0);

    connect(periodIncrement, SIGNAL(buttonClicked(int)),
            this, SLOT(onPeriodIncrement(int)));
    connect(periodIncrement, SIGNAL(clicked()),
            &buttonClick, SLOT(play()));
    connect(periodDecrement, SIGNAL(buttonClicked(int)),
            this, SLOT(onPeriodDecrement(int)));
    connect(periodDecrement, SIGNAL(clicked()),
            &buttonClick, SLOT(play()));

    if(iPeriod == 0)
        periodDecrement->setEnabled(false);

    gameLayout->addWidget(bonusEdit[0],    1,  0, 2, 2, Qt::AlignRight|Qt::AlignVCenter);
    gameLayout->addWidget(periodLabel,     1,  2, 2, 3, Qt::AlignRight|Qt::AlignVCenter);
    gameLayout->addWidget(periodEdit,      1,  5, 2, 3, Qt::AlignHCenter|Qt::AlignVCenter);
    gameLayout->addWidget(periodIncrement, 0,  8, 2, 2, Qt::AlignLeft);
    gameLayout->addWidget(periodDecrement, 2,  8, 2, 2, Qt::AlignLeft);
    gameLayout->addWidget(bonusEdit[1],    1, 11, 2, 2, Qt::AlignRight|Qt::AlignVCenter);
    gameBox->setLayout(gameLayout);
    return gameBox;
}


QGroupBox*
BasketController::CreateGameButtonBox() {
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
BasketController::CreateGamePanel() {
    QGridLayout* gamePanel = new QGridLayout();
    gamePanel->addWidget(CreateTeamBox(0),      0, 0, 1, 1);
    gamePanel->addWidget(CreateTeamBox(1),      0, 1, 1, 1);
    return gamePanel;
}


QString
BasketController::FormatStatusMsg() {
    QString sFunctionName = " BasketController::FormatStatusMsg ";
    Q_UNUSED(sFunctionName)
    QString sMessage = tr("");
    QString sTemp;
    for(int i=0; i<2; i++) {
        sTemp.sprintf("<team%1d>%s</team%1d>", i, sTeam[i].toLocal8Bit().data(), i);
        sMessage += sTemp;
        sTemp.sprintf("<timeout%1d>%d</timeout%1d>", i, iTimeout[i], i);
        sMessage += sTemp;
        sTemp.sprintf("<score%1d>%d</score%1d>", i, iScore[i], i);
        sMessage += sTemp;
        sTemp.sprintf("<fauls%1d>%d</fauls%1d>", i, iFauls[i], i);
        sMessage += sTemp;
        sTemp.sprintf("<fauls%1d>%d</fauls%1d>", i, iFauls[i], i);
        sMessage += sTemp;
        sTemp.sprintf("<bonus%1d>%d</bons%1d>", i, iBonus[i], i);
        sMessage += sTemp;
    }
    sTemp.sprintf("<period>%d</period>", iPeriod);
    sMessage += sTemp;
    sTemp.sprintf("<possess>%d</possess>", iPossess);
    sMessage += sTemp;
    return sMessage;
}


// =========================
// Event management routines
// =========================


void
BasketController::onTimeOutIncrement(int iTeam) {
    QString sMessage;
    iTimeout[iTeam]++;
    if(iTimeout[iTeam] == MAX_TIMEOUTS) {
        timeoutIncrement[iTeam]->setEnabled(false);
        timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }
    timeoutDecrement[iTeam]->setEnabled(true);
    sMessage.sprintf("<timeout%1d>%d</timeout%1d>", iTeam, iTimeout[iTeam], iTeam);
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iTimeout[iTeam]);
    timeoutEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/timeouts", iTeam+1);
    pSettings->setValue(sText, iTimeout[iTeam]);
}


void
BasketController::onTimeOutDecrement(int iTeam) {
    QString sMessage;
    iTimeout[iTeam]--;
    if(iTimeout[iTeam] == 0) {
        timeoutDecrement[iTeam]->setEnabled(false);
    }
    timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
    timeoutIncrement[iTeam]->setEnabled(true);
    sMessage.sprintf("<timeout%1d>%d</timeout%1d>", iTeam, iTimeout[iTeam], iTeam);
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iTimeout[iTeam]);
    timeoutEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/timeouts", iTeam+1);
    pSettings->setValue(sText, iTimeout[iTeam]);
}


void
BasketController::onFaulsIncrement(int iTeam) {
    QString sMessage, sText;
    iFauls[iTeam]++;
    faulsDecrement[iTeam]->setEnabled(true);
    if(iFauls[iTeam] == MAX_FAULS) {// To be changed
        faulsIncrement[iTeam]->setEnabled(false);
    }
    if(iFauls[iTeam] >= BONUS_TARGET) {
        iBonus[iTeam] = 1;
        bonusEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }
    else {
        iBonus[iTeam] = 0;
        bonusEdit[iTeam]->setStyleSheet("background:white;color:white;");
    }

    sMessage.sprintf("<fauls%1d>%d</fauls%1d>", iTeam, iFauls[iTeam], iTeam);
    sText.sprintf("<bonus%1d>%d</bonus%1d>", iTeam, iBonus[iTeam], iTeam);
    sMessage += sText;
    SendToAll(sMessage);
    sText.sprintf("%1d", iFauls[iTeam]);
    faulsEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/fauls", iTeam+1);
    pSettings->setValue(sText, iFauls[iTeam]);
}


void
BasketController::onFaulsDecrement(int iTeam) {
    QString sMessage, sText;
    iFauls[iTeam]--;
    faulsIncrement[iTeam]->setEnabled(true);
    if(iFauls[iTeam] == 0) {
       faulsDecrement[iTeam]->setEnabled(false);
    }
    if(iFauls[iTeam] >= BONUS_TARGET) {
        iBonus[iTeam] = 1;
        bonusEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }
    else {
        iBonus[iTeam] = 0;
        bonusEdit[iTeam]->setStyleSheet("background:white;color:white;");
    }
    sMessage.sprintf("<fauls%1d>%d</fauls%1d>", iTeam, iFauls[iTeam], iTeam);
    sText.sprintf("<bonus%1d>%d</bonus%1d>", iTeam, iBonus[iTeam], iTeam);
    sMessage += sText;
    SendToAll(sMessage);
    sText.sprintf("%1d", iFauls[iTeam]);
    faulsEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/fauls", iTeam+1);
    pSettings->setValue(sText, iFauls[iTeam]);
}


void
BasketController::onScoreIncrement(int iTeam) {
    QString sMessage;
    iScore[iTeam]++;
    scoreDecrement[iTeam]->setEnabled(true);
    if(iScore[iTeam] > 98) {
      scoreIncrement[iTeam]->setEnabled(false);
    }
    sMessage.sprintf("<score%1d>%d</score%1d>", iTeam, iScore[iTeam], iTeam);
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iScore[iTeam]);
    scoreEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/score", iTeam+1);
    pSettings->setValue(sText, iScore[iTeam]);
}


void
BasketController::onScoreDecrement(int iTeam) {
    QString sMessage;
    iScore[iTeam]--;
    scoreIncrement[iTeam]->setEnabled(true);
    if(iScore[iTeam] == 0) {
      scoreDecrement[iTeam]->setEnabled(false);
    }
    sMessage.sprintf("<score%1d>%d</score%1d>", iTeam, iScore[iTeam], iTeam);
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iScore[iTeam]);
    scoreEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/score", iTeam+1);
    pSettings->setValue(sText, iScore[iTeam]);
}


void
BasketController::onTeamTextChanged(QString sText, int iTeam) {
    QString sMessage;
    sTeam[iTeam] = sText;
    if(sText=="")// C'è un problema con la stringa vuota...
        sMessage.sprintf("<team%1d>-</team%1d>", iTeam, iTeam);
    else
        sMessage.sprintf("<team%1d>%s</team%1d>", iTeam, sTeam[iTeam].toLocal8Bit().data(), iTeam);
    SendToAll(sMessage);
    sText.sprintf("team%1d/name", iTeam+1);
    pSettings->setValue(sText, sTeam[iTeam]);
}


void
BasketController::onButtonChangeFieldClicked() {
    int iRes = QMessageBox::question(this, tr("BasketController"),
                                     tr("Scambiare il campo delle squadre ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;

    QString sText = sTeam[0];
    sTeam[0] = sTeam[1];
    sTeam[1] = sText;
    teamName[0]->setText(sTeam[0]);
    teamName[1]->setText(sTeam[1]);

    int iVal = iScore[0];
    iScore[0] = iScore[1];
    iScore[1] = iVal;
    sText.sprintf("%1d", iScore[0]);
    scoreEdit[0]->setText(sText);
    sText.sprintf("%1d", iScore[1]);
    scoreEdit[1]->setText(sText);

    iVal = iTimeout[0];
    iTimeout[0] = iTimeout[1];
    iTimeout[1] = iVal;
    sText.sprintf("%1d", iTimeout[0]);
    timeoutEdit[0]->setText(sText);
    sText.sprintf("%1d", iTimeout[1]);
    timeoutEdit[1]->setText(sText);

    for(int iTeam=0; iTeam<2; iTeam++) {
        scoreDecrement[iTeam]->setEnabled(true);
        scoreIncrement[iTeam]->setEnabled(true);
        if(iScore[iTeam] == 0) {
          scoreDecrement[iTeam]->setEnabled(false);
        }
        if(iScore[iTeam] > 98) {
          scoreIncrement[iTeam]->setEnabled(false);
        }
        timeoutIncrement[iTeam]->setEnabled(true);
        timeoutDecrement[iTeam]->setEnabled(true);
        timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
        if(iTimeout[iTeam] == MAX_TIMEOUTS) {
            timeoutIncrement[iTeam]->setEnabled(false);
            timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        if(iTimeout[iTeam] == 0) {
            timeoutDecrement[iTeam]->setEnabled(false);
        }
    }
    SendToAll(FormatStatusMsg());
    SaveStatus();
}


void
BasketController::onButtonNewPeriodClicked() {
    int iRes = QMessageBox::question(this, tr("BasketController"),
                                     tr("Vuoi davvero iniziare un nuovo Periodo ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;

    // Exchange teams order in the field
    QString sText = sTeam[0];
    sTeam[0] = sTeam[1];
    sTeam[1] = sText;
    teamName[0]->setText(sTeam[0]);
    teamName[1]->setText(sTeam[1]);
    for(int iTeam=0; iTeam<2; iTeam++) {
        iTimeout[iTeam] = 0;
        sText.sprintf("%1d", iTimeout[iTeam]);
        timeoutEdit[iTeam]->setText(sText);
        timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
        iScore[iTeam]   = 0;
        sText.sprintf("%1d", iScore[iTeam]);
        scoreEdit[iTeam]->setText(sText);
        timeoutDecrement[iTeam]->setEnabled(false);
        timeoutIncrement[iTeam]->setEnabled(true);
        scoreDecrement[iTeam]->setEnabled(false);
        scoreIncrement[iTeam]->setEnabled(true);
    }
    SendToAll(FormatStatusMsg());
    SaveStatus();
}


void
BasketController::onButtonNewGameClicked() {
    int iRes = QMessageBox::question(this, tr("BasketController"),
                                     tr("Vuoi davvero azzerare tutto ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;
    sTeam[0]    = tr("Locali");
    sTeam[1]    = tr("Ospiti");
    QString sText;
    for(int iTeam=0; iTeam<2; iTeam++) {
        teamName[iTeam]->setText(sTeam[iTeam]);
        iTimeout[iTeam] = 0;
        sText.sprintf("%1d", iTimeout[iTeam]);
        timeoutEdit[iTeam]->setText(sText);
        timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
        iScore[iTeam]   = 0;
        sText.sprintf("%1d", iScore[iTeam]);
        scoreEdit[iTeam]->setText(sText);
        timeoutDecrement[iTeam]->setEnabled(false);
        timeoutIncrement[iTeam]->setEnabled(true);
        scoreDecrement[iTeam]->setEnabled(false);
        scoreIncrement[iTeam]->setEnabled(true);
    }
    SendToAll(FormatStatusMsg());
    SaveStatus();
}


void
BasketController::onPeriodIncrement(int iDummy) {
    Q_UNUSED(iDummy)
    if(iPeriod < MAX_PERIODS) {
        iPeriod++;
    }
    if(iPeriod >= MAX_PERIODS) {
        periodIncrement->setDisabled(true);
        iPeriod= MAX_PERIODS;
    }
    periodDecrement->setEnabled(true);
    QString sString, sMessage;
    sString.sprintf("%2d", iPeriod);
    periodEdit->setText(sString);
    sMessage.sprintf("<period>%d</period>", iPeriod);
    SendToAll(sMessage);
    pSettings->setValue("game/period", iPeriod);
}


void
BasketController::onPeriodDecrement(int iDummy) {
    Q_UNUSED(iDummy)
    if(iPeriod > 0) {
        iPeriod--;
    }
    if(iPeriod >= MAX_PERIODS) {
        periodIncrement->setDisabled(true);
        iPeriod= MAX_PERIODS;
    }
    periodIncrement->setEnabled(true);
    QString sString, sMessage;
    sString.sprintf("%2d", iPeriod);
    periodEdit->setText(sString);
    sMessage.sprintf("<period>%d</period>", iPeriod);
    SendToAll(sMessage);
    pSettings->setValue("game/period", iPeriod);
}


void
BasketController::onPossessClicked(int iTeam, bool bChecked) {
    Q_UNUSED(bChecked)
    QString sMessage;
    iPossess = iTeam;
    possess[iPossess ? 1 : 0]->setChecked(true);
    possess[iPossess ? 0 : 1]->setChecked(false);
    sMessage.sprintf("<possess>%d</possess>", iPossess);
    SendToAll(sMessage);
    pSettings->setValue("game/possess", iPossess);
}


