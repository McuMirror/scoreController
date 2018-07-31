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
#include <QResizeEvent>

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
    , bFontBuilt(false)
    , maxTimeouts(MAX_TIMEOUTS)
    , maxPeriods(MAX_PERIODS)
    , periodTime(REGULAR_TIME)

{
    GetSettings();
    prepareDirectories();

    logFileName = QString("%1score_controller.txt").arg(sLogDir);
    prepareLogFile();
    prepareServices();

    pSlideUpdaterServer->setDir(sSlideDir, "*.jpg *.jpeg *.png *.JPG *.JPEG *.PNG");
    emit startSlideServer();

    pSpotUpdaterServer->setDir(sSpotDir, "*.mp4 *.MP4");
    emit startSpotServer();

    buildControls();
    auto *mainLayout = new QGridLayout();

    int gamePanelWidth   = 15;
    int gamePanelHeight  = 8;

    mainLayout->addLayout(CreateGamePanel(),
                          0,
                          0,
                          gamePanelHeight,
                          gamePanelWidth);

    mainLayout->addLayout(CreateGameButtons(),
                          gamePanelHeight,
                          0,
                          2,
                          5);

    mainLayout->addLayout(CreateSpotButtons(),
                          gamePanelHeight,
                          5,
                          2,
                          gamePanelWidth-5);
    setLayout(mainLayout);

    setEventHandlers();
}


void
HandballController::buildControls() {
    QString sString;
    QPixmap plusPixmap, minusPixmap;
    QIcon plusButtonIcon, minusButtonIcon;
    plusPixmap.load(":/buttonIcons/Plus.png");
    plusButtonIcon.addPixmap(plusPixmap);
    minusPixmap.load(":/buttonIcons/Minus.png");
    minusButtonIcon.addPixmap(minusPixmap);
    for(int iTeam=0; iTeam<2; iTeam++) {
        // Teams
        teamName[iTeam] = new Edit(sTeam[iTeam], iTeam);
        teamName[iTeam]->setAlignment(Qt::AlignHCenter);
        teamName[iTeam]->setMaxLength(15);
        // Timeout
        sString.sprintf("%1d", iTimeout[iTeam]);
        timeoutEdit[iTeam] = new Edit(sString);
        timeoutEdit[iTeam]->setMaxLength(1);
        timeoutEdit[iTeam]->setAlignment(Qt::AlignHCenter);
        timeoutEdit[iTeam]->setReadOnly(true);
        // Timeout buttons
        timeoutIncrement[iTeam] = new Button("", iTeam);
        timeoutIncrement[iTeam]->setIcon(plusButtonIcon);
        timeoutIncrement[iTeam]->setIconSize(plusPixmap.rect().size());
        timeoutDecrement[iTeam] = new Button("", iTeam);
        timeoutDecrement[iTeam]->setIcon(minusButtonIcon);
        timeoutDecrement[iTeam]->setIconSize(minusPixmap.rect().size());
        if(iTimeout[iTeam] == 0)
            timeoutDecrement[iTeam]->setEnabled(false);
        if(iTimeout[iTeam] == MAX_TIMEOUTS) {
            timeoutIncrement[iTeam]->setEnabled(false);
            timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        // Score
        sString.sprintf("%3d", iScore[iTeam]);
        scoreEdit[iTeam] = new Edit(sString);
        scoreEdit[iTeam]->setMaxLength(3);
        scoreEdit[iTeam]->setReadOnly(true);
        scoreEdit[iTeam]->setAlignment(Qt::AlignRight);
        // Score buttons
        scoreIncrement[iTeam] = new Button("", iTeam);
        scoreIncrement[iTeam]->setIcon(plusButtonIcon);
        scoreIncrement[iTeam]->setIconSize(plusPixmap.rect().size());
        scoreDecrement[iTeam] = new Button("", iTeam);
        scoreDecrement[iTeam]->setIcon(minusButtonIcon);
        scoreDecrement[iTeam]->setIconSize(minusPixmap.rect().size());
        if(iScore[iTeam] == 0)
            scoreDecrement[iTeam]->setEnabled(false);
    }
    // Period
    sString.sprintf("%2d", iPeriod);
    periodEdit = new Edit(sString);
    periodEdit->setMaxLength(2);
    periodEdit->setReadOnly(true);
    periodEdit->setAlignment(Qt::AlignRight);
    // Period Buttons
    periodIncrement = new QPushButton();
    periodIncrement->setIcon(plusButtonIcon);
    periodIncrement->setIconSize(plusPixmap.rect().size());
    periodDecrement = new QPushButton();
    periodDecrement->setIcon(minusButtonIcon);
    periodDecrement->setIconSize(minusPixmap.rect().size());
    if(iPeriod < 2)
        periodDecrement->setDisabled(true);

    //Labels:

    // Timeout
    timeoutLabel = new QLabel(tr("Timeout"));
    timeoutLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);
    // Score
    scoreLabel = new QLabel(tr("Score"));
    scoreLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);
    // Period
    periodLabel = new QLabel(tr("Period"));
    periodLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);
}


void
HandballController::resizeEvent(QResizeEvent *event) {
    if(!bFontBuilt) {
        bFontBuilt = true;
        buildFontSizes();
        event->setAccepted(true);
    }
}


void
HandballController::buildFontSizes() {
    QFont font;
    int iFontSize;
    int hMargin, vMargin;
    QMargins margins;
    // Teams
    font = teamName[0]->font();
    margins = teamName[0]->contentsMargins();
    vMargin = margins.bottom() + margins.top();
    hMargin = margins.left() + margins.right();
    font.setCapitalization(QFont::Capitalize);
    iFontSize = qMin((teamName[0]->width()/teamName[0]->maxLength())-2*hMargin,
                     teamName[0]->height()-vMargin);
    font.setPixelSize(iFontSize);
    teamName[0]->setFont(font);
    teamName[1]->setFont(font);
    // Timeout
    font = timeoutEdit[0]->font();
    margins = timeoutEdit[0]->contentsMargins();
    vMargin = margins.bottom() + margins.top();
    hMargin = margins.left() + margins.right();
    iFontSize = qMin((timeoutEdit[0]->width()/timeoutEdit[0]->maxLength())-2*hMargin,
                     timeoutEdit[0]->height()-vMargin);
    font.setPixelSize(iFontSize);
    timeoutEdit[0]->setFont(font);
    timeoutEdit[1]->setFont(font);
    // Period
    font = periodEdit->font();
    margins = periodEdit->contentsMargins();
    vMargin = margins.bottom() + margins.top();
    hMargin = margins.left() + margins.right();
    iFontSize = qMin((periodEdit->width()/periodEdit->maxLength())-2*hMargin,
                     periodEdit->height()-vMargin);
    font.setPixelSize(iFontSize);
    font.setPixelSize(iFontSize);
    periodEdit->setFont(font);
    periodEdit->setFont(font);
    // Score
    font = scoreEdit[0]->font();
    margins = scoreEdit[0]->contentsMargins();
    vMargin = margins.bottom() + margins.top();
    hMargin = margins.left() + margins.right();
    font.setWeight(QFont::Black);
    iFontSize = qMin((scoreEdit[0]->width()/scoreEdit[0]->maxLength())-2*hMargin,
                     scoreEdit[0]->height()-vMargin);
    font.setPixelSize(iFontSize);
    scoreEdit[0]->setFont(font);
    scoreEdit[1]->setFont(font);

    //Labels:
// Can't understand why it is noy working !
//    font = scoreLabel->font();
//    iFontSize = qMin(scoreLabel->width()/scoreLabel->text().length(),
//                     scoreLabel->height()/2);
//    font.setPixelSize(iFontSize);

    scoreLabel->setFont(font);
    font.setWeight(QFont::Normal);
    timeoutLabel->setFont(font);
    periodLabel->setFont(font);
}


void
HandballController::setEventHandlers() {
    for(int iTeam=0; iTeam <2; iTeam++) {
        // Teams
        connect(teamName[iTeam], SIGNAL(textChanged(QString,int)),
                this, SLOT(onTeamTextChanged(QString,int)));
        // Timeouts
        connect(timeoutIncrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onTimeOutIncrement(int)));
        connect(timeoutIncrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(timeoutDecrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onTimeOutDecrement(int)));
        connect(timeoutDecrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        // Scores
        connect(scoreIncrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onScoreIncrement(int)));
        connect(scoreIncrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(scoreDecrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onScoreDecrement(int)));
        connect(scoreDecrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
    }
    // Period
    connect(periodIncrement, SIGNAL(clicked()),
            this, SLOT(onPeriodIncrement()));
    connect(periodIncrement, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
    connect(periodDecrement, SIGNAL(clicked()),
            this, SLOT(onPeriodDecrement()));
    connect(periodDecrement, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
    // New Period
    connect(newPeriodButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonNewPeriodClicked()));
    connect(newPeriodButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
    // New Game
    connect(newGameButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonNewGameClicked()));
    connect(newGameButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
    // Exchange Field Position
    connect(changeFieldButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonChangeFieldClicked()));
    connect(changeFieldButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
}


void
HandballController::GetSettings() {
    pSettings = new QSettings("Gabriele Salvato", "Handball Controller");

    sTeam[0]    = pSettings->value("team1/name", QString(tr("Locali"))).toString();
    sTeam[1]    = pSettings->value("team2/name", QString(tr("Ospiti"))).toString();
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
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Slide Dir: %1").arg(sSlideDir));
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Spot Dir: %1").arg(sSpotDir));
}


void
HandballController::closeEvent(QCloseEvent *event) {
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Closing"));
    SaveStatus();
    ScoreController::closeEvent(event);// Propagate the event
}


QHBoxLayout*
HandballController::CreateGameButtons() {
    auto* gameButtonLayout = new QHBoxLayout();
    QPixmap pixmap(":/buttonIcons/ExchangeBasketField.png");
    QIcon ButtonIcon(pixmap);
    changeFieldButton = new QPushButton(ButtonIcon, "");
    changeFieldButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/New-Set-Volley.png");
    ButtonIcon.addPixmap(pixmap);
    newPeriodButton   = new QPushButton(ButtonIcon, "");
    newPeriodButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/NewGameHandball.png");
    ButtonIcon.addPixmap(pixmap);
    newGameButton = new QPushButton(ButtonIcon, "");
    newGameButton->setIconSize(pixmap.rect().size());

    gameButtonLayout->addWidget(newPeriodButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(newGameButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(changeFieldButton);
    gameButtonLayout->addStretch();
    return gameButtonLayout;
}


QGridLayout*
HandballController::CreateGamePanel() {
    auto* gamePanel = new QGridLayout();
    //
    int iRow = 0;
    for(int iTeam=0; iTeam<2; iTeam++) {
        // Matrice x righe e 8 colonne
        iRow = 0;
        gamePanel->addWidget(teamName[iTeam], iRow, iTeam*4, 1, 4);
        int iCol = iTeam*5;
        iRow += 1;
        gamePanel->addWidget(timeoutDecrement[iTeam], iRow, iCol,   1, 1, Qt::AlignRight);
        gamePanel->addWidget(timeoutEdit[iTeam],      iRow, iCol+1, 1, 1, Qt::AlignHCenter|Qt::AlignVCenter);
        gamePanel->addWidget(timeoutIncrement[iTeam], iRow, iCol+2, 1, 1, Qt::AlignLeft);
        iRow += 3; // Leave space for Period controls
        gamePanel->addWidget(scoreDecrement[iTeam], iRow, iCol,   2, 1, Qt::AlignRight);
        gamePanel->addWidget(scoreEdit[iTeam],      iRow, iCol+1, 2, 1, Qt::AlignHCenter|Qt::AlignVCenter);
        gamePanel->addWidget(scoreIncrement[iTeam], iRow, iCol+2, 2, 1, Qt::AlignLeft);
    }
    gamePanel->addWidget(scoreLabel, iRow, 3, 2, 2, Qt::AlignHCenter|Qt::AlignVCenter);

    iRow -= 2;
    gamePanel->addWidget(periodLabel,     iRow,  0, 2, 2, Qt::AlignRight|Qt::AlignVCenter);
    gamePanel->addWidget(periodDecrement, iRow,  2, 2, 1, Qt::AlignRight);
    gamePanel->addWidget(periodEdit,      iRow,  3, 2, 2, Qt::AlignHCenter|Qt::AlignVCenter);
    gamePanel->addWidget(periodIncrement, iRow,  5, 2, 1, Qt::AlignLeft);

    gamePanel->addWidget(timeoutLabel, 1, 3, 1, 2, Qt::AlignHCenter|Qt::AlignVCenter);

    return gamePanel;
}


QString
HandballController::FormatStatusMsg() {
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
    if(myStatus == showSlides)
        sMessage += "<slideshow>1</slideshow>";
    else if(myStatus == showCamera)
        sMessage += QString("<live>1</live>");
    else if(myStatus == showSpots)
        sMessage += QString("<spotloop>1</spotloop>");

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
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Slide Dir: %1").arg(sSlideDir));
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Spot Dir: %1").arg(sSpotDir));
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
    timeoutEdit[iTeam]->setStyleSheet(styleSheet());
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
HandballController::onPeriodIncrement() {
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
HandballController::onPeriodDecrement() {
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
        timeoutEdit[iTeam]->setStyleSheet(styleSheet());
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
                                     tr("Iniziare una Nuova Partita ?"),
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
        timeoutEdit[iTeam]->setStyleSheet(styleSheet());
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
        timeoutEdit[iTeam]->setStyleSheet(styleSheet());
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

