#include "clientlistdialog.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>

ClientListDialog::ClientListDialog(QWidget* parent)
    : QDialog(parent)
    , pMyParent(parent)
    , panMin(-30)
    , panMax(30)
    , tiltMin(-30)
    , tiltMax(30)
{
    QHBoxLayout*  mainLayout = new QHBoxLayout();
    mainLayout->addWidget(createClientListBox());
    mainLayout->addWidget(createPanTiltBox());
    setLayout(mainLayout);
    connect(this, SIGNAL(finished(int)),
            this, SLOT(onCloseCamera()));
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
    clientListWidget = new QListWidget();
    QFont *font;
    font = new QFont("Arial", 24);
    clientListWidget->setFont(*font);
    clientListLayout->addWidget(clientListWidget, 0, 0, 6, 3);
    connect(clientListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onClientSelected(QListWidgetItem*)));
    clientListLayout->addWidget(closeButton, 6, 1, 1, 1);
    clientListBox->setLayout(clientListLayout);
    return clientListBox;
}


QGroupBox*
ClientListDialog::createPanTiltBox() {
  QGroupBox* panTiltBox = new QGroupBox();
  QGridLayout* panTiltLayout = new QGridLayout();

  int iDelay  = 300;
  int iRepeat = 100;

  leftButton = new QPushButton(tr(""), this);
  leftButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
  leftButton->setAutoRepeat(true);
  leftButton->setAutoRepeatDelay(iDelay);
  leftButton->setAutoRepeatInterval(iRepeat);
  connect(leftButton, SIGNAL(pressed()),
          this, SLOT(onLeftButtonPressed()));

  rightButton = new QPushButton(tr(""), this);
  rightButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
  rightButton->setAutoRepeat(true);
  rightButton->setAutoRepeatDelay(iDelay);
  rightButton->setAutoRepeatInterval(iRepeat);
  connect(rightButton, SIGNAL(pressed()),
          this, SLOT(onRightButtonPressed()));

  upButton = new QPushButton(tr(""), this);
  upButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
  upButton->setAutoRepeat(true);
  upButton->setAutoRepeatDelay(iDelay);
  upButton->setAutoRepeatInterval(iRepeat);
  connect(upButton, SIGNAL(pressed()),
          this, SLOT(onUpButtonPressed()));

  downButton = new QPushButton(tr(""), this);
  downButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
  downButton->setAutoRepeat(true);
  downButton->setAutoRepeatDelay(iDelay);
  downButton->setAutoRepeatInterval(iRepeat);
  connect(downButton, SIGNAL(pressed()),
          this, SLOT(onDownButtonPressed()));

  leftButton->setDisabled(true);
  rightButton->setDisabled(true);
  upButton->setDisabled(true);
  downButton->setDisabled(true);

  panTiltBox->setTitle("Camera Control");
  panTiltLayout->addWidget(upButton,    0, 1, 1, 1);
  panTiltLayout->addWidget(leftButton,  1, 0, 1, 1);
  panTiltLayout->addWidget(rightButton, 1, 2, 1, 1);
  panTiltLayout->addWidget(downButton,  2, 1, 1, 1);

  panTiltBox->setLayout(panTiltLayout);
  return panTiltBox;
}


void
ClientListDialog::clear() {
    clientListWidget->clear();
}


void
ClientListDialog::addItem(QString sAddress) {
    clientListWidget->addItem(sAddress);
}


void
ClientListDialog::onCloseCamera() {
    emit disableVideo();
}


void
ClientListDialog::onLeftButtonPressed() {
    iPan--;
    rightButton->setDisabled(false);
    if(iPan >= panMin)
        emit newPanValue(sSelectedClient, iPan);
    else {
        iPan = panMin;
        leftButton->setDisabled(true);
    }
}


void
ClientListDialog::onRightButtonPressed() {
    iPan++;
    leftButton->setDisabled(false);
    if(iPan <= panMax)
        emit newPanValue(sSelectedClient, iPan);
    else {
        iPan = panMax;
        rightButton->setDisabled(true);
    }
}


void
ClientListDialog::onUpButtonPressed() {
    iTilt++;
    downButton->setDisabled(false);
    if(iTilt <= tiltMax)
        emit newTiltValue(sSelectedClient, iTilt);
    else {
        iTilt = tiltMax;
        upButton->setDisabled(true);
    }
}


void
ClientListDialog::onDownButtonPressed() {
    iTilt--;
    upButton->setDisabled(false);
    if(iTilt >= tiltMin)
        emit newTiltValue(sSelectedClient, iTilt);
    else {
        iTilt = tiltMin;
        downButton->setDisabled(true);
    }
}


void
ClientListDialog::onClientSelected(QListWidgetItem* selectedClient) {
  emit disableVideo();
  leftButton->setDisabled(true);
  rightButton->setDisabled(true);
  upButton->setDisabled(true);
  downButton->setDisabled(true);
  sSelectedClient = selectedClient->text();
  emit enableVideo(sSelectedClient);
}


void
ClientListDialog::onRemotePanTiltReceived(int newPan, int newTilt) {
    iPan  = newPan;
    iTilt = newTilt;
    leftButton->setDisabled(iPan <= panMin);
    rightButton->setDisabled(iPan >= panMax);
    upButton->setDisabled(iTilt >= tiltMax);
    downButton->setDisabled(iTilt <= tiltMin);
}


int
ClientListDialog::exec() {
  clientListWidget->clearSelection();
  return QDialog::exec();
}