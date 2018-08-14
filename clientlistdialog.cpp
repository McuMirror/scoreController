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

#include "clientlistdialog.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QComboBox>
#include <QMessageBox>


/*!
 * \brief ClientListDialog::ClientListDialog
 * A Dialog to handle the connected clients of this Controller
 * \param parent
 */
ClientListDialog::ClientListDialog(QWidget* parent)
    : QDialog(parent)
    , pMyParent(parent)
{
    auto*  mainLayout = new QGridLayout();
    mainLayout->addWidget(createClientListBox(),  0,  0, 10, 10);
    setLayout(mainLayout);
    connect(this, SIGNAL(finished(int)),
            this, SLOT(onCloseCamera()));

    pConfigurator = new PanelConfigurator(this);
    connect(pConfigurator, SIGNAL(newPanValue(int)),
            this, SLOT(onSetNewPan(int)));
    connect(pConfigurator, SIGNAL(startCamera()),
            this, SLOT(onStartCamera()));
    connect(pConfigurator, SIGNAL(stopCamera()),
            this, SLOT(onCloseCamera()));
    connect(pConfigurator, SIGNAL(newTiltValue(int)),
            this, SLOT(onSetNewTilt(int)));
    connect(pConfigurator, SIGNAL(changeOrientation(PanelOrientation)),
            this, SLOT(onChangePanelOrientation(PanelOrientation)));
    connect(pConfigurator, SIGNAL(scoreOnly(bool)),
            this, SLOT(onChangeScoreOnly(bool)));
    connect(pConfigurator, SIGNAL(closingDialog()),
            this, SLOT(onConfiguratorClosing()));
}



ClientListDialog::~ClientListDialog() = default;


/*!
 * \brief ClientListDialog::createClientListBox
 * \return
 */
QGroupBox*
ClientListDialog::createClientListBox() {
    auto* clientListBox = new QGroupBox();
    auto* clientListLayout = new QGridLayout();
    closeButton = new QPushButton(tr("Chiudi"));
    connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

    clientListBox->setTitle(tr("Client Connessi"));
    clientListWidget.setFont(QFont("Arial", 24));
    clientListLayout->addWidget(&clientListWidget, 0, 0, 6, 3);
    connect(&clientListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onClientSelected(QListWidgetItem*)));
    clientListLayout->addWidget(closeButton, 6, 1, 1, 1);
    clientListBox->setLayout(clientListLayout);
    return clientListBox;
}


/*!
 * \brief ClientListDialog::clear
 */
void
ClientListDialog::clear() {
    clientListWidget.clear();
}


/*!
 * \brief ClientListDialog::addItem
 * \param sAddress
 */
void
ClientListDialog::addItem(const QString& sAddress) {
    clientListWidget.addItem(sAddress);
}


/*!
 * \brief ClientListDialog::onStartCamera
 */
void
ClientListDialog::onStartCamera() {
    emit enableVideo(sSelectedClient);
}


/*!
 * \brief ClientListDialog::onCloseCamera
 */
void
ClientListDialog::onCloseCamera() {
    emit disableVideo();
}


/*!
 * \brief ClientListDialog::onConfiguratorClosing
 */
void
ClientListDialog::onConfiguratorClosing() {
    emit disableVideo();
    show();
}


/*!
 * \brief ClientListDialog::onClientSelected
 * \param selectedClient
 */
void
ClientListDialog::onClientSelected(QListWidgetItem* selectedClient) {
    emit disableVideo();
    pConfigurator->show();
    sSelectedClient = selectedClient->text();
    emit getOrientation(sSelectedClient);
    emit getScoreOnly(sSelectedClient);
}


/*!
 * \brief ClientListDialog::onSetNewPan
 * \param newPan
 */
void
ClientListDialog::onSetNewPan(int newPan) {
    emit newPanValue(sSelectedClient, newPan);
}


/*!
 * \brief ClientListDialog::onSetNewTilt
 * \param newTilt
 */
void
ClientListDialog::onSetNewTilt(int newTilt) {
    emit newTiltValue(sSelectedClient, newTilt);
}


/*!
 * \brief ClientListDialog::remotePanTiltReceived
 * \param newPan
 * \param newTilt
 */
void
ClientListDialog::remotePanTiltReceived(int newPan, int newTilt) {
    pConfigurator->SetCurrentPanTilt(newPan, newTilt);
}


/*!
 * \brief ClientListDialog::remoteOrientationReceived
 * \param currentOrientation
 */
void
ClientListDialog::remoteOrientationReceived(PanelOrientation currentOrientation) {
    auto index = static_cast<int>(currentOrientation);
    pConfigurator->SetCurrrentOrientaton(index);
}


/*!
 * \brief ClientListDialog::onChangePanelOrientation
 * \param newOrientation
 */
void
ClientListDialog::onChangePanelOrientation(PanelOrientation newOrientation) {
    emit changeOrientation(sSelectedClient, newOrientation);
}


/*!
 * \brief ClientListDialog::remoteScoreOnlyValueReceived
 * \param bScoreOnly
 */
void
ClientListDialog::remoteScoreOnlyValueReceived(bool bScoreOnly) {
    pConfigurator->SetIsScoreOnly(bScoreOnly);
}


/*!
 * \brief ClientListDialog::onChangeScoreOnly
 * \param bScoreOnly
 */
void
ClientListDialog::onChangeScoreOnly(bool bScoreOnly) {
    emit changeScoreOnly(sSelectedClient, bScoreOnly);
}


/*!
 * \brief ClientListDialog::exec
 * \return
 */
int
ClientListDialog::exec() {
  clientListWidget.clearSelection();
  return QDialog::exec();
}
