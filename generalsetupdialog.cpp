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
    tabWidget = new QTabWidget;

    pDirectoryTab = new DirectoryTab();
    tabWidget->addTab(pDirectoryTab, tr("Directories"));

    pVolleyTab = new VolleyTab();
    tabWidget->addTab(pVolleyTab, tr("Volley"));

    pBasketTab = new BasketTab();
    tabWidget->addTab(pBasketTab, tr("Basket"));

    pHandballTab = new HandBallTab();
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


