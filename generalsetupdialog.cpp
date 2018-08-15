#include "generalsetupdialog.h"
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
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
    tabWidget->addTab(pDirectoryTab, tr("General"));

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
    connect(&buttonSelectSlidesDir, SIGNAL(clicked()),
            this, SLOT(onSelectSlideDir()));
    connect(&buttonSelectSpotsDir, SIGNAL(clicked()),
            this, SLOT(onSelectSpotDir()));

    slidesDirEdit.setReadOnly(true);
    spotsDirEdit.setReadOnly(true);

    QLabel *slidesPathLabel = new QLabel(tr("Slides folder:"));
    QLabel *spotsPathLabel = new QLabel(tr("Spots folder:"));

    auto *mainLayout = new QGridLayout;
    mainLayout->addWidget(slidesPathLabel,        0, 0, 1, 1);
    mainLayout->addWidget(&slidesDirEdit,         0, 1, 1, 3);
    mainLayout->addWidget(&buttonSelectSlidesDir, 0, 4, 1, 1);

    mainLayout->addWidget(spotsPathLabel,        1, 0, 1, 1);
    mainLayout->addWidget(&spotsDirEdit,         1, 1, 1, 3);
    mainLayout->addWidget(&buttonSelectSpotsDir, 1, 4, 1, 1);

    setLayout(mainLayout);
}


void
DirectoryTab::setSlideDir(const QString& sDir) {
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
DirectoryTab::setSpotDir(const QString& sDir){
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


void
DirectoryTab::onSelectSlideDir() {
    QString sSlideDir = slidesDirEdit.text();
    QFileDialog* pGetDirDlg;
    QDir slideDir = QDir(sSlideDir);
    if(slideDir.exists()) {
        pGetDirDlg = new QFileDialog(this, tr("Slide Dir"),
                                     sSlideDir);
    }
    else {
        pGetDirDlg = new QFileDialog(this, tr("Slide Dir"),
                                     QStandardPaths::displayName(QStandardPaths::GenericDataLocation));
    }
    pGetDirDlg->setOptions(QFileDialog::ShowDirsOnly);
    pGetDirDlg->setFileMode(QFileDialog::Directory);
    pGetDirDlg->setViewMode(QFileDialog::List);
    pGetDirDlg->setWindowFlags(Qt::Window);
    if(pGetDirDlg->exec() == QDialog::Accepted)
        sSlideDir = pGetDirDlg->directory().absolutePath();
    pGetDirDlg->deleteLater();
    if(!sSlideDir.endsWith(QString("/"))) sSlideDir+= QString("/");
    slidesDirEdit.setText(sSlideDir);
    slideDir.setPath(sSlideDir);
    if(slideDir.exists()) {
        slidesDirEdit.setStyleSheet(styleSheet());
    }
    else {
        slidesDirEdit.setStyleSheet("background:red;color:white;");
    }
}


void
DirectoryTab::onSelectSpotDir() {
    QString sSpotDir = spotsDirEdit.text();
    QFileDialog* pGetDirDlg;
    QDir spotDir = QDir(sSpotDir);
    if(spotDir.exists()) {
        pGetDirDlg = new QFileDialog(this, tr("Spot Dir"),
                                     sSpotDir);
    }
    else {
        pGetDirDlg = new QFileDialog(this, tr("Spot Dir"),
                                     QStandardPaths::displayName(QStandardPaths::GenericDataLocation));
    }
    pGetDirDlg->setOptions(QFileDialog::ShowDirsOnly);
    pGetDirDlg->setFileMode(QFileDialog::Directory);
    pGetDirDlg->setViewMode(QFileDialog::List);
    pGetDirDlg->setWindowFlags(Qt::Window);
    if(pGetDirDlg->exec() == QDialog::Accepted)
        sSpotDir = pGetDirDlg->directory().absolutePath();
    pGetDirDlg->deleteLater();
    if(!sSpotDir.endsWith(QString("/"))) sSpotDir+= QString("/");
    spotsDirEdit.setText(sSpotDir);
    spotDir.setPath(sSpotDir);
    if(spotDir.exists()) {
        spotsDirEdit.setStyleSheet(styleSheet());
    }
    else {
        spotsDirEdit.setStyleSheet("background:red;color:white;");
    }
}
