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
#include <QDir>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QGuiApplication>
#include <QScreen>

#include "volleycontroller.h"
#include "edit.h"
#include "button.h"
#include "radioButton.h"
#include "fileserver.h"

#define MAX_TIMEOUTS 2
#define MAX_SETS     3

VolleyController::VolleyController()
    : ScoreController(VOLLEY_PANEL, Q_NULLPTR)
{
    QString sFunctionName = QString(" VolleyController::VolleyController ");
    Q_UNUSED(sFunctionName)

    GetSettings();
    PrepareDirectories();

    logFileName = QString("%1score_controller.txt").arg(sLogDir);
    PrepareLogFile();
    prepareServices();

    pSlideUpdaterServer->setDir(sSlideDir, "*.jpg *.jpeg *.png *.JPG *.JPEG *.PNG");
    emit startSlideServer();

    pSpotUpdaterServer->setDir(sSpotDir, "*.mp4 *.MP4");
    emit startSpotServer();

    buildControls();
    QGridLayout *mainLayout = new QGridLayout();

    int gamePanelWidth  = 15;
    int gamePanelHeigth =  8;

    mainLayout->addLayout(CreateGamePanel(),
                          0,
                          0,
                          gamePanelHeigth,
                          gamePanelWidth);

    mainLayout->addLayout(CreateGameButtons(),
                          gamePanelHeigth,
                          0,
                          2,
                          5);

    mainLayout->addLayout(CreateSpotButtons(),
                          gamePanelHeigth,
                          5,
                          2,
                          gamePanelWidth-5);

    setLayout(mainLayout);

    buildFontSizes();
    setEventHandlers();

    service[iServizio ? 1 : 0]->setChecked(true);
    service[iServizio ? 0 : 1]->setChecked(false);
}


void
VolleyController::buildControls() {
    QString sString;
    QPixmap plusPixmap, minusPixmap;
    QIcon plusButtonIcon, minusButtonIcon;
    plusPixmap.load(":/buttonIcons/Plus.png");
    plusButtonIcon.addPixmap(plusPixmap);
    minusPixmap.load(":/buttonIcons/Minus.png");
    minusButtonIcon.addPixmap(minusPixmap);
    for(int iTeam=0; iTeam<2; iTeam++){
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
        // Sets
        sString.sprintf("%1d", iSet[iTeam]);
        setsEdit[iTeam] = new Edit(sString);
        setsEdit[iTeam]->setMaxLength(1);
        setsEdit[iTeam]->setAlignment(Qt::AlignHCenter);
        setsEdit[iTeam]->setReadOnly(true);
        // Set buttons
        setsIncrement[iTeam] = new Button("", iTeam);
        setsIncrement[iTeam]->setIcon(plusButtonIcon);
        setsIncrement[iTeam]->setIconSize(plusPixmap.rect().size());
        setsDecrement[iTeam] = new Button("", iTeam);
        setsDecrement[iTeam]->setIcon(minusButtonIcon);
        setsDecrement[iTeam]->setIconSize(minusPixmap.rect().size());
        if(iSet[iTeam] == 0)
            setsDecrement[iTeam]->setEnabled(false);
        if(iSet[iTeam] == MAX_SETS)
            setsIncrement[iTeam]->setEnabled(false);
        // Service
        service[iTeam] = new RadioButton(" ", iTeam);// Android requires at least one character to work
        // Score
        scoreLabel = new QLabel(tr("Punti"));
        scoreLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);
        sString.sprintf("%2d", iScore[iTeam]);
        scoreEdit[iTeam] = new Edit(sString);
        scoreEdit[iTeam]->setMaxLength(2);
        scoreEdit[iTeam]->setReadOnly(true);
        scoreEdit[iTeam]->setAlignment(Qt::AlignHCenter);
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
    // Timeout
    timeoutLabel = new QLabel(tr("Timeout"));
    timeoutLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    // Set
    setsLabel = new QLabel(tr("Set Vinti"));
    setsLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    // Service
    serviceLabel = new QLabel(tr("Servizio"));
    serviceLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    // Score
    scoreLabel = new QLabel(tr("Punti"));
    scoreLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void
VolleyController::buildFontSizes() {
    int rW, rH;
    QFont font;

    font = teamName[0]->font();
    font.setCapitalization(QFont::Capitalize);
    iTeamFontSize = QFontMetrics(font).maxWidth();
    rH = QFontMetrics(font).height();
    for(int i=iTeamFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).maxWidth()*15;
        rH = QFontMetrics(font).height();
        if((rW > teamName[0]->width()) || (rH > teamName[0]->height())){
            iTeamFontSize = i-1;
            break;
        }
    }
    font.setPixelSize(iTeamFontSize);
    teamName[0]->setFont(font);
    teamName[1]->setFont(font);

    font = setsEdit[0]->font();
    iSetFontSize = QFontMetrics(font).maxWidth();
    rH = QFontMetrics(font).height();
    for(int i=iSetFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).maxWidth();
        rH = QFontMetrics(font).height();
        if((rW > setsEdit[0]->width()) || (rH > setsEdit[0]->height())){
            iSetFontSize = i-1;
            break;
        }
    }
    font.setPixelSize(iSetFontSize);
    setsEdit[0]->setFont(font);
    setsEdit[1]->setFont(font);

    font = timeoutEdit[0]->font();
    iTimeoutFontSize = QFontMetrics(font).maxWidth();
    rH = QFontMetrics(font).height();
    for(int i=iTimeoutFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).maxWidth();
        rH = QFontMetrics(font).height();
        if((rW > timeoutEdit[0]->width()) || (rH > timeoutEdit[0]->height())){
            iTimeoutFontSize = i-1;
            break;
        }
    }
    font.setPixelSize(iTimeoutFontSize);
    timeoutEdit[0]->setFont(font);
    timeoutEdit[1]->setFont(font);

    font = scoreEdit[0]->font();
    font.setWeight(QFont::Black);
    iScoreFontSize = QFontMetrics(font).maxWidth();
    rH = QFontMetrics(font).height();
    for(int i=iScoreFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).maxWidth()*2;
        rH = QFontMetrics(font).height();
        if((rW > scoreEdit[0]->width()) || (rH > scoreEdit[0]->height())){
            iScoreFontSize = i-1;
            break;
        }
    }
    font.setPixelSize(iScoreFontSize);
    scoreEdit[0]->setFont(font);
    scoreEdit[1]->setFont(font);

    font = timeoutLabel->font();
    iLabelFontSize = QFontMetrics(font).maxWidth();
    rH = QFontMetrics(font).height();
    for(int i=iLabelFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).maxWidth()*15;
        rH = QFontMetrics(font).height();
        if((rW > timeoutLabel->width()) || (rH > timeoutLabel->height())){
            iScoreFontSize = i-1;
            break;
        }
    }

    font.setPixelSize(iLabelFontSize);
    timeoutLabel->setFont(font);
    setsLabel->setFont(font);
    serviceLabel->setFont(font);
    font.setWeight(QFont::Black);
    scoreLabel->setFont(font);

    font = timeoutIncrement[0]->font();
    font.setWeight(QFont::Black);
    iTimeoutFontSize = QFontMetrics(font).maxWidth();
    rH = QFontMetrics(font).height();
    for(int i=iTimeoutFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).maxWidth();
        rH = QFontMetrics(font).height();
        if((rW > timeoutIncrement[0]->width()) || (rH > timeoutIncrement[0]->height())){
            iTimeoutFontSize = i-1;
            break;
        }
    }
    timeoutIncrement[0]->setFont(font);
    timeoutIncrement[1]->setFont(font);
    timeoutDecrement[0]->setFont(font);
    timeoutDecrement[1]->setFont(font);
    setsIncrement[0]->setFont(font);
    setsIncrement[1]->setFont(font);
    setsDecrement[0]->setFont(font);
    setsDecrement[1]->setFont(font);
    scoreIncrement[0]->setFont(font);
    scoreIncrement[1]->setFont(font);
    scoreDecrement[0]->setFont(font);
    scoreDecrement[1]->setFont(font);
}


void
VolleyController::GetSettings() {
    QString sFunctionName = QString(" VolleyController::GetSettings ");
    Q_UNUSED(sFunctionName)

    pSettings = new QSettings("Gabriele Salvato", "Volley Controller");

    sTeam[0]    = pSettings->value("team1/name", QString(tr("Locali"))).toString();
    sTeam[1]    = pSettings->value("team2/name", QString(tr("Ospiti"))).toString();
    iTimeout[0] = pSettings->value("team1/timeouts", 0).toInt();
    iTimeout[1] = pSettings->value("team2/timeouts", 0).toInt();
    iSet[0]     = pSettings->value("team1/sets", 0).toInt();
    iSet[1]     = pSettings->value("team2/sets", 0).toInt();
    iScore[0]   = pSettings->value("team1/score", 0).toInt();
    iScore[1]   = pSettings->value("team2/score", 0).toInt();
    iServizio   = pSettings->value("set/service", 0).toInt();
    lastService = pSettings->value("set/lastservice", 0).toInt();

    sSlideDir   = pSettings->value("directories/slides", sSlideDir).toString();
    sSpotDir    = pSettings->value("directories/spots", sSpotDir).toString();
}


void
VolleyController::closeEvent(QCloseEvent *event) {
    QString sFunctionName = " Volley_Controller::closeEvent ";
    Q_UNUSED(sFunctionName)
    SaveStatus();
    ScoreController::closeEvent(event);// Propagate the event
}


void
VolleyController::SaveStatus() {
    pSettings->setValue("team1/name", sTeam[0]);
    pSettings->setValue("team2/name", sTeam[1]);
    pSettings->setValue("team1/timeouts", iTimeout[0]);
    pSettings->setValue("team2/timeouts", iTimeout[1]);
    pSettings->setValue("team1/sets", iSet[0]);
    pSettings->setValue("team2/sets", iSet[1]);
    pSettings->setValue("team1/score", iScore[0]);
    pSettings->setValue("team2/score", iScore[1]);
    pSettings->setValue("set/service", iServizio);
    pSettings->setValue("set/lastservice", lastService);
    pSettings->setValue("directories/slides", sSlideDir);
    pSettings->setValue("directories/spots", sSpotDir);
}


QHBoxLayout*
VolleyController::CreateGameButtons() {
    QHBoxLayout* gameButtonLayout = new QHBoxLayout();
    QPixmap pixmap(":/buttonIcons/ExchangeVolleyField.png");
    QIcon ButtonIcon(pixmap);

    changeFieldButton = new QPushButton(ButtonIcon, "");
    changeFieldButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/New-Game-Volley.png");
    ButtonIcon.addPixmap(pixmap);
    newGameButton = new QPushButton(ButtonIcon, "");
    newGameButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/New-Set-Volley.png");
    ButtonIcon.addPixmap(pixmap);
    newSetButton  = new QPushButton(ButtonIcon, "");
    newSetButton->setIconSize(pixmap.rect().size());

    gameButtonLayout->addWidget(newSetButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(newGameButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(changeFieldButton);
    gameButtonLayout->addStretch();
    return gameButtonLayout;
}


QGridLayout*
VolleyController::CreateGamePanel() {
    QGridLayout* gamePanel = new QGridLayout();
    // Team
    int iRow;
    for(int iTeam=0; iTeam<2; iTeam++) {
        // Matrice x righe e 8 colonne
        iRow = 0;
        gamePanel->addWidget(teamName[iTeam], iRow, iTeam*4, 1, 4);
        int iCol = iTeam*5;
        iRow += 1;
        gamePanel->addWidget(timeoutDecrement[iTeam], iRow, iCol,   1, 1, Qt::AlignRight);
        gamePanel->addWidget(timeoutEdit[iTeam],      iRow, iCol+1, 1, 1, Qt::AlignHCenter|Qt::AlignVCenter);
        gamePanel->addWidget(timeoutIncrement[iTeam], iRow, iCol+2, 1, 1, Qt::AlignLeft);
        iRow += 1;
        gamePanel->addWidget(setsDecrement[iTeam], iRow, iCol,   1, 1, Qt::AlignRight);
        gamePanel->addWidget(setsEdit[iTeam],      iRow, iCol+1, 1, 1, Qt::AlignHCenter|Qt::AlignVCenter);
        gamePanel->addWidget(setsIncrement[iTeam], iRow, iCol+2, 1, 1, Qt::AlignLeft);
        iRow += 1;
        gamePanel->addWidget(service[iTeam],   iRow, iCol, 1, 4, Qt::AlignHCenter|Qt::AlignVCenter);
        iRow += 1;
        gamePanel->addWidget(scoreDecrement[iTeam], iRow, iCol,   2, 1, Qt::AlignRight);
        gamePanel->addWidget(scoreEdit[iTeam],      iRow, iCol+1, 2, 1, Qt::AlignHCenter|Qt::AlignVCenter);
        gamePanel->addWidget(scoreIncrement[iTeam], iRow, iCol+2, 2, 1, Qt::AlignLeft);
        iRow++;
    }

    gamePanel->addWidget(timeoutLabel, 1, 3, 1, 2);
    gamePanel->addWidget(setsLabel, 2, 3, 1, 2);
    gamePanel->addWidget(serviceLabel, 3, 3, 1, 2);
    gamePanel->addWidget(scoreLabel, 4, 3, 2, 2);

    return gamePanel;
}


QString
VolleyController::FormatStatusMsg() {
    QString sFunctionName = " Volley_Controller::FormatStatusMsg ";
    Q_UNUSED(sFunctionName)
    QString sMessage = tr("");
    QString sTemp;
    for(int i=0; i<2; i++) {
        sTemp.sprintf("<team%1d>%s</team%1d>", i, sTeam[i].toLocal8Bit().data(), i);
        sMessage += sTemp;
        sTemp.sprintf("<timeout%1d>%d</timeout%1d>", i, iTimeout[i], i);
        sMessage += sTemp;
        sTemp.sprintf("<set%1d>%d</set%1d>", i, iSet[i], i);
        sMessage += sTemp;
        sTemp.sprintf("<score%1d>%d</score%1d>", i, iScore[i], i);
        sMessage += sTemp;
    }
    sTemp.sprintf("<servizio>%d</servizio>", iServizio);
    sMessage += sTemp;
    if(myStatus == showSlides)
        sMessage += "<slideshow>1</slideshow>";
    else if(myStatus == showCamera)
        sMessage += QString("<live>1</live>");
    else if(myStatus == showSpots)
        sMessage += QString("<spotloop>1</spotloop>");

    return sMessage;
}


// =========================
// Event management routines
// =========================


void
VolleyController::onTimeOutIncrement(int iTeam) {
    QString sMessage;
    iTimeout[iTeam]++;
    if(iTimeout[iTeam] == MAX_TIMEOUTS) {
        timeoutIncrement[iTeam]->setEnabled(false);
        timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }
    timeoutDecrement[iTeam]->setEnabled(true);
    sMessage.sprintf("<timeout%1d>%d</timeout%1d>", iTeam, iTimeout[iTeam], iTeam);
    SendToAll(sMessage);
    sMessage.sprintf("<startTimeout>%d</startTimeout>", 30);
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iTimeout[iTeam]);
    timeoutEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/timeouts", iTeam+1);
    pSettings->setValue(sText, iTimeout[iTeam]);
}


void
VolleyController::onTimeOutDecrement(int iTeam) {
    QString sMessage;
    iTimeout[iTeam]--;
    if(iTimeout[iTeam] == 0) {
        timeoutDecrement[iTeam]->setEnabled(false);
    }
    timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
    timeoutIncrement[iTeam]->setEnabled(true);
    sMessage.sprintf("<timeout%1d>%d</timeout%1d>", iTeam, iTimeout[iTeam], iTeam);
    SendToAll(sMessage);
    sMessage.sprintf("<stopTimeout>1</stopTimeout>");
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iTimeout[iTeam]);
    timeoutEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/timeouts", iTeam+1);
    pSettings->setValue(sText, iTimeout[iTeam]);
}


void
VolleyController::setEventHandlers() {
    for(int iTeam=0; iTeam <2; iTeam++) {
        connect(teamName[iTeam], SIGNAL(textChanged(QString, int)),
                this, SLOT(onTeamTextChanged(QString, int)));
        connect(timeoutIncrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onTimeOutIncrement(int)));
        connect(timeoutIncrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(timeoutDecrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onTimeOutDecrement(int)));
        connect(timeoutDecrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(setsIncrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onSetIncrement(int)));
        connect(setsIncrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(setsDecrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onSetDecrement(int)));
        connect(setsDecrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(service[iTeam], SIGNAL(buttonClicked(int, bool)),
                this, SLOT(onServiceClicked(int, bool)));
        connect(service[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(scoreIncrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onScoreIncrement(int)));
        connect(scoreIncrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(scoreDecrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onScoreDecrement(int)));
        connect(scoreDecrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        // New Set
        connect(newSetButton, SIGNAL(clicked(bool)),
                this, SLOT(onButtonNewSetClicked()));
        connect(newSetButton, SIGNAL(clicked()),
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
}


void
VolleyController::onSetIncrement(int iTeam) {
    QString sMessage;
    iSet[iTeam]++;
    setsDecrement[iTeam]->setEnabled(true);
    if(iSet[iTeam] == MAX_SETS) {
        setsIncrement[iTeam]->setEnabled(false);
    }
    sMessage.sprintf("<set%1d>%d</set%1d>", iTeam, iSet[iTeam], iTeam);
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iSet[iTeam]);
    setsEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/sets", iTeam+1);
    pSettings->setValue(sText, iSet[iTeam]);
}


void
VolleyController::onSetDecrement(int iTeam) {
    QString sMessage;
    iSet[iTeam]--;
    setsIncrement[iTeam]->setEnabled(true);
    if(iSet[iTeam] == 0) {
       setsDecrement[iTeam]->setEnabled(false);
    }
    sMessage.sprintf("<set%1d>%d</set%1d>", iTeam, iSet[iTeam], iTeam);
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iSet[iTeam]);
    setsEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/sets", iTeam+1);
    pSettings->setValue(sText, iSet[iTeam]);
}


void
VolleyController::onServiceClicked(int iTeam, bool bChecked) {
    Q_UNUSED(bChecked)
    QString sMessage;
    iServizio = iTeam;
    lastService = iServizio;
    service[iServizio ? 1 : 0]->setChecked(true);
    service[iServizio ? 0 : 1]->setChecked(false);
    sMessage.sprintf("<servizio>%d</servizio>", iServizio);
    SendToAll(sMessage);
    pSettings->setValue("set/service", iServizio);
    pSettings->setValue("set/lastservice", lastService);
}


void
VolleyController::onScoreIncrement(int iTeam) {
    QString sMessage;
    iScore[iTeam]++;
    scoreDecrement[iTeam]->setEnabled(true);
    if(iScore[iTeam] > 98) {
      scoreIncrement[iTeam]->setEnabled(false);
    }
    lastService = iServizio;
    iServizio = iTeam;
    service[iServizio ? 1 : 0]->setChecked(true);
    service[iServizio ? 0 : 1]->setChecked(false);
    sMessage.sprintf("<score%1d>%d</score%1d><servizio>%d</servizio>", iTeam, iScore[iTeam], iTeam, iServizio);
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iScore[iTeam]);
    scoreEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/score", iTeam+1);
    pSettings->setValue(sText, iScore[iTeam]);
}


void
VolleyController::onScoreDecrement(int iTeam) {
    QString sMessage;
    iScore[iTeam]--;
    scoreIncrement[iTeam]->setEnabled(true);
    if(iScore[iTeam] == 0) {
      scoreDecrement[iTeam]->setEnabled(false);
    }
    iServizio = lastService;
    service[iServizio ? 1 : 0]->setChecked(true);
    service[iServizio ? 0 : 1]->setChecked(false);
    sMessage.sprintf("<score%1d>%d</score%1d><servizio>%d</servizio>", iTeam, iScore[iTeam], iTeam, iServizio);
    SendToAll(sMessage);
    QString sText;
    sText.sprintf("%1d", iScore[iTeam]);
    scoreEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/score", iTeam+1);
    pSettings->setValue(sText, iScore[iTeam]);
}


void
VolleyController::onTeamTextChanged(QString sText, int iTeam) {
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
VolleyController::onButtonChangeFieldClicked() {
    int iRes = QMessageBox::question(this, tr("Volley_Controller"),
                                     tr("Scambiare il campo delle squadre ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;

    QString sText = sTeam[0];
    sTeam[0] = sTeam[1];
    sTeam[1] = sText;
    teamName[0]->setText(sTeam[0]);
    teamName[1]->setText(sTeam[1]);

    int iVal = iSet[0];
    iSet[0] = iSet[1];
    iSet[1] = iVal;
    sText.sprintf("%1d", iSet[0]);
    setsEdit[0]->setText(sText);
    sText.sprintf("%1d", iSet[1]);
    setsEdit[1]->setText(sText);

    iVal = iScore[0];
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

    iServizio = 1 - iServizio;
    lastService = 1 -lastService;

    service[iServizio ? 1 : 0]->setChecked(true);
    service[iServizio ? 0 : 1]->setChecked(false);

    for(int iTeam=0; iTeam<2; iTeam++) {
        scoreDecrement[iTeam]->setEnabled(true);
        scoreIncrement[iTeam]->setEnabled(true);
        if(iScore[iTeam] == 0) {
          scoreDecrement[iTeam]->setEnabled(false);
        }
        if(iScore[iTeam] > 98) {
          scoreIncrement[iTeam]->setEnabled(false);
        }

        setsDecrement[iTeam]->setEnabled(true);
        setsIncrement[iTeam]->setEnabled(true);
        if(iSet[iTeam] == 0) {
            setsDecrement[iTeam]->setEnabled(false);
        }
        if(iSet[iTeam] == MAX_SETS) {
            setsIncrement[iTeam]->setEnabled(false);
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
VolleyController::onButtonNewSetClicked() {
    int iRes = QMessageBox::question(this, tr("Volley_Controller"),
                                     tr("Vuoi davvero iniziare un nuovo Set ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;

    // Exchange teams order in the field
    QString sText = sTeam[0];
    sTeam[0] = sTeam[1];
    sTeam[1] = sText;
    teamName[0]->setText(sTeam[0]);
    teamName[1]->setText(sTeam[1]);
    int iVal = iSet[0];
    iSet[0] = iSet[1];
    iSet[1] = iVal;
    sText.sprintf("%1d", iSet[0]);
    setsEdit[0]->setText(sText);
    sText.sprintf("%1d", iSet[1]);
    setsEdit[1]->setText(sText);
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
        setsDecrement[iTeam]->setEnabled(iSet[iTeam] != 0);
        setsIncrement[iTeam]->setEnabled(true);
        scoreDecrement[iTeam]->setEnabled(false);
        scoreIncrement[iTeam]->setEnabled(true);
    }
    iServizio   = 0;
    lastService = 0;
    service[iServizio ? 1 : 0]->setChecked(true);
    service[iServizio ? 0 : 1]->setChecked(false);
    SendToAll(FormatStatusMsg());
    SaveStatus();
}


void
VolleyController::onButtonNewGameClicked() {
    int iRes = QMessageBox::question(this, tr("Volley_Controller"),
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
        iSet[iTeam]   = 0;
        sText.sprintf("%1d", iSet[iTeam]);
        setsEdit[iTeam]->setText(sText);
        iScore[iTeam]   = 0;
        sText.sprintf("%1d", iScore[iTeam]);
        scoreEdit[iTeam]->setText(sText);
        timeoutDecrement[iTeam]->setEnabled(false);
        timeoutIncrement[iTeam]->setEnabled(true);
        setsDecrement[iTeam]->setEnabled(false);
        setsIncrement[iTeam]->setEnabled(true);
        scoreDecrement[iTeam]->setEnabled(false);
        scoreIncrement[iTeam]->setEnabled(true);
    }
    iServizio   = 0;
    lastService = 0;
    service[iServizio ? 1 : 0]->setChecked(true);
    service[iServizio ? 0 : 1]->setChecked(false);
    SendToAll(FormatStatusMsg());
    SaveStatus();
}

