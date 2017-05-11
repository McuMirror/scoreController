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

#ifndef CLIENTLISTDIALOG_H
#define CLIENTLISTDIALOG_H

#include "panelorientation.h"

#include <QObject>
#include <QDialog>

QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QComboBox)

class ClientListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientListDialog(QWidget *parent);
    ~ClientListDialog();
    int exec();

public slots:
    void onRemotePanTiltReceived(int newPan, int newTilt);
    void onCloseCamera();
    void onOrientationReceived(PanelOrientation currentOrientation);

signals:
    void enableVideo(QString sIpAdress);
    void disableVideo();
    void setPanTilt(int newPan, int newTilt);
    void newPanValue(QString sClientIp, int newPan);
    void newTiltValue(QString sClientIp, int newTilt);
    void getOrientation(QString sIpAdress);
    void changeOrientation(QString sIpAdress, PanelOrientation newOrientation);

private slots:
    void onClientSelected(QListWidgetItem* selectedClient);
    void onLeftButtonPressed();
    void onRightButtonPressed();
    void onUpButtonPressed();
    void onDownButtonPressed();
    void onChangePanelOrientation(int);

private:
    QGroupBox* createClientListBox();
    QGroupBox* createPanTiltBox();
    QGroupBox* createOrientationBox();

private:
    QWidget*      pMyParent;
    QListWidget*  clientListWidget;
    QPushButton*  closeButton;
    QPushButton*  upButton;
    QPushButton*  downButton;
    QPushButton*  leftButton;
    QPushButton*  rightButton;
    QString       sSelectedClient;
    int           iPan;
    int           iTilt;
    int           panMin;
    int           panMax;
    int           tiltMin;
    int           tiltMax;
    QComboBox*    pPanelOrientation;

public:
    void clear();
    void addItem(QString sAddress);
};

#endif // CLIENTLISTDIALOG_H
