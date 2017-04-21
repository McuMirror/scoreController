#ifndef SCORECONTROLLER_H
#define SCORECONTROLLER_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QUdpSocket)
QT_FORWARD_DECLARE_CLASS(QHostAddress)
QT_FORWARD_DECLARE_CLASS(ClientListDialog)


class ScoreController : public QWidget
{
    Q_OBJECT

public:
    ScoreController(QWidget *parent = 0);
    ~ScoreController();

signals:
    void startFileServer();

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
    void onPeriodicUpdate();

    void onStartCamera(QString sClientIp);
    void onStopCamera();
    void onSetNewPanValue(QString sClientIp, int newPan);
    void onSetNewTiltValue(QString sClientIp, int newTilt);

    void onFileServerDone(bool bError);

protected:
    void WaitForNetworkReady();
    int  SendToAll(QString sMessage);
    int  SendToOne(QWebSocket* pSocket, QString sMessage);
    int  prepareServer();
    int  sendAcceptConnection(QUdpSocket *pDiscoverySocket, QString sMessage, QHostAddress hostAddress, quint16 port);
    void RemoveClient(QHostAddress hAddress);
    bool isConnectedToNetwork();
    bool PrepareLogFile();
    void logMessage(QString sFunctionName, QString sMessage);
    void prepareDiscovery();

protected:
    ClientListDialog     *pClientListDialog;
};

#endif // SCORECONTROLLER_H
