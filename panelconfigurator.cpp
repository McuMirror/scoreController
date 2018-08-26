#include "panelconfigurator.h"
#include "ui_panelconfigurator.h"
#include "utility.h"

#include <QDialogButtonBox>
#include <QVBoxLayout>


/*!
 * \brief PanelConfigurator::PanelConfigurator
 * A Dialog to configure some Panel behaviours
 * \param parent
 */
PanelConfigurator::PanelConfigurator(QWidget *parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;

    pPanelTab = new PanelTab(this);
    connect(pPanelTab, SIGNAL(changeDirection(PanelDirection)),
            this, SLOT(onChangeDirection(PanelDirection)));
    connect(pPanelTab, SIGNAL(changeScoreOnly(bool)),
            this, SLOT(onChangeScoreOnly(bool)));

    pCameraTab = new CameraTab(this);
    connect(pCameraTab, SIGNAL(newTiltValue(int)),
            this, SLOT(onChangeTiltValue(int)));
    connect(pCameraTab, SIGNAL(newPanValue(int)),
            this, SLOT(onChangePanValue(int)));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    tabWidget->addTab(pPanelTab,  tr("Panel Control"));
    iCameraTab = tabWidget->addTab(pCameraTab, tr("Camera Control"));
    connect(tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(onChangedTab(int)));

    auto *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    connect(this, SIGNAL(finished(int)),
            this, SLOT(onFinished(int)));
}


void
PanelConfigurator::setClient(QString sClient) {
    sCurrentClient = sClient;
}


void
PanelConfigurator::SetCurrentPanTilt(int newPan, int newTilt) {
    pCameraTab->SetCurrentPanTilt(newPan, newTilt);
}


void
PanelConfigurator::SetCurrrentOrientaton(PanelDirection newDirection) {
    pPanelTab->setDirection(newDirection);
}


void
PanelConfigurator::SetIsScoreOnly(bool bScoreOnly) {
    pPanelTab->setScoreOnly(bScoreOnly);
}


void
PanelConfigurator::onChangedTab(int iTabIndex) {
    if(iTabIndex == iCameraTab)
        emit startCamera();
    else
        emit stopCamera();
}


void
PanelConfigurator::onChangeDirection(PanelDirection newDirection) {
    emit changeDirection(newDirection);
}


void
PanelConfigurator::onChangeScoreOnly(bool bScoreOnly) {
    #ifdef LOG_VERBOSE
        logMessage(Q_NULLPTR,
                   Q_FUNC_INFO,
                   QString("ScoreOnly: %2")
                   .arg(bScoreOnly));
    #endif
    emit changeScoreOnly(bScoreOnly);
}


void
PanelConfigurator::onChangeTiltValue(int newValue) {
    emit newTiltValue(newValue);

}


void
PanelConfigurator::onChangePanValue(int newValue) {
    emit newPanValue(newValue);
}


void
PanelConfigurator::onFinished(int iResult) {
    if(iResult==QDialog::Accepted) {
    }
    else {
    }
}

