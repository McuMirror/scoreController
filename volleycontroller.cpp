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
#include "generalsetupdialog.h"



/*!
 * \brief VolleyController::VolleyController
 * The constructor of the Volley Control Panel.
 *
 * It is responsible to start the various services for
 * updating Slide and Spots
 */
VolleyController::VolleyController(QString sMyLanguage)
    : ScoreController(VOLLEY_PANEL, Q_NULLPTR)
    , bFontBuilt(false)
{
    sLanguage = sMyLanguage;
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

    setEventHandlers();

    service[iServizio ? 1 : 0]->setChecked(true);
    service[iServizio ? 0 : 1]->setChecked(false);
}


/*!
 * \brief VolleyController::buildControls
 * Utility member to create all the controls on the Control Panel
 */
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
        sString = QString("%1").arg(iTimeout[iTeam], 1);
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
        if(iTimeout[iTeam] == pGeneralSetupDialog->getNumTimeoutVY()) {
            timeoutIncrement[iTeam]->setEnabled(false);
            timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
        }
        // Sets
        sString = QString("%1").arg(iSet[iTeam], 1);
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
        if(iSet[iTeam] == pGeneralSetupDialog->getNumSetVY())
            setsIncrement[iTeam]->setEnabled(false);
        // Service
        service[iTeam] = new RadioButton(" ", iTeam);// Android requires at least one character to work
        // Score
        scoreLabel = new QLabel(tr("Punti"));
        scoreLabel->setAlignment(Qt::AlignRight|Qt::AlignHCenter);
        sString = QString("%1").arg(iScore[iTeam], 2);
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


/*!
 * \brief VolleyController::resizeEvent
 * The very first time it is called it is resposible to resize the fonts used
 * \param event
 */
void
VolleyController::resizeEvent(QResizeEvent *event) {
    if(!bFontBuilt) {
        bFontBuilt = true;
        buildFontSizes();
        event->setAccepted(true);
    }
}


/*!
 * \brief VolleyController::buildFontSizes
 * Utility member to calculate the font sizes for the various controls
 */
void
VolleyController::buildFontSizes() {
    QFont font;
    int iFontSize;
    int hMargin, vMargin;
    QMargins margins;

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

    font = setsEdit[0]->font();
    margins = setsEdit[0]->contentsMargins();
    vMargin = margins.bottom() + margins.top();
    hMargin = margins.left() + margins.right();
    iFontSize = qMin((setsEdit[0]->width()/setsEdit[0]->maxLength())-2*hMargin,
                     setsEdit[0]->height()-vMargin);
    font.setPixelSize(iFontSize);
    setsEdit[0]->setFont(font);
    setsEdit[1]->setFont(font);

    font = timeoutEdit[0]->font();
    margins = timeoutEdit[0]->contentsMargins();
    vMargin = margins.bottom() + margins.top();
    hMargin = margins.left() + margins.right();
    iFontSize = qMin((timeoutEdit[0]->width()/timeoutEdit[0]->maxLength())-2*hMargin,
                     timeoutEdit[0]->height()-vMargin);
    font.setPixelSize(iFontSize);
    timeoutEdit[0]->setFont(font);
    timeoutEdit[1]->setFont(font);

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

    font = timeoutLabel->font();
    margins = timeoutLabel->contentsMargins();
    vMargin = margins.bottom() + margins.top();
    hMargin = margins.left() + margins.right();
    iFontSize = qMin((timeoutLabel->width()/timeoutLabel->text().length())-2*hMargin,
                     timeoutLabel->height()-vMargin);
    font.setPixelSize(iFontSize);

    timeoutLabel->setFont(font);
    setsLabel->setFont(font);
    serviceLabel->setFont(font);
    font.setWeight(QFont::Black);
    scoreLabel->setFont(font);
}


/*!
 * \brief VolleyController::GetSettings
 * Recall from the non volatile memory the last values of the controls
 */
void
VolleyController::GetSettings() {
    pSettings = new QSettings("Gabriele Salvato", "Volley Controller");

    sTeam[0]    = pSettings->value("team1/name", QString(tr("Locali"))).toString();
    sTeam[1]    = pSettings->value("team2/name", QString(tr("Ospiti"))).toString();
    iTimeout[0] = pSettings->value("team1/timeouts", 0).toInt();
    iTimeout[1] = pSettings->value("team2/timeouts", 0).toInt();
    if(iTimeout[0] > pGeneralSetupDialog->getNumTimeoutVY())
        iTimeout[0] = pGeneralSetupDialog->getNumTimeoutVY();
    if(iTimeout[1] > pGeneralSetupDialog->getNumTimeoutVY())
        iTimeout[1] = pGeneralSetupDialog->getNumTimeoutVY();
    iSet[0]     = pSettings->value("team1/sets", 0).toInt();
    iSet[1]     = pSettings->value("team2/sets", 0).toInt();
    if(iSet[0] > pGeneralSetupDialog->getNumSetVY())
        iSet[0] = pGeneralSetupDialog->getNumSetVY();
    if(iSet[1] > pGeneralSetupDialog->getNumSetVY())
        iSet[1] = pGeneralSetupDialog->getNumSetVY();
    iScore[0]   = pSettings->value("team1/score", 0).toInt();
    iScore[1]   = pSettings->value("team2/score", 0).toInt();
    iServizio   = pSettings->value("set/service", 0).toInt();
    lastService = pSettings->value("set/lastservice", 0).toInt();

    sSlideDir   = pSettings->value("directories/slides", sSlideDir).toString();
    sSpotDir    = pSettings->value("directories/spots", sSpotDir).toString();
}


/*!
 * \brief VolleyController::closeEvent
 * To handle the closure of the Panel Controller
 * \param event unused
 */
void
VolleyController::closeEvent(QCloseEvent *event) {
    SaveStatus();
    ScoreController::closeEvent(event);// Propagate the event
}


/*!
 * \brief VolleyController::SaveStatus
 * Save the values of the controls into the non volatile memory
 */
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


/*!
 * \brief VolleyController::CreateGameButtons
 * Utility member to create the Layout of the Buttons
 * \return the Layout
 */
QHBoxLayout*
VolleyController::CreateGameButtons() {
    auto* gameButtonLayout = new QHBoxLayout();

    QPixmap pixmap(":/buttonIcons/ExchangeVolleyField.png");
    QIcon ButtonIcon(pixmap);
    changeFieldButton = new QPushButton(ButtonIcon, "");
    changeFieldButton->setIconSize(pixmap.rect().size());
    changeFieldButton->setToolTip("Inverti Campo");

    pixmap.load(":/buttonIcons/New-Game-Volley.png");
    ButtonIcon.addPixmap(pixmap);
    newGameButton = new QPushButton(ButtonIcon, "");
    newGameButton->setIconSize(pixmap.rect().size());
    newGameButton->setToolTip("Nuova Partita");

    pixmap.load(":/buttonIcons/New-Set-Volley.png");
    ButtonIcon.addPixmap(pixmap);
    newSetButton  = new QPushButton(ButtonIcon, "");
    newSetButton->setIconSize(pixmap.rect().size());
    newSetButton->setToolTip("Nuovo Set");

    gameButtonLayout->addWidget(newGameButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(newSetButton);
    gameButtonLayout->addStretch();
    gameButtonLayout->addWidget(changeFieldButton);
    gameButtonLayout->addStretch();
    return gameButtonLayout;
}


/*!
 * \brief VolleyController::CreateGamePanel
 * Utility member to create the Layout of the Control Panel
 * \return The Layout
 * \return
 */
QGridLayout*
VolleyController::CreateGamePanel() {
    auto* gamePanel = new QGridLayout();
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


/*!
 * \brief VolleyController::FormatStatusMsg
 * Utility function to format a String containing the current status of the controls
 * \return The "Current Status Message"
 */
QString
VolleyController::FormatStatusMsg() {
    QString sMessage = tr("");
    QString sTemp;
    for(int i=0; i<2; i++) {
        sTemp = QString("<team%1>%2</team%3>").arg(i,1).arg(sTeam[i].toLocal8Bit().data()).arg(i,1);
        sMessage += sTemp;
        sTemp = QString("<timeout%1>%2</timeout%3>").arg(i,1).arg(iTimeout[i]).arg(i,1);
        sMessage += sTemp;
        sTemp = QString("<set%1>%2</set%3>").arg(i,1).arg(iSet[i]).arg(i,1);
        sMessage += sTemp;
        sTemp = QString("<score%1>%2</score%3>").arg(i,1).arg(iScore[i], 2).arg(i,1);
        sMessage += sTemp;
    }
    sTemp = QString("<servizio>%1</servizio>").arg(iServizio, 1);
    sMessage += sTemp;
    if(myStatus == showSlides)
        sMessage += QString("<slideshow>1</slideshow>");
    else if(myStatus == showCamera)
        sMessage += QString("<live>1</live>");
    else if(myStatus == showSpots)
        sMessage += QString("<spotloop>1</spotloop>");

    sMessage += QString("<language>%1</language>").arg(sLanguage);
    return sMessage;
}


// =========================
// Event management routines
// =========================

/*!
 * \brief VolleyController::onTimeOutIncrement
 * \param iTeam
 */
void
VolleyController::onTimeOutIncrement(int iTeam) {
    QString sMessage;
    iTimeout[iTeam]++;
    if(iTimeout[iTeam] >= pGeneralSetupDialog->getNumTimeoutVY()) {
        timeoutIncrement[iTeam]->setEnabled(false);
        timeoutEdit[iTeam]->setStyleSheet("background:red;color:white;");
    }
    timeoutDecrement[iTeam]->setEnabled(true);
    sMessage = QString("<timeout%1>%2</timeout%3>")
              .arg(iTeam, 1)
              .arg(iTimeout[iTeam]).arg(iTeam, 1);
    SendToAll(sMessage);
    sMessage = QString("<startTimeout>%1</startTimeout>")
               .arg(pGeneralSetupDialog->getTimeoutDurationVY());
    SendToAll(sMessage);
    QString sText;
    sText = QString("%1").arg(iTimeout[iTeam]);
    timeoutEdit[iTeam]->setText(sText);
    sText = QString("team%1/timeouts").arg(iTeam+1, 1);
    pSettings->setValue(sText, iTimeout[iTeam]);
}


/*!
 * \brief VolleyController::onTimeOutDecrement
 * \param iTeam
 */
void
VolleyController::onTimeOutDecrement(int iTeam) {
    QString sMessage;
    iTimeout[iTeam]--;
    if(iTimeout[iTeam] == 0) {
        timeoutDecrement[iTeam]->setEnabled(false);
    }
    timeoutEdit[iTeam]->setStyleSheet(styleSheet());
    timeoutIncrement[iTeam]->setEnabled(true);
    sMessage = QString("<timeout%1>%2</timeout%3>").arg(iTeam, 1).arg(iTimeout[iTeam]).arg(iTeam, 1);
    SendToAll(sMessage);
    sMessage = QString("<stopTimeout>1</stopTimeout>");
    SendToAll(sMessage);
    QString sText;
    sText = QString("%1").arg(iTimeout[iTeam], 1);
    timeoutEdit[iTeam]->setText(sText);
    sText = QString("team%1/timeouts").arg(iTeam+1, 1);
    pSettings->setValue(sText, iTimeout[iTeam]);
}


/*!
 * \brief VolleyController::setEventHandlers
 * Utility member to connect the various controls with their event handlers
 */
void
VolleyController::setEventHandlers() {
    for(int iTeam=0; iTeam <2; iTeam++) {
        connect(teamName[iTeam], SIGNAL(textChanged(QString,int)),
                this, SLOT(onTeamTextChanged(QString,int)));
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
        connect(service[iTeam], SIGNAL(buttonClicked(int,bool)),
                this, SLOT(onServiceClicked(int,bool)));
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
    }
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


/*!
 * \brief VolleyController::onSetIncrement
 * \param iTeam
 */
void
VolleyController::onSetIncrement(int iTeam) {
    QString sMessage;
    iSet[iTeam]++;
    setsDecrement[iTeam]->setEnabled(true);
    if(iSet[iTeam] == pGeneralSetupDialog->getNumSetVY()) {
        setsIncrement[iTeam]->setEnabled(false);
    }
    sMessage = QString("<set%1>%2</set%3>").arg(iTeam, 1).arg(iSet[iTeam]).arg(iTeam, 1);
    SendToAll(sMessage);
    QString sText;
    sText = QString("%1").arg(iSet[iTeam], 1);
    setsEdit[iTeam]->setText(sText);
    sText = QString("team%1/sets").arg(iTeam+1, 1);
    pSettings->setValue(sText, iSet[iTeam]);
}


/*!
 * \brief VolleyController::onSetDecrement
 * \param iTeam
 */
void
VolleyController::onSetDecrement(int iTeam) {
    QString sMessage;
    iSet[iTeam]--;
    setsIncrement[iTeam]->setEnabled(true);
    if(iSet[iTeam] == 0) {
       setsDecrement[iTeam]->setEnabled(false);
    }
    sMessage= QString("<set%1>%2</set%3>").arg(iTeam, 1).arg(iSet[iTeam]).arg(iTeam, 1);
    SendToAll(sMessage);
    QString sText;
    sText = QString("%1").arg(iSet[iTeam], 1);
    setsEdit[iTeam]->setText(sText);
    sText = QString("team%1/sets").arg(iTeam+1, 1);
    pSettings->setValue(sText, iSet[iTeam]);
}


/*!
 * \brief VolleyController::onServiceClicked
 * \param iTeam
 * \param bChecked
 */
void
VolleyController::onServiceClicked(int iTeam, bool bChecked) {
    Q_UNUSED(bChecked)
    QString sMessage;
    iServizio = iTeam;
    lastService = iServizio;
    service[iServizio ? 1 : 0]->setChecked(true);
    service[iServizio ? 0 : 1]->setChecked(false);
    sMessage = QString("<servizio>%1</servizio>").arg(iServizio);
    SendToAll(sMessage);
    pSettings->setValue("set/service", iServizio);
    pSettings->setValue("set/lastservice", lastService);
}


/*!
 * \brief VolleyController::onScoreIncrement
 * \param iTeam
 */
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
    sMessage = QString("<score%1>%2</score%3><servizio>%4</servizio>")
               .arg(iTeam, 1)
               .arg(iScore[iTeam], 2)
               .arg(iTeam, 1)
               .arg(iServizio, 1);
    SendToAll(sMessage);
    QString sText;
    sText = QString("%1").arg(iScore[iTeam], 2);
    scoreEdit[iTeam]->setText(sText);
    sText = QString("team%1/score").arg(iTeam+1, 1);
    pSettings->setValue(sText, iScore[iTeam]);
}


/*!
 * \brief VolleyController::onScoreDecrement
 * \param iTeam
 */
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
    sMessage = QString("<score%1>%2</score%3><servizio>%4</servizio>")
               .arg(iTeam, 1)
               .arg(iScore[iTeam], 2)
               .arg(iTeam, 1)
               .arg(iServizio, 1);
    SendToAll(sMessage);
    QString sText;
    sText = QString("%1").arg(iScore[iTeam], 2);
    scoreEdit[iTeam]->setText(sText);
    sText = QString("team%1/score").arg(iTeam+1, 1);
    pSettings->setValue(sText, iScore[iTeam]);
}


/*!
 * \brief VolleyController::onTeamTextChanged
 * \param sText
 * \param iTeam
 */
void
VolleyController::onTeamTextChanged(QString sText, int iTeam) {
    QString sMessage;
    sTeam[iTeam] = sText;
    if(sText=="")// C'è un problema con la stringa vuota...
        sMessage = QString("<team%1>-</team%1>").arg(iTeam, 1).arg(iTeam, 1);
    else
        sMessage = QString("<team%1>%2</team%3>").arg(iTeam, 1).arg(sTeam[iTeam].toLocal8Bit().data()).arg(iTeam, 1);
    SendToAll(sMessage);
    sText = QString("team%1/name").arg(iTeam+1, 1);
    pSettings->setValue(sText, sTeam[iTeam]);
}


/*!
 * \brief VolleyController::onButtonChangeFieldClicked
 */
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
    sText = QString("%1").arg(iSet[0], 1);
    setsEdit[0]->setText(sText);
    sText = QString("%1").arg(iSet[1], 1);
    setsEdit[1]->setText(sText);

    iVal = iScore[0];
    iScore[0] = iScore[1];
    iScore[1] = iVal;
    sText = QString("%1").arg(iScore[0], 2);
    scoreEdit[0]->setText(sText);
    sText = QString("%1").arg(iScore[1], 2);
    scoreEdit[1]->setText(sText);

    iVal = iTimeout[0];
    iTimeout[0] = iTimeout[1];
    iTimeout[1] = iVal;
    sText = QString("%1").arg(iTimeout[0]);
    timeoutEdit[0]->setText(sText);
    sText = QString("%1").arg(iTimeout[1]);
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
        if(iSet[iTeam] == pGeneralSetupDialog->getNumSetVY()) {
            setsIncrement[iTeam]->setEnabled(false);
        }

        timeoutIncrement[iTeam]->setEnabled(true);
        timeoutDecrement[iTeam]->setEnabled(true);
        timeoutEdit[iTeam]->setStyleSheet(styleSheet());
        if(iTimeout[iTeam] == pGeneralSetupDialog->getNumTimeoutVY()) {
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


/*!
 * \brief VolleyController::onButtonNewSetClicked
 */
void
VolleyController::onButtonNewSetClicked() {
    int iRes = QMessageBox::question(this, tr("Volley_Controller"),
                                     tr("Vuoi davvero iniziare un nuovo Set ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;

    // Exchange team's order in the field
    QString sText = sTeam[0];
    sTeam[0] = sTeam[1];
    sTeam[1] = sText;
    teamName[0]->setText(sTeam[0]);
    teamName[1]->setText(sTeam[1]);
    int iVal = iSet[0];
    iSet[0] = iSet[1];
    iSet[1] = iVal;
    sText = QString("%1").arg(iSet[0], 1);
    setsEdit[0]->setText(sText);
    sText = QString("%1").arg(iSet[1], 1);
    setsEdit[1]->setText(sText);
    for(int iTeam=0; iTeam<2; iTeam++) {
        iTimeout[iTeam] = 0;
        sText = QString("%1").arg(iTimeout[iTeam], 1);
        timeoutEdit[iTeam]->setText(sText);
        timeoutEdit[iTeam]->setStyleSheet(styleSheet());
        iScore[iTeam]   = 0;
        sText = QString("%1").arg(iScore[iTeam], 2);
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


/*!
 * \brief VolleyController::onButtonNewGameClicked
 */
void
VolleyController::onButtonNewGameClicked() {
    int iRes = QMessageBox::question(this, tr("Volley_Controller"),
                                     tr("Iniziare una Nuova Partita ?"),
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No);
    if(iRes != QMessageBox::Yes) return;
    sTeam[0]    = tr("Locali");
    sTeam[1]    = tr("Ospiti");
    QString sText;
    for(int iTeam=0; iTeam<2; iTeam++) {
        teamName[iTeam]->setText(sTeam[iTeam]);
        iTimeout[iTeam] = 0;
        sText = QString("%1").arg(iTimeout[iTeam], 1);
        timeoutEdit[iTeam]->setText(sText);
        timeoutEdit[iTeam]->setStyleSheet(styleSheet());
        iSet[iTeam]   = 0;
        sText = QString("%1").arg(iSet[iTeam], 1);
        setsEdit[iTeam]->setText(sText);
        iScore[iTeam]   = 0;
        sText = QString("%1").arg(iScore[iTeam], 2);
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

