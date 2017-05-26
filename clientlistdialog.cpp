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

ClientListDialog::ClientListDialog(QWidget* parent)
    : QDialog(parent)
    , pMyParent(parent)
{
    QGridLayout*  mainLayout = new QGridLayout();
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
}


ClientListDialog::~ClientListDialog() {
}


QGroupBox*
ClientListDialog::createClientListBox() {
    QGroupBox* clientListBox = new QGroupBox();
    QGridLayout* clientListLayout = new QGridLayout();
    closeButton = new QPushButton("Chiudi");
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


void
ClientListDialog::clear() {
    clientListWidget.clear();
}


void
ClientListDialog::addItem(QString sAddress) {
    clientListWidget.addItem(sAddress);
}


void
ClientListDialog::onStartCamera() {
    emit enableVideo(sSelectedClient);
}


void
ClientListDialog::onCloseCamera() {
    emit disableVideo();
}


void
ClientListDialog::onClientSelected(QListWidgetItem* selectedClient) {
    emit disableVideo();
    pConfigurator->show();
    sSelectedClient = selectedClient->text();
    emit getOrientation(sSelectedClient);
    emit getScoreOnly(sSelectedClient);
}


void
ClientListDialog::onSetNewPan(int newPan) {
    emit newPanValue(sSelectedClient, newPan);
}


void
ClientListDialog::onSetNewTilt(int newTilt) {
    emit newTiltValue(sSelectedClient, newTilt);
}


void
ClientListDialog::remotePanTiltReceived(int newPan, int newTilt) {
    pConfigurator->SetCurrentPanTilt(newPan, newTilt);
}


void
ClientListDialog::remoteOrientationReceived(PanelOrientation currentOrientation) {
    int index = static_cast<int>(currentOrientation);
    pConfigurator->SetCurrrentOrientaton(index);
}


void
ClientListDialog::onChangePanelOrientation(PanelOrientation newOrientation) {
    emit changeOrientation(sSelectedClient, newOrientation);
}


void
ClientListDialog::remoteScoreOnlyValueReceived(bool bScoreOnly) {
    pConfigurator->SetIsScoreOnly(bScoreOnly);
}


void
ClientListDialog::onChangeScoreOnly(bool bScoreOnly) {
    emit changeScoreOnly(sSelectedClient, bScoreOnly);
}


int
ClientListDialog::exec() {
  clientListWidget.clearSelection();
  return QDialog::exec();
}
