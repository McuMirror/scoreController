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
#include <QSettings>

#include "fileserver.h"
#include "panelorientation.h"
#include "utility.h"


QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QUdpSocket)
QT_FORWARD_DECLARE_CLASS(ClientListDialog)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QThread)
QT_FORWARD_DECLARE_CLASS(NetServer)
QT_FORWARD_DECLARE_CLASS(FileServer)
QT_FORWARD_DECLARE_CLASS(QGroupBox)
QT_FORWARD_DECLARE_CLASS(QGridLayout)
QT_FORWARD_DECLARE_CLASS(QHBoxLayout)
QT_FORWARD_DECLARE_CLASS(GeneralSetupDialog)



class ScoreController : public QWidget
{
    Q_OBJECT

public:
    ScoreController(int myPanelType, QWidget *parent=Q_NULLPTR);
    ~ScoreController();

signals:
    void startSpotServer();
    void closeSpotServer();
    void startSlideServer();
    void closeSlideServer();

protected slots:
    void onButtonStartStopSpotLoopClicked();
    void onButtonStartStopSlideShowClicked();
    void onButtonStartStopLiveCameraClicked();

    void onButtonPanelControlClicked();
    void onButtonSetupClicked();
    void onButtonShutdownClicked();

    void onNewConnection(QWebSocket *pClient);
    void onClientDisconnected();
    void onProcessTextMessage(QString sMessage);
    void onProcessBinaryMessage(QByteArray message);
    void onProcessConnectionRequest();

    void onStartCamera(const QString& sClientIp);
    void onStopCamera();
    void onSetNewPanValue(const QString& sClientIp, int newPan);
    void onSetNewTiltValue(const QString& sClientIp, int newTilt);

    void onGetPanelOrientation(const QString& sClientIp);
    void onChangePanelOrientation(const QString& sClientIp, PanelOrientation orientation);

    void onGetIsPanelScoreOnly(const QString& sClientIp);
    void onSetScoreOnly(const QString& sClientIp, bool bScoreOnly);

    void onSlideServerDone(bool bError);
    void onSpotServerDone(bool bError);

protected:
    void            prepareDirectories();
    void            prepareServices();
    void            prepareSpotUpdateService();
    void            prepareSlideUpdateService();
    QGroupBox      *CreateClientListBox();
    QHBoxLayout    *CreateSpotButtons();
    int             WaitForNetworkReady();
    int             SendToAll(const QString& sMessage);
    int             SendToOne(QWebSocket* pSocket, const QString& sMessage);
    bool            prepareServer();
    void            sendAcceptConnection(QUdpSocket *pDiscoverySocket, const QHostAddress& hostAddress, quint16 port);
    void            RemoveClient(const QHostAddress& hAddress);
    bool            isConnectedToNetwork();
    bool            prepareLogFile();
    bool            prepareDiscovery();
    void            UpdateUI();
    void            closeEvent(QCloseEvent *event);
    virtual QString FormatStatusMsg();
    virtual void    SaveStatus();

protected:
    QSettings            *pSettings;
    GeneralSetupDialog   *pGeneralSetupDialog;

    struct connection {
      QWebSocket*     pClientSocket;
      QHostAddress    clientAddress;
      QString         sHostName = QString("NoName");
    };

    int                   panelType;
    NetServer            *pPanelServer{};

    QString               sLogDir;
    QString               logFileName;
    QFile*                logFile;

    ClientListDialog     *pClientListDialog;
    QList<connection>     connectionList;

    quint16               discoveryPort;
    quint16               serverPort;
    QVector<QUdpSocket*>  discoverySocketArray;

    QStringList           sIpAddresses;
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

    QSoundEffect*         pButtonClick;

    QPushButton*          startStopLoopSpotButton{};
    QPushButton*          startStopSlideShowButton{};
    QPushButton*          startStopLiveCameraButton{};
    QPushButton*          panelControlButton{};
    QPushButton*          generalSetupButton{};
    QPushButton*          shutdownButton{};
    enum status {
        showPanel,
        showSpots,
        showSlides,
        showCamera
    };
    status                myStatus;
};

#endif // SCORECONTROLLER_H
