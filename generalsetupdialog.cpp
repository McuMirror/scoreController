#include "generalsetupdialog.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDir>
#include <utility>


/*!
 * \brief GeneralSetupDialog::GeneralSetupDialog
 * \param parent
 */
GeneralSetupDialog::GeneralSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    connect(this, SIGNAL(finished(int)),
            this, SLOT(onFinished(int)));

    tabWidget = new QTabWidget;

    pDirectoryTab = new DirectoryTab();
    tabWidget->addTab(pDirectoryTab, tr("Directories"));

    pVolleyTab = new VolleyTab();
    pVolleyTab->GetSettings();
    tabWidget->addTab(pVolleyTab, tr("Volley"));

    pBasketTab = new BasketTab();
//    pBasketTab->GetSettings();
    tabWidget->addTab(pBasketTab, tr("Basket"));

    pHandballTab = new HandBallTab();
    pHandballTab->GetSettings();
    tabWidget->addTab(pHandballTab, tr("Handball"));

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
        pVolleyTab->StoreSettings();
//      pBasketTab->StoreSettings();
      pHandballTab->StoreSettings();
    }
    else {
        pVolleyTab->GetSettings();
//      pBasketTab->GetSettings();
      pHandballTab->GetSettings();
    }
}

