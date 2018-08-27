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
#include "generalsetupdialog.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDir>
#include <QApplication>
#include <QLabel>
#include <QFrame>

#include "utility.h"
#include "build_number.h"


/*!
 * \brief GeneralSetupDialog::GeneralSetupDialog
 * \param parent
 */
GeneralSetupDialog::GeneralSetupDialog(int PanelType, QWidget *parent)
    : QDialog(parent)
{
    pVolleyTab   = Q_NULLPTR;
    pBasketTab   = Q_NULLPTR;
    pHandballTab = Q_NULLPTR;

    QString statusMessage = QString("Score Controller ver. %1 - build: %2")
                                    .arg(QApplication::applicationVersion())
                                    .arg(BUILD);
    pStatusBar = new QLabel(statusMessage);
    pStatusBar->setFrameStyle(QFrame::Panel|QFrame::Sunken);

    connect(this, SIGNAL(finished(int)),
            this, SLOT(onFinished(int)));

    tabWidget = new QTabWidget;

    pDirectoryTab = new DirectoryTab();
    tabWidget->addTab(pDirectoryTab, tr("Directories"));

    if(PanelType == VOLLEY_PANEL) {
        pVolleyTab = new VolleyTab();
        pVolleyTab->GetSettings();
        tabWidget->addTab(pVolleyTab, tr("Volley"));
    }
    else if(PanelType == BASKET_PANEL) {
        pBasketTab = new BasketTab();
        pBasketTab->GetSettings();
        tabWidget->addTab(pBasketTab, tr("Basket"));
    }
    else if(PanelType == HANDBALL_PANEL) {
        pHandballTab = new HandBallTab();
        pHandballTab->GetSettings();
        tabWidget->addTab(pHandballTab, tr("Handball"));
    }
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                     QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    mainLayout->addWidget(pStatusBar);
    setLayout(mainLayout);
}


/*!
 * \brief GeneralSetupDialog::setSlideDir
 * \param sDir
 */
void
GeneralSetupDialog::setSlideDir(const QString& sDir) {
    pDirectoryTab->setSlideDir(sDir);
}


/*!
 * \brief GeneralSetupDialog::setSpotDir
 * \param sDir
 */
void
GeneralSetupDialog::setSpotDir(const QString& sDir){
    pDirectoryTab->setSpotDir(sDir);
}


/*!
 * \brief GeneralSetupDialog::getSlideDir
 * \return
 */
QString
GeneralSetupDialog::getSlideDir() {
    return pDirectoryTab->getSlideDir();
}


/*!
 * \brief GeneralSetupDialog::getSpotDir
 * \return
 */
QString
GeneralSetupDialog::getSpotDir(){
    return pDirectoryTab->getSpotDir();
}



int
GeneralSetupDialog::getNumTimeoutVY() {
    return pVolleyTab->getNumTimeout();
}


int
GeneralSetupDialog::getNumSetVY(){
    return pVolleyTab->getNumSet();
}


int
GeneralSetupDialog::getTimeoutDurationVY() {
    return pVolleyTab->getTimeoutDuration();
}


int
GeneralSetupDialog::getNumTimeoutHB() {
    return pHandballTab->getNumTimeout();
}


int
GeneralSetupDialog::getNumPeriodHB() {
    return pHandballTab->getNumPeriod();
}


int
GeneralSetupDialog::getRegularTimeHB() {
    return pHandballTab->getRegularTime();
}


void
GeneralSetupDialog::onFinished(int iResult) {
    if(iResult==QDialog::Accepted) {
        if(pVolleyTab) pVolleyTab->StoreSettings();
        if(pBasketTab) pBasketTab->StoreSettings();
        if(pHandballTab) pHandballTab->StoreSettings();
    }
    else {
        if(pVolleyTab) pVolleyTab->GetSettings();
        if(pBasketTab) pBasketTab->GetSettings();
        if(pHandballTab) pHandballTab->GetSettings();
    }
}



int
GeneralSetupDialog::getNumTimeout1BB(){
    return pBasketTab->getNumTimeout1();
}


int
GeneralSetupDialog::getNumTimeout2BB(){
    return pBasketTab->getNumTimeout2();
}


int
GeneralSetupDialog::getNumTimeout3BB(){
    return pBasketTab->getNumTimeout3();
}


int
GeneralSetupDialog::getGamePeriodsBB(){
    return pBasketTab->getGamePeriods();
}


int
GeneralSetupDialog::getBonusTargetBB(){
    return pBasketTab->getBonusTarget();
}


int
GeneralSetupDialog::getRegularTimeBB(){
    return pBasketTab->getRegularTime();
}


int
GeneralSetupDialog::getOverTimeBB(){
    return pBasketTab->getOverTime();
}

