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
#include <QMessageBox>

#include "handballcontroller.h"
#include "fileserver.h"
#include "edit.h"
#include "button.h"

#define MAX_TIMEOUTS   2 // Numero massimo di sospensioni
#define MAX_PERIODS    2 //
#define MAX_SCORE    999 //
#define REGULAR_TIME  30 // 30 Minuti è la durata di un tempo regolare


HandballController::HandballController()
    : ScoreController(HANDBALL_PANEL, Q_NULLPTR)
    , maxTimeouts(MAX_TIMEOUTS)
    , maxPeriods(MAX_PERIODS)
    , periodTime(REGULAR_TIME)

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
HandballController::GetSettings() {
    QString sFunctionName = QString(" VolleyController::GetSettings ");
    Q_UNUSED(sFunctionName)
    pSettings = new QSettings("Gabriele Salvato", "Handball Controller");

    sTeam[0]    = pSettings->value("team1/name", QString("Locali")).toString();
    sTeam[1]    = pSettings->value("team2/name", QString("Ospiti")).toString();
    iTimeout[0] = pSettings->value("team1/timeouts", 0).toInt();
    iTimeout[1] = pSettings->value("team2/timeouts", 0).toInt();
    iScore[0]   = pSettings->value("team1/score", 0).toInt();
    iScore[1]   = pSettings->value("team2/score", 0).toInt();
    iPeriod     = pSettings->value("game/period", 1).toInt();

    // Safety check
    for(int i=0; i<2; i++) {
        if(iTimeout[i] < 0) iTimeout[i] = 0;
        if(iTimeout[i] > maxTimeouts) iTimeout[i] = maxTimeouts;
        if(iScore[i] < 0) iScore[i] = 0;
        if(iScore[i] > MAX_SCORE) iScore[i] = MAX_SCORE;
    }
    if(iPeriod < 0) iPeriod = 0;
    if(iPeriod > maxPeriods) iPeriod = maxPeriods;

    pSettings->setValue("team1/timeouts", iTimeout[0]);
    pSettings->setValue("team2/timeouts", iTimeout[1]);
    pSettings->setValue("team1/score", iScore[0]);
    pSettings->setValue("team2/score", iScore[1]);
    pSettings->setValue("game/period", iPeriod);

    sSlideDir   = pSettings->value("directories/slides", sSlideDir).toString();
    sSpotDir    = pSettings->value("directories/spots", sSpotDir).toString();
}


void
HandballController::closeEvent(QCloseEvent *event) {
    QString sFunctionName = " HandballController::closeEvent ";
    Q_UNUSED(sFunctionName)
    SaveStatus();
    ScoreController::closeEvent(event);// Propagate the event
}


QGroupBox*
HandballController::CreateTeamBox(int iTeam) {
    QGroupBox* teamBox      = new QGroupBox();
    QString sString;
    QGridLayout* teamLayout = new QGridLayout();
    QLabel* labelSpacer = new QLabel(QString());

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
    if(iTeamFontSize < 11) iTeamFontSize = 11;
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
    if(iTimeoutLabelFontSize < 11) iTimeoutLabelFontSize = 11;
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

    timeoutIncrement[iTeam] = new Button("+", iTeam);
    timeoutDecrement[iTeam] = new Button("-", iTeam);

    connect(timeoutIncrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onTimeOutIncrement(int)));
    connect(timeoutIncrement[iTeam], SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
    connect(timeoutDecrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onTimeOutDecrement(int)));
    connect(timeoutDecrement[iTeam], SIGNAL(clicked()),
            pButtonClick, SLOT(play()));

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

    scoreIncrement[iTeam] = new Button("+", iTeam);
    scoreDecrement[iTeam] = new Button("-", iTeam);

    connect(scoreIncrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onScoreIncrement(int)));
    connect(scoreIncrement[iTeam], SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
    connect(scoreDecrement[iTeam], SIGNAL(buttonClicked(int)),
            this, SLOT(onScoreDecrement(int)));
    connect(scoreDecrement[iTeam], SIGNAL(clicked()),
            pButtonClick, SLOT(play()));

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
    QString sString;
    QGridLayout* gameLayout = new QGridLayout();

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width();
    int rW;

    // Period
    QLabel *periodLabel;
    periodLabel = new QLabel(tr("Period"));
    periodLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);

    QFont font = periodLabel->font();
    int iPeriodLabelFontSize = font.pointSize();
    for(int i=iPeriodLabelFontSize; i<100; i++) {
        font.setPointSize(i);
        QFontMetrics f(font);
        rW = f.width(periodLabel->text());
        if(rW > width/10) {
            iPeriodLabelFontSize = i-1;
            break;
        }
    }
    font.setPointSize(iPeriodLabelFontSize);

    font = periodLabel->font();
    font.setPointSize(iPeriodLabelFontSize);
    periodLabel->setFont(font);

    periodEdit = new Edit();
    periodEdit->setMaxLength(2);
    periodEdit->setReadOnly(true);
    periodEdit->setAlignment(Qt::AlignRight);
    sString.sprintf("%2d", iPeriod);
    periodEdit->setText(sString);

    font = periodEdit->font();
    font.setPointSize(iPeriodLabelFontSize);
    periodEdit->setFont(font);

    periodIncrement = new Button("+", 0);
    periodDecrement = new Button("-", 0);

    connect(periodIncrement, SIGNAL(buttonClicked(int)),
            this, SLOT(onPeriodIncrement(int)));
    connect(periodIncrement, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
    connect(periodDecrement, SIGNAL(buttonClicked(int)),
            this, SLOT(onPeriodDecrement(int)));
    connect(periodDecrement, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));

    if(iPeriod < 2)
        periodDecrement->setEnabled(false);

    gameLayout->addWidget(periodLabel,     0,  2, 2, 2, Qt::AlignRight|Qt::AlignVCenter);
    gameLayout->addWidget(periodDecrement, 0,  4, 2, 2, Qt::AlignRight);
    gameLayout->addWidget(periodEdit,      0,  6, 2, 2, Qt::AlignHCenter|Qt::AlignVCenter);
    gameLayout->addWidget(periodIncrement, 0,  8, 2, 2, Qt::AlignLeft);
    gameBox->setLayout(gameLayout);
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
            pButtonClick, SLOT(play()));
    connect(newGameButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonNewGameClicked()));
    connect(newGameButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
    connect(changeFieldButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonChangeFieldClicked()));
    connect(changeFieldButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));

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
    QString sFunctionName = " HandballController::FormatStatusMsg ";
    Q_UNUSED(sFunctionName)
    QString sMessage = QString();

    QString sTemp;
    for(int i=0; i<2; i++) {
        sTemp.sprintf("<team%1d>%s</team%1d>", i, sTeam[i].toLocal8Bit().data(), i);
        sMessage += sTemp;
        sTemp.sprintf("<timeout%1d>%d</timeout%1d>", i, iTimeout[i], i);
        sMessage += sTemp;
        sTemp.sprintf("<score%1d>%d</score%1d>", i, iScore[i], i);
        sMessage += sTemp;
    }
    sTemp.sprintf("<period>%d,%d</period>", iPeriod, periodTime);
    sMessage += sTemp;
    if(!startStopSlideShowButton->text().contains(QString(tr("Avvia"))))
        sMessage += "<slideshow>1</slideshow>";
    else if(!startStopLiveCameraButton->text().contains(QString(tr("Avvia"))))
        sMessage += QString("<live>1</live>");
    else if(!startStopLoopSpotButton->text().contains(QString(tr("Avvia"))))
        sMessage += QString("<spotloop>1</spotloop>");
    else if(!startStopSpotButton->text().contains(QString(tr("Avvia"))))
        sMessage += QString("<spot>1</spot>");

    return sMessage;
}


void
HandballController::SaveStatus() {
    pSettings->setValue("team1/name", sTeam[0]);
    pSettings->setValue("team2/name", sTeam[1]);
    pSettings->setValue("team1/timeouts", iTimeout[0]);
    pSettings->setValue("team2/timeouts", iTimeout[1]);
    pSettings->setValue("team1/score", iScore[0]);
    pSettings->setValue("team2/score", iScore[1]);

    pSettings->setValue("game/period", iPeriod);

    pSettings->setValue("directories/slides", sSlideDir);
    pSettings->setValue("directories/spots", sSpotDir);
}


// =========================
// Event management routines
// =========================


void
HandballController::onTimeOutIncrement(int iTeam) {
    QString sMessage;
    iTimeout[iTeam]++;
    if(iTimeout[iTeam] == maxTimeouts) {
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
HandballController::onTimeOutDecrement(int iTeam) {
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
HandballController::onScoreIncrement(int iTeam) {
    QString sMessage;
    iScore[iTeam]++;
    scoreDecrement[iTeam]->setEnabled(true);
    if(iScore[iTeam] >= MAX_SCORE) {
        iScore[iTeam] = MAX_SCORE;
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
HandballController::onScoreDecrement(int iTeam) {
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
HandballController::onTeamTextChanged(QString sText, int iTeam) {
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
HandballController::onPeriodIncrement(int iDummy) {
    Q_UNUSED(iDummy)
    if(iPeriod < maxPeriods) {
        iPeriod++;
    }
    if(iPeriod >= maxPeriods) {
        periodIncrement->setDisabled(true);
        iPeriod= maxPeriods;
    }
    periodDecrement->setEnabled(true);
    QString sString, sMessage;
    sString.sprintf("%2d", iPeriod);
    periodEdit->setText(sString);
    sMessage.sprintf("<period>%d,%d</period>", iPeriod, periodTime);
    SendToAll(sMessage);
    pSettings->setValue("game/period", iPeriod);
}


void
HandballController::onPeriodDecrement(int iDummy) {
    Q_UNUSED(iDummy)
    if(iPeriod > 1) {
        iPeriod--;
    }
    if(iPeriod < 2)
        periodDecrement->setDisabled(true);
    if(iPeriod >= maxPeriods) {
        periodIncrement->setDisabled(true);
        iPeriod= maxPeriods;
    }
    periodIncrement->setEnabled(true);
    QString sString, sMessage;
    sString.sprintf("%2d", iPeriod);
    periodEdit->setText(sString);
    sMessage.sprintf("<period>%d,%d</period>", iPeriod, periodTime);
    SendToAll(sMessage);
    pSettings->setValue("game/period", iPeriod);
}


void
HandballController::onButtonNewPeriodClicked() {
    int iRes = QMessageBox::question(this, tr("Handball Controller"),
                                     tr("Vuoi davvero iniziare un nuovo Periodo ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;

    // Increment period number
    if(iPeriod < maxPeriods) {
        iPeriod++;
    }
    if(iPeriod >= maxPeriods) {
        periodIncrement->setDisabled(true);
        iPeriod= maxPeriods;
    }
    periodDecrement->setEnabled(true);
    QString sString;
    sString.sprintf("%2d", iPeriod);
    periodEdit->setText(sString);

    // Exchange teams order, score and timeouts
    QString sText = sTeam[0];
    sTeam[0] = sTeam[1];
    sTeam[1] = sText;

    int iVal = iScore[0];
    iScore[0] = iScore[1];
    iScore[1] = iVal;

    iVal = iTimeout[0];
    iTimeout[0] = iTimeout[1];
    iTimeout[1] = iVal;

    // Update panel
    for(int iTeam=0; iTeam<2; iTeam++) {
        teamName[iTeam]->setText(sTeam[iTeam]);
        sText.sprintf("%1d", iScore[iTeam]);
        scoreEdit[iTeam]->setText(sText);
        scoreDecrement[iTeam]->setEnabled(true);
        scoreIncrement[iTeam]->setEnabled(true);
        if(iScore[iTeam] == 0) {
          scoreDecrement[iTeam]->setEnabled(false);
        }
        if(iScore[iTeam] >= MAX_SCORE) {
          scoreIncrement[iTeam]->setEnabled(false);
        }
        sText.sprintf("%1d", iTimeout[iTeam]);
        timeoutEdit[iTeam]->setText(sText);
        timeoutIncrement[iTeam]->setEnabled(true);
        timeoutDecrement[iTeam]->setEnabled(true);
        timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
        if(iTimeout[iTeam] >= maxTimeouts) {
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
HandballController::onButtonNewGameClicked() {
    int iRes = QMessageBox::question(this, tr("Handball Controller"),
                                     tr("Vuoi davvero azzerare tutto ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;
    sTeam[0]    = tr("Locali");
    sTeam[1]    = tr("Ospiti");
    QString sText;
    iPeriod = 1;
    sText.sprintf("%2d", iPeriod);
    periodEdit->setText(sText);
    periodIncrement->setEnabled(true);
    periodDecrement->setEnabled(false);
    for(int iTeam=0; iTeam<2; iTeam++) {
        teamName[iTeam]->setText(sTeam[iTeam]);
        iTimeout[iTeam] = 0;
        sText.sprintf("%1d", iTimeout[iTeam]);
        timeoutEdit[iTeam]->setText(sText);
        timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
        timeoutDecrement[iTeam]->setEnabled(false);
        timeoutIncrement[iTeam]->setEnabled(true);
        iScore[iTeam]   = 0;
        sText.sprintf("%3d", iScore[iTeam]);
        scoreEdit[iTeam]->setText(sText);
        scoreDecrement[iTeam]->setEnabled(false);
        scoreIncrement[iTeam]->setEnabled(true);
    }
    SendToAll(FormatStatusMsg());
    SaveStatus();
}


void
HandballController::onButtonChangeFieldClicked() {
    int iRes = QMessageBox::question(this, tr("Handball Controller"),
                                     tr("Scambiare il campo delle squadre ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;

    // Exchange teams order, score and timeouts
    QString sText = sTeam[0];
    sTeam[0] = sTeam[1];
    sTeam[1] = sText;

    int iVal = iScore[0];
    iScore[0] = iScore[1];
    iScore[1] = iVal;

    iVal = iTimeout[0];
    iTimeout[0] = iTimeout[1];
    iTimeout[1] = iVal;

    // Update panel
    for(int iTeam=0; iTeam<2; iTeam++) {
        teamName[iTeam]->setText(sTeam[iTeam]);
        sText.sprintf("%1d", iScore[iTeam]);
        scoreEdit[iTeam]->setText(sText);
        scoreDecrement[iTeam]->setEnabled(true);
        scoreIncrement[iTeam]->setEnabled(true);
        if(iScore[iTeam] == 0) {
          scoreDecrement[iTeam]->setEnabled(false);
        }
        if(iScore[iTeam] >= MAX_SCORE) {
          scoreIncrement[iTeam]->setEnabled(false);
        }
        sText.sprintf("%1d", iTimeout[iTeam]);
        timeoutEdit[iTeam]->setText(sText);
        timeoutIncrement[iTeam]->setEnabled(true);
        timeoutDecrement[iTeam]->setEnabled(true);
        timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
        if(iTimeout[iTeam] >= maxTimeouts) {
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


