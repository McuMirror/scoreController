#include "generalsetupdialog.h"
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QDir>


GeneralSetupDialog::GeneralSetupDialog(QWidget *parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;

    pDirectoryTab = new DirectoryTab();
    tabWidget->addTab(pDirectoryTab, tr("General"));

    tabWidget->addTab(pDirectoryTab, tr("Permissions"));
    tabWidget->addTab(pDirectoryTab, tr("Applications"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                     QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}


void
GeneralSetupDialog::setSlideDir(QString sDir) {
    pDirectoryTab->setSlideDir(sDir);
}


void
GeneralSetupDialog::setSpotDir(QString sDir){
    pDirectoryTab->setSpotDir(sDir);
}


QString
GeneralSetupDialog::getSlideDir() {
    return pDirectoryTab->getSlideDir();
}


QString
GeneralSetupDialog::getSpotDir(){
    return pDirectoryTab->getSpotDir();
}


///////////////////
// Directory Tab //
///////////////////
/// \brief DirectoryTab::DirectoryTab
/// \param parent
///
DirectoryTab::DirectoryTab(QWidget *parent)
    : QWidget(parent)
{
    slidesDirEdit.setText(QString(""));
    spotsDirEdit.setText(QString(""));
    slidesDirEdit.setStyleSheet("background:red;color:white;");
    spotsDirEdit.setStyleSheet("background:red;color:white;");

    buttonSelectSlidesDir.setText("...");
    buttonSelectSpotsDir.setText("...");

    slidesDirEdit.setReadOnly(true);
    spotsDirEdit.setReadOnly(true);

    QLabel *slidesPathLabel = new QLabel(tr("Slides folder:"));
    slidesPathLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *spotsPathLabel = new QLabel(tr("Spots folder:"));
    spotsPathLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(slidesPathLabel,        0, 0, 1, 1);
    mainLayout->addWidget(&slidesDirEdit,         0, 1, 1, 3);
    mainLayout->addWidget(&buttonSelectSlidesDir, 0, 3, 1, 1);

    mainLayout->addWidget(spotsPathLabel,        1, 0, 1, 1);
    mainLayout->addWidget(&spotsDirEdit,         1, 1, 1, 3);
    mainLayout->addWidget(&buttonSelectSpotsDir, 1, 3, 1, 1);

    setLayout(mainLayout);
}


void
DirectoryTab::setSlideDir(QString sDir) {
    slidesDirEdit.setText(sDir);
    QDir slideDir(sDir);
    if(slideDir.exists()) {
        slidesDirEdit.setStyleSheet(styleSheet());
    }
    else {
        slidesDirEdit.setStyleSheet("background:red;color:white;");
    }
}


void
DirectoryTab::setSpotDir(QString sDir){
    spotsDirEdit.setText(sDir);
    QDir spotDir(sDir);
    if(spotDir.exists()) {
        spotsDirEdit.setStyleSheet(styleSheet());
    }
    else {
        spotsDirEdit.setStyleSheet("background:red;color:white;");
    }
}


QString
DirectoryTab::getSlideDir() {
    return slidesDirEdit.text();
}


QString
DirectoryTab::getSpotDir(){
    return spotsDirEdit.text();
}
