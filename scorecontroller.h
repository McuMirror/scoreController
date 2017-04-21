#ifndef SCORECONTROLLER_H
#define SCORECONTROLLER_H

#include <QWidget>
#include <QHostAddress>
#include <QFileInfoList>
#include <QSoundEffect>


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

class ScoreController : public QWidget
{
    Q_OBJECT

public:
    ScoreController(QWidget *parent = 0);
    ~ScoreController();
    void closeEvent(QCloseEvent *event);

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
    virtual QGridLayout* createGamePanel();
    QGroupBox* createGameButtonBox();
    QGroupBox* createSpotButtonBox();
    QGroupBox* createClientListBox();
    void WaitForNetworkReady();
    int  SendToAll(QString sMessage);
    int  SendToOne(QWebSocket* pSocket, QString sMessage);
    int  prepareServer();
    int  sendAcceptConnection(QUdpSocket *pDiscoverySocket, QString sMessage, QHostAddress hostAddress, quint16 port);
    void RemoveClient(QHostAddress hAddress);
    bool isConnectedToNetwork();
    bool PrepareLogFile();
    void prepareDiscovery();
    virtual QString FormatStatusMsg();

protected:
    struct connection{
      QWebSocket*     pClientSocket;
      QHostAddress    clientAddress;
    };
    QString               sHostName;

    NetServer            *pPanelServer;
    QThread              *pFileServerThread;
    FileServer           *pFileUpdaterServer;

    QString               logFileName;
    QFile*                logFile;

    ClientListDialog     *pClientListDialog;
    QList<connection>     connectionList;

    quint16               discoveryPort;
    quint16               serverPort;
    quint16               updaterPort;
    QVector<QUdpSocket*>  discoverySocketArray;

    QString               sIpAddresses;
    QHostAddress          discoveryAddress;

    QString               sSlideDir;
    QStringList           slideList;
    int                   iCurrentSlide;
    QString               sSpotDir;
    QFileInfoList         spotList;
    int                   iCurrentSpot;
//    int                   updatePeriod;
    QSoundEffect          buttonClick;
    QTimer               *pExitTimer;

    QPushButton*          newSetButton;
    QPushButton*          newGameButton;
    QPushButton*          changeFieldButton;

    QPushButton*          startStopLoopSpotButton;
    QPushButton*          startStopSpotButton;
    QPushButton*          startStopSlideShowButton;
    QPushButton*          startStopLiveCameraButton;
    QPushButton*          cameraControlButton;
    QPushButton*          generalSetupButton;
    QPushButton*          shutdownButton;

};

#endif // SCORECONTROLLER_H
