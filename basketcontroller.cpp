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

#include "basketcontroller.h"
#include "edit.h"
#include "button.h"
#include "radioButton.h"
#include "fileserver.h"


#define MAX_TIMEOUTS_1  2 // Numero massimo di sospensioni nella prima metà di gara
#define MAX_TIMEOUTS_2  3 // Numero massimo di sospensioni nella seconda metà di gara
#define MAX_TIMEOUTS_3  1 // Numero massimo di sospensioni negli OVERTIMES
#define MAX_FAULS      99 // Da definire il comportamento dopo il numero max di falli !
#define MAX_PERIODS    99 // Da definire il comportamento dopo il numero max di periodi !
#define BONUS_TARGET    4 // Dopo questo valore il Bonus per il team è finito !
#define GAME_PERIODS    4 // Oltre questo valore ci sono gli OVERTIMES
#define REGULAR_TIME   10 // 10 Minuti è la durata di un tempo regolare
#define OVER_TIME       5 // 5 Minuti è a durata di ciascun overtime


BasketController::BasketController()
    : ScoreController(BASKET_PANEL, Q_NULLPTR)
{
    QString sFunctionName = QString(" BasketController::BasketController ");
    Q_UNUSED(sFunctionName)

    GetSettings();
    PrepareDirectories();

    logFileName = QString("%1score_controller.txt").arg(sLogDir);
    PrepareLogFile();
    prepareServices();

    pSlideUpdaterServer->setDir(sSlideDir,"*.jpg *.jpeg *.png *.JPG *.JPEG *.PNG");
    pSpotUpdaterServer->setDir(sSpotDir, "*.mp4 *.MP4");

    emit startSpotServer();
    emit startSlideServer();

    buildControls();
    QGridLayout *mainLayout = new QGridLayout();

    int gamePanelWidth   = 15;
    int gamePanelHeight  = 8;

    mainLayout->addLayout(CreateGamePanel(),
                          0,
                          0,
                          gamePanelHeight,
                          gamePanelWidth);

    mainLayout->addLayout(CreateGameButtonBox(),
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
    buildFontSizes();
    setEventHandlers();

    possess[iPossess ? 1 : 0]->setChecked(true);
    possess[iPossess ? 0 : 1]->setChecked(false);
}


void
BasketController::buildControls() {
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
//>>>>        if(iTimeout[iTeam] == MAX_TIMEOUTS) {
//>>>>            timeoutIncrement[iTeam]->setEnabled(false);
//>>>>            timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
//>>>>        }
        // Team Fauls
        sString.sprintf("%1d", iFauls[iTeam]);
        faulsEdit[iTeam] = new Edit(sString);
        faulsEdit[iTeam]->setMaxLength(2);
        faulsEdit[iTeam]->setAlignment(Qt::AlignHCenter);
        faulsEdit[iTeam]->setReadOnly(true);
        // Team Fauls buttons
        faulsIncrement[iTeam] = new Button("", iTeam);
        faulsIncrement[iTeam]->setIcon(plusButtonIcon);
        faulsIncrement[iTeam]->setIconSize(plusPixmap.rect().size());
        faulsDecrement[iTeam] = new Button("", iTeam);
        faulsDecrement[iTeam]->setIcon(minusButtonIcon);
        faulsDecrement[iTeam]->setIconSize(minusPixmap.rect().size());
        if(iFauls[iTeam] == 0)
            faulsDecrement[iTeam]->setEnabled(false);
        if(iFauls[iTeam] == MAX_FAULS)
            faulsIncrement[iTeam]->setEnabled(false);
        // Possess
        possess[iTeam] = new RadioButton(tr(" "), iTeam);
        // Bonus
        bonusEdit[iTeam] = new Edit(QString(tr("Bonus")));
        bonusEdit[iTeam]->setFrame(false);
        bonusEdit[iTeam]->setAlignment(Qt::AlignHCenter);
        bonusEdit[iTeam]->setReadOnly(true);
        if(iFauls[iTeam] < BONUS_TARGET) {
            bonusEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        else {
            bonusEdit[iTeam]->setStyleSheet("background:transparent;color:transparent;");
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
    periodIncrement = new Button("", 0);
    periodIncrement->setIcon(plusButtonIcon);
    periodIncrement->setIconSize(plusPixmap.rect().size());
    periodDecrement = new Button("", 0);
    periodDecrement->setIcon(minusButtonIcon);
    periodDecrement->setIconSize(minusPixmap.rect().size());
    if(iPeriod < 2)
        periodDecrement->setDisabled(true);

    //Labels:

    // Timeout
    timeoutLabel = new QLabel(tr("Timeout"));
    timeoutLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    // Team Fauls
    faulsLabel = new QLabel(tr("Team Fauls"));
    faulsLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    // Score
    scoreLabel = new QLabel(tr("Score"));
    scoreLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    // Period
    periodLabel = new QLabel(tr("Period"));
    periodLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    // Posses
    possessLabel = new QLabel(tr("Possess"));
    possessLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}


void
BasketController::buildFontSizes() {
    int rW, rH;
    QFont font;
    // Teams
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
    // Timeout
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
    // Fauls
    font = faulsEdit[0]->font();
    iFaulsFontSize  = QFontMetrics(font).maxWidth();
    rH = QFontMetrics(font).height();
    for(int i=iFaulsFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).maxWidth()*2;
        rH = QFontMetrics(font).height();
        if((rW > faulsEdit[0]->width()) || (rH > faulsEdit[0]->height())){
            iFaulsFontSize = i-1;
            break;
        }
    }
    font.setPixelSize(iFaulsFontSize);
    faulsEdit[0]->setFont(font);
    faulsEdit[1]->setFont(font);
    // Bonus
    font = bonusEdit[0]->font();
    iBonusEditFontSize =  QFontMetrics(font).maxWidth();
    rH = QFontMetrics(font).height();
    for(int i=iBonusEditFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).maxWidth()*bonusEdit[0]->text().length();
        rH = QFontMetrics(font).height();
        if((rW > bonusEdit[0]->width()) || (rH > bonusEdit[0]->height())){
            iBonusEditFontSize = i-1;
            break;
        }
    }
    font.setPixelSize(iBonusEditFontSize);
    bonusEdit[0]->setFont(font);
    bonusEdit[1]->setFont(font);
    // Score
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

    //Labels:
    // Period
    font = periodLabel->font();
    iPeriodLabelFontSize = QFontMetrics(font).maxWidth();
    for(int i=iPeriodLabelFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).width(periodLabel->text());
        rH = QFontMetrics(font).height();
        if((rW > periodLabel->width()) || (rH > periodLabel->height())){
            iPeriodLabelFontSize = i-1;
            break;
        }
    }
    font.setPixelSize(iPeriodLabelFontSize);
    periodLabel->setFont(font);
    periodEdit->setFont(font);

    // Timeout
    font = timeoutLabel->font();
    iLabelFontSize = QFontMetrics(font).maxWidth();
    rH = QFontMetrics(font).height();
    for(int i=iLabelFontSize; i<100; i++) {
        font.setPixelSize(i);
        rW = QFontMetrics(font).width(timeoutLabel->text());
        rH = QFontMetrics(font).height();
        if((rW > timeoutLabel->width()) || (rH > timeoutLabel->height())){
            iScoreFontSize = i-1;
            break;
        }
    }
    font.setPixelSize(iLabelFontSize);
    timeoutLabel->setFont(font);
    faulsLabel->setFont(font);
    possess[0]->setFont(font);
    possess[1]->setFont(font);
    possessLabel->setFont(font);
    font.setWeight(QFont::Black);
    scoreLabel->setFont(font);
}


void
BasketController::setEventHandlers() {
    for(int iTeam=0; iTeam<2; iTeam++) {
        // Team
        connect(teamName[iTeam], SIGNAL(textChanged(QString, int)),
                this, SLOT(onTeamTextChanged(QString, int)));
        // Timeout
        connect(timeoutIncrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onTimeOutIncrement(int)));
        connect(timeoutIncrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(timeoutDecrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onTimeOutDecrement(int)));
        connect(timeoutDecrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        // Fauls
        connect(faulsIncrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onFaulsIncrement(int)));
        connect(faulsIncrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(faulsDecrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onFaulsDecrement(int)));
        connect(faulsDecrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        // Period
        connect(periodIncrement, SIGNAL(buttonClicked(int)),
                this, SLOT(onPeriodIncrement(int)));
        connect(periodIncrement, SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(periodDecrement, SIGNAL(buttonClicked(int)),
                this, SLOT(onPeriodDecrement(int)));
        connect(periodDecrement, SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        // Possess
        connect(possess[iTeam], SIGNAL(buttonClicked(int, bool)), this, SLOT(onPossessClicked(int, bool)));
        // Score
        connect(scoreIncrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onScoreIncrement(int)));
        connect(scoreIncrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        connect(scoreDecrement[iTeam], SIGNAL(buttonClicked(int)),
                this, SLOT(onScoreDecrement(int)));
        connect(scoreDecrement[iTeam], SIGNAL(clicked()),
                pButtonClick, SLOT(play()));
        // Buttons
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
    }
}


void
BasketController::GetSettings() {
    QString sFunctionName = QString(" BasketController::GetSettings ");
    Q_UNUSED(sFunctionName)
    pSettings = new QSettings("Gabriele Salvato", "Basket Controller");

    sTeam[0]    = pSettings->value("team1/name", QString(tr("Locali"))).toString();
    sTeam[1]    = pSettings->value("team2/name", QString(tr("Ospiti"))).toString();
    iTimeout[0] = pSettings->value("team1/timeouts", 0).toInt();
    iTimeout[1] = pSettings->value("team2/timeouts", 0).toInt();
    iScore[0]   = pSettings->value("team1/score", 0).toInt();
    iScore[1]   = pSettings->value("team2/score", 0).toInt();
    iFauls[0]   = pSettings->value("team1/fauls", 0).toInt();
    iFauls[1]   = pSettings->value("team2/fauls", 0).toInt();

    iPeriod     = pSettings->value("game/period", 1).toInt();
    iPossess    = pSettings->value("game/possess", 0).toInt();

    sSlideDir   = pSettings->value("directories/slides", sSlideDir).toString();
    sSpotDir    = pSettings->value("directories/spots", sSpotDir).toString();

    for(int iTeam=0; iTeam<2; iTeam++) {
        if(iFauls[iTeam] < BONUS_TARGET) {
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


QHBoxLayout*
BasketController::CreateGameButtonBox() {
    QHBoxLayout* gameButtonLayout = new QHBoxLayout();
    QPixmap pixmap(":/buttonIcons/ExchangeVolleyField.png");
    QIcon ButtonIcon(pixmap);

    changeFieldButton = new QPushButton(ButtonIcon, "");
    changeFieldButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/New-Set-Volley.png");
    ButtonIcon.addPixmap(pixmap);
    newPeriodButton   = new QPushButton(ButtonIcon, "");
    newPeriodButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/New-Game-Volley.png");
    ButtonIcon.addPixmap(pixmap);
    newGameButton = new QPushButton(ButtonIcon, "");
    newGameButton->setIconSize(pixmap.rect().size());

    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(newPeriodButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(newGameButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(changeFieldButton);
    gameButtonLayout->addStretch();
    return gameButtonLayout;
}


QGridLayout*
BasketController::CreateGamePanel() {
    QGridLayout* gamePanel = new QGridLayout();
    //
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
        gamePanel->addWidget(faulsDecrement[iTeam], iRow, iCol,   1, 1, Qt::AlignRight);
        gamePanel->addWidget(faulsEdit[iTeam],      iRow, iCol+1, 1, 1, Qt::AlignHCenter|Qt::AlignVCenter);
        gamePanel->addWidget(faulsIncrement[iTeam], iRow, iCol+2, 1, 1, Qt::AlignLeft);
        iRow += 1;
        if(iTeam == 0) {
            gamePanel->addWidget(possess[iTeam],   iRow, 2, 1, 1, Qt::AlignRight|Qt::AlignVCenter);
        } else {
            gamePanel->addWidget(possess[iTeam],   iRow, 5, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
        }
        iRow += 2;// Leave space for Bonus & Period
        gamePanel->addWidget(scoreDecrement[iTeam], iRow, iCol,   2, 1, Qt::AlignRight);
        gamePanel->addWidget(scoreEdit[iTeam],      iRow, iCol+1, 2, 1, Qt::AlignHCenter|Qt::AlignVCenter);
        gamePanel->addWidget(scoreIncrement[iTeam], iRow, iCol+2, 2, 1, Qt::AlignLeft);
    }
    gamePanel->addWidget(timeoutLabel,  1, 3, 1, 2, Qt::AlignHCenter|Qt::AlignVCenter);
    gamePanel->addWidget(faulsLabel,    2, 3, 1, 2, Qt::AlignHCenter|Qt::AlignVCenter);
    gamePanel->addWidget(possessLabel,  3, 3, 1, 2, Qt::AlignHCenter|Qt::AlignVCenter);
    gamePanel->addWidget(bonusEdit[0],  3, 0, 1, 2, Qt::AlignLeft|Qt::AlignVCenter);
    gamePanel->addWidget(bonusEdit[1],  3, 6, 1, 2, Qt::AlignRight|Qt::AlignVCenter);

    gamePanel->addWidget(periodLabel,      4, 0, 1, 2, Qt::AlignRight|Qt::AlignVCenter);
    gamePanel->addWidget(periodDecrement,  4, 2, 1, 1, Qt::AlignRight);
    gamePanel->addWidget(periodEdit,       4, 3, 1, 2, Qt::AlignHCenter|Qt::AlignVCenter);
    gamePanel->addWidget(periodIncrement,  4, 5, 1, 1, Qt::AlignLeft);

    gamePanel->addWidget(scoreLabel,       5, 3, 2, 2, Qt::AlignHCenter|Qt::AlignVCenter);

    return gamePanel;
}


QString
BasketController::FormatStatusMsg() {
    QString sFunctionName = " BasketController::FormatStatusMsg ";
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
        sTemp.sprintf("<fauls%1d>%d</fauls%1d>", i, iFauls[i], i);
        sMessage += sTemp;
        sTemp.sprintf("<bonus%1d>%d</bonus%1d>", i, iBonus[i], i);
        sMessage += sTemp;
    }
    if(iPeriod > GAME_PERIODS)
        sTemp.sprintf("<period>%d,%d</period>", iPeriod, OVER_TIME);
    else
        sTemp.sprintf("<period>%d,%d</period>", iPeriod, REGULAR_TIME);
    sMessage += sTemp;
    sTemp.sprintf("<possess>%d</possess>", iPossess);
    sMessage += sTemp;
    if(!startStopSlideShowButton->text().contains(QString("Avvia")))
        sMessage += "<slideshow>1</slideshow>";
    else if(!startStopLiveCameraButton->text().contains(QString("Avvia")))
        sMessage += QString("<live>1</live>");
    else if(!startStopLoopSpotButton->text().contains(QString("Avvia")))
        sMessage += QString("<spotloop>1</spotloop>");

    return sMessage;
}


// =========================
// Event management routines
// =========================


void
BasketController::onTimeOutIncrement(int iTeam) {
    QString sMessage;
    iTimeout[iTeam]++;
    if((iPeriod < 3) && (iTimeout[iTeam] == MAX_TIMEOUTS_1)) {
        timeoutIncrement[iTeam]->setEnabled(false);
        timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }
    else if((iPeriod > GAME_PERIODS) && (iTimeout[iTeam] == MAX_TIMEOUTS_3)) {
        timeoutIncrement[iTeam]->setEnabled(false);
        timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }
    else if((iPeriod > 2) && (iTimeout[iTeam] == MAX_TIMEOUTS_2)) {
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
    if(iFauls[iTeam] < BONUS_TARGET) {
        iBonus[iTeam] = 1;
        bonusEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }
    else {
        iBonus[iTeam] = 0;
        bonusEdit[iTeam]->setStyleSheet("background:transparent;color:transparent;");
    }
    faulsDecrement[iTeam]->setEnabled(true);

    sMessage.sprintf("<fauls%1d>%d</fauls%1d>", iTeam, iFauls[iTeam], iTeam);
    sText.sprintf("<bonus%1d>%d</bonus%1d>", iTeam, iBonus[iTeam], iTeam);
    sMessage += sText;
    SendToAll(sMessage);
    sText.sprintf("%2d", iFauls[iTeam]);
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
    if(iFauls[iTeam] < BONUS_TARGET) {
        iBonus[iTeam] = 1;
        bonusEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }
    else {
        iBonus[iTeam] = 0;
        bonusEdit[iTeam]->setStyleSheet("background:transparent;color:transparent;");
    }
    faulsIncrement[iTeam]->setEnabled(true);

    sMessage.sprintf("<fauls%1d>%d</fauls%1d>", iTeam, iFauls[iTeam], iTeam);
    sText.sprintf("<bonus%1d>%d</bonus%1d>", iTeam, iBonus[iTeam], iTeam);
    sMessage += sText;
    SendToAll(sMessage);
    sText.sprintf("%2d", iFauls[iTeam]);
    faulsEdit[iTeam]->setText(sText);
    sText.sprintf("team%1d/fauls", iTeam+1);
    pSettings->setValue(sText, iFauls[iTeam]);
}


void
BasketController::onScoreIncrement(int iTeam) {
    QString sMessage;
    iScore[iTeam]++;
    scoreDecrement[iTeam]->setEnabled(true);
    if(iScore[iTeam] > 998) {
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

    // Exchange teams order, score, timeouts and team fauls
    QString sText = sTeam[0];
    sTeam[0] = sTeam[1];
    sTeam[1] = sText;

    int iVal = iScore[0];
    iScore[0] = iScore[1];
    iScore[1] = iVal;

    iVal = iTimeout[0];
    iTimeout[0] = iTimeout[1];
    iTimeout[1] = iVal;

    iVal = iFauls[0];
    iFauls[0] = iFauls[1];
    iFauls[1] = iVal;
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
        if(iScore[iTeam] > 98) {
          scoreIncrement[iTeam]->setEnabled(false);
        }
        sText.sprintf("%1d", iTimeout[iTeam]);
        timeoutEdit[iTeam]->setText(sText);
        timeoutIncrement[iTeam]->setEnabled(true);
        timeoutDecrement[iTeam]->setEnabled(true);
        timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
        if((iPeriod < 3) && (iTimeout[iTeam] == MAX_TIMEOUTS_1)) {
            timeoutIncrement[iTeam]->setEnabled(false);
            timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        else if((iPeriod > GAME_PERIODS) && (iTimeout[iTeam] == MAX_TIMEOUTS_3)) {
            timeoutIncrement[iTeam]->setEnabled(false);
            timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        else if((iPeriod > 2) && (iTimeout[iTeam] == MAX_TIMEOUTS_2)) {
            timeoutIncrement[iTeam]->setEnabled(false);
            timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        if(iTimeout[iTeam] == 0) {
            timeoutDecrement[iTeam]->setEnabled(false);
        }
        sText.sprintf("%2d", iFauls[iTeam]);
        faulsEdit[iTeam]->setText(sText);
        if(iFauls[iTeam] == 0) {
           faulsDecrement[iTeam]->setEnabled(false);
        }
        if(iFauls[iTeam] == MAX_FAULS) {// To be changed
            faulsIncrement[iTeam]->setEnabled(false);
        }
        if(iFauls[iTeam] < BONUS_TARGET) {
            iBonus[iTeam] = 1;
            bonusEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        else {
            iBonus[iTeam] = 0;
            bonusEdit[iTeam]->setStyleSheet("background:transparent;color:transparent;");
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

    // Increment period number
    if(iPeriod < MAX_PERIODS) {
        iPeriod++;
    }
    if(iPeriod >= MAX_PERIODS) {
        periodIncrement->setDisabled(true);
        iPeriod= MAX_PERIODS;
    }
    periodDecrement->setEnabled(true);
    QString sString;
    sString.sprintf("%2d", iPeriod);
    periodEdit->setText(sString);

    // Exchange teams order, score and team fauls
    QString sText = sTeam[0];
    sTeam[0] = sTeam[1];
    sTeam[1] = sText;
    int iVal = iScore[0];
    iScore[0] = iScore[1];
    iScore[1] = iVal;
    if(iPeriod > GAME_PERIODS) {// Art. 41.1.3 - Tutti i falli di squadra commessi in un tempo
                                //supplementare devono essere considerati come avvenuti nel quarto periodo.
        iVal = iFauls[0];
        iFauls[0] = iFauls[1];
        iFauls[1] = iVal;
     }
    else {
        iFauls[0] = iFauls[1] = 0;
    }
    // Update panel
    for(int iTeam=0; iTeam<2; iTeam++) {
        teamName[iTeam]->setText(sTeam[iTeam]);
        iTimeout[iTeam] = 0;
        sText.sprintf("%1d", iTimeout[iTeam]);
        timeoutEdit[iTeam]->setText(sText);
        timeoutEdit[iTeam]->setStyleSheet("background:white;color:black;");
        timeoutDecrement[iTeam]->setEnabled(false);
        timeoutIncrement[iTeam]->setEnabled(true);
        sText.sprintf("%3d", iScore[iTeam]);
        scoreEdit[iTeam]->setText(sText);
        sText.sprintf("%2d", iFauls[iTeam]);
        faulsEdit[iTeam]->setText(sText);
        if(iFauls[iTeam] == 0) {
           faulsDecrement[iTeam]->setEnabled(false);
        }
        if(iFauls[iTeam] == MAX_FAULS) {// To be changed
            faulsIncrement[iTeam]->setEnabled(false);
        }
        if(iFauls[iTeam] < BONUS_TARGET) {
            iBonus[iTeam] = 1;
            bonusEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        else {
            iBonus[iTeam] = 0;
            bonusEdit[iTeam]->setStyleSheet("background:transparent;color:transparent;");
        }
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
        iFauls[iTeam] = 0;
        sText.sprintf("%2d", iFauls[iTeam]);
        faulsEdit[iTeam]->setText(sText);
        faulsIncrement[iTeam]->setEnabled(true);
        faulsDecrement[iTeam]->setEnabled(false);
        bonusEdit[iTeam]->setStyleSheet("background:red;color:white;");
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
    if(iPeriod > GAME_PERIODS)
        sMessage.sprintf("<period>%d,%d</period>", iPeriod, OVER_TIME);
    else
        sMessage.sprintf("<period>%d,%d</period>", iPeriod, REGULAR_TIME);
    SendToAll(sMessage);
    pSettings->setValue("game/period", iPeriod);
}


void
BasketController::onPeriodDecrement(int iDummy) {
    Q_UNUSED(iDummy)
    if(iPeriod > 1) {
        iPeriod--;
    }
    if(iPeriod < 2)
        periodDecrement->setDisabled(true);
    if(iPeriod >= MAX_PERIODS) {
        periodIncrement->setDisabled(true);
        iPeriod= MAX_PERIODS;
    }
    periodIncrement->setEnabled(true);
    QString sString, sMessage;
    sString.sprintf("%2d", iPeriod);
    periodEdit->setText(sString);
    if(iPeriod > GAME_PERIODS)
        sMessage.sprintf("<period>%d,%d</period>", iPeriod, OVER_TIME);
    else
        sMessage.sprintf("<period>%d,%d</period>", iPeriod, REGULAR_TIME);
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


