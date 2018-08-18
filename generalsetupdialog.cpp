#include "generalsetupdialog.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDir>
#include "utility.h"


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

