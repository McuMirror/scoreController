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

#ifndef SCORECONTROLLER_H
#define SCORECONTROLLER_H

#include <QWidget>
#include <QHostAddress>
#include <QFileInfoList>
#include <QSoundEffect>

#include "panelorientation.h"


QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QUdpSocket)
QT_FORWARD_DECLARE_CLASS(ClientListDialog)
QT_FORWARD_DECLARE_CLASS(QTimer)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QThread)
QT_FORWARD_DECLARE_CLASS(NetServer)
QT_FORWARD_DECLARE_CLASS(FileServer)
QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(QGridLayout)


#define VOLLEY_PANEL 0
#define FIRST_PANEL  VOLLEY_PANEL
#define BASKET_PANEL 1
#define LAST_PANEL   BASKET_PANEL


class ScoreController : public QWidget
{
    Q_OBJECT

public:
    ScoreController(int _panelType, QWidget *parent = 0);
    ~ScoreController();
    void closeEvent(QCloseEvent *event);

signals:
    void startSpotServer();
    void closeSpotServer();
    void startSlideServer();
    void closeSlideServer();

protected slots:
    void onButtonStartStopSpotClicked();
    void onButtonStartStopSpotLoopClicked();
    void onButtonStartStopSlideShowClicked();
    void onButtonStartStopLiveCameraClicked();

    void onButtonCameraControlClicked();
    void onButtonSetupClicked();
    void onButtonShutdownClicked();

    void onNewConnection(QWebSocket *pClient);
    void onClientDisconnected();
    void onProcessTextMessage(QString sMessage);
    void onProcessBinaryMessage(QByteArray message);
    void onProcessConnectionRequest();

    void onStartCamera(QString sClientIp);
    void onStopCamera();
    void onSetNewPanValue(QString sClientIp, int newPan);
    void onSetNewTiltValue(QString sClientIp, int newTilt);

    void onGetPanelOrientation(QString sClientIp);
    void onChangePanelOrientation(QString sClientIp, PanelOrientation orientation);

    void onSlideServerDone(bool bError);
    void onSpotServerDone(bool bError);

protected:
    QGroupBox      *CreateClientListBox();
    QGroupBox      *CreateSpotButtonBox();
    void            WaitForNetworkReady();
    int             SendToAll(QString sMessage);
    int             SendToOne(QWebSocket* pSocket, QString sMessage);
    int             prepareServer();
    int             sendAcceptConnection(QUdpSocket *pDiscoverySocket, QString sMessage, QHostAddress hostAddress, quint16 port);
    void            RemoveClient(QHostAddress hAddress);
    bool            isConnectedToNetwork();
    bool            PrepareLogFile();
    void            prepareDiscovery();
    virtual QString FormatStatusMsg();
    void            UpdateUI();

protected:
    struct connection{
      QWebSocket*     pClientSocket;
      QHostAddress    clientAddress;
    };
    QString               sHostName;

    int                   panelType;
    NetServer            *pPanelServer;

    QString               logFileName;
    QFile*                logFile;

    ClientListDialog     *pClientListDialog;
    QList<connection>     connectionList;

    quint16               discoveryPort;
    quint16               serverPort;
    QVector<QUdpSocket*>  discoverySocketArray;

    QString               sIpAddresses;
    QHostAddress          discoveryAddress;

    QThread              *pSlideServerThread;
    FileServer           *pSlideUpdaterServer;
    quint16               slideUpdaterPort;
    QString               sSlideDir;
    QFileInfoList         slideList;
    int                   iCurrentSlide;

    QThread              *pSpotServerThread;
    FileServer           *pSpotUpdaterServer;
    quint16               spotUpdaterPort;
    QString               sSpotDir;
    QFileInfoList         spotList;
    int                   iCurrentSpot;

    QSoundEffect          buttonClick;
    QTimer               *pExitTimer;

    QPushButton*          startStopLoopSpotButton;
    QPushButton*          startStopSpotButton;
    QPushButton*          startStopSlideShowButton;
    QPushButton*          startStopLiveCameraButton;
    QPushButton*          panelControlButton;
    QPushButton*          generalSetupButton;
    QPushButton*          shutdownButton;

};

#endif // SCORECONTROLLER_H
