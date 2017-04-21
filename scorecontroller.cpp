#include "scorecontroller.h"


#define DISCOVERY_PORT     45453
#define SERVER_SOCKET_PORT 45454
#define FILE_UPDATER_PORT  45455


#define UPDATE_PERIOD  2000

#define VOLLEY_PANEL 0
#define FIRST_PANEL  VOLLEY_PANEL
#define BASKET_PANEL 1
#define LAST_PANEL   BASKET_PANEL

//#define QT_DEBUG
#define LOG_MESG


ScoreController::ScoreController(QWidget *parent)
    : QWidget(parent)
    , pClientListDialog(new ClientListDialog(this))
    , connectionList(QList<connection>())
    , discoveryPort(DISCOVERY_PORT)
    , serverPort(SERVER_SOCKET_PORT)
    , updaterPort(FILE_UPDATER_PORT)
    , discoveryAddress(QHostAddress("224.0.0.1"))
{
    QString sFunctionName = QString(" Volley_Controller::Volley_Controller ");
    Q_UNUSED(sFunctionName)

    buttonClick.setSource(QUrl::fromLocalFile(":/key.wav"));
    sIpAddresses = QString();
    sNoData      = QString(tr("NoData"));

    QString sBaseDir;
#ifdef Q_OS_ANDROID
    sBaseDir = QString("/storage/extSdCard/");
#else
    sBaseDir = QDir::homePath();
    if(!sBaseDir.endsWith(QString("/"))) sBaseDir+= QString("/");
#endif
    sSlideDir   = QString("%1slides").arg(sBaseDir);
    sSpotDir    = QString("%1spots").arg(sBaseDir);
    logFileName = QString("%1volley_controller.txt").arg(sBaseDir);

    logFile       = Q_NULLPTR;
    slideList     = QStringList();
    iCurrentSlide = 0;
    iCurrentSpot  = 0;
    updatePeriod  = UPDATE_PERIOD;

    sDebugInformation.setString(&sDebugMessage);
    sDebugMessage = QString();

    PrepareLogFile();

    pExitTimer = new QTimer(this);
    connect(pExitTimer, SIGNAL(timeout()),
            this, SLOT(close()));

    waitForNetworkReady();

    sHostName = QHostInfo::localHostName();

    prepareDiscovery();

    if(prepareServer() < 0) {
        return;
    }

    // Creating a File Update Service
    pFileUpdaterServer = new FileServer(logFile, Q_NULLPTR);
    connect(pFileUpdaterServer, SIGNAL(fileServerDone(bool)),
            this, SLOT(onFileServerDone(bool)));
    pFileUpdaterServer->setServerPort(updaterPort);
    pFileUpdaterServer->setDirs(sSlideDir, sSpotDir);
    pFileServerThread = new QThread();
    pFileUpdaterServer->moveToThread(pFileServerThread);
    connect(this, SIGNAL(startFileServer()),
            pFileUpdaterServer, SLOT(startServer()));
    pFileServerThread->start(QThread::LowestPriority);
    emit startFileServer();

//    pPeriodicUpdate = new QTimer(this);
//    connect(pPeriodicUpdate, SIGNAL(timeout()),
//            this, SLOT(onPeriodicUpdate()));
//    pPeriodicUpdate->start(updatePeriod);

    // Pan-Tilt Camera management
    connect(pClientListDialog, SIGNAL(disableVideo()),
            this, SLOT(onStopCamera()));
    connect(pClientListDialog, SIGNAL(enableVideo(QString)),
            this, SLOT(onStartCamera(QString)));
    connect(pClientListDialog, SIGNAL(newPanValue(QString,int)),
            this, SLOT(onSetNewPanValue(QString, int)));
    connect(pClientListDialog, SIGNAL(newTiltValue(QString,int)),
            this, SLOT(onSetNewTiltValue(QString, int)));

    QGridLayout*  mainLayout = new QGridLayout();

    mainLayout->addWidget(createGamePanel(),     0, 0, 3, 1);
    mainLayout->addWidget(createGameButtonBox(), 3, 0, 1, 2);
    mainLayout->addWidget(createSpotButtonBox(), 0, 2, 4, 1);
    setLayout(mainLayout);
}


void
ScoreController::WaitForNetworkReady() {
    int iResponse;
    while(!isConnectedToNetwork()) {
        iResponse = QMessageBox::critical(this,
                                          "Connessione Assente",
                                          "Connettiti alla rete e ritenta",
                                          QMessageBox::Retry,
                                          QMessageBox::Ignore,
                                          QMessageBox::Abort);

        if(iResponse == QMessageBox::Ignore) {
            break;
        } else if(iResponse == QMessageBox::Abort) {
            pExitTimer->start(1000);
            QCursor waitCursor;
            waitCursor.setShape(Qt::WaitCursor);
            setCursor(waitCursor);
            break;
        }
        QThread::sleep(1);
    }
}


ScoreController::~ScoreController() {
}


void
ScoreController::onFileServerDone(bool bError) {
    QString sFunctionName = QString(" ScoreController::onFileServerDone ");
    if(bError) {
        logMessage(sFunctionName, QString("File server stopped with errors"));
    }
    else {
        logMessage(sFunctionName, QString("File server stopped without errors"));
    }
}


void
ScoreController::logMessage(QString sFunctionName, QString sMessage) {
    Q_UNUSED(sFunctionName)
    Q_UNUSED(sMessage)
#ifdef LOG_MESG
    sDebugMessage = QString();
    sDebugInformation << dateTime.currentDateTime().toString()
                      << sFunctionName
                      << sMessage;
    if(logFile) {
      logFile->write(sDebugMessage.toUtf8().data());
      logFile->write("\n");
      logFile->flush();
    }
    else {
        qDebug() << sDebugMessage;
    }
#endif
}


void
ScoreController::prepareDiscovery() {
    QString sFunctionName = QString(" ScoreController::prepareDiscovery ");
    sIpAddresses = QString();
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    for(int i=0; i<interfaceList.count(); i++)
    {
        QNetworkInterface interface = interfaceList.at(i);
        if(interface.flags().testFlag(QNetworkInterface::IsUp) &&
           interface.flags().testFlag(QNetworkInterface::IsRunning) &&
           interface.flags().testFlag(QNetworkInterface::CanMulticast) &&
          !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            QList<QNetworkAddressEntry> list = interface.addressEntries();
            for(int j=0; j<list.count(); j++)
            {
                QUdpSocket* pDiscoverySocket = new QUdpSocket(this);
                if(list[j].ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    if(pDiscoverySocket->bind(QHostAddress::AnyIPv4, discoveryPort, QUdpSocket::ShareAddress)) {
                        pDiscoverySocket->joinMulticastGroup(discoveryAddress);
                        sIpAddresses += list[j].ip().toString() + tr(",");
                        discoverySocketArray.append(pDiscoverySocket);
                        connect(pDiscoverySocket, SIGNAL(readyRead()),
                                this, SLOT(onProcessConnectionRequest()));
                        logMessage(sFunctionName,
                                   QString("Listening for connections at address: %1 port:%2")
                                   .arg(discoveryAddress.toString())
                                   .arg(discoveryPort));
                    }
                    else {
                        logMessage(sFunctionName,
                                   QString("Unable to bound %1")
                                   .arg(discoveryAddress.toString()));
                    }
                }
            }// for(int j=0; j<list.count(); j++)
        }
    }// for(int i=0; i<interfaceList.count(); i++)
}


void
ScoreController::onStartCamera(QString sClientIp) {
    QHostAddress hostAddress(sClientIp);
    for(int i=0; i<connectionList.count(); i++) {
        if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
            QString sMessage = QString("<live>1</live>");
            SendToOne(connectionList.at(i).pClientSocket, sMessage);
            sMessage = QString("<getPanTilt>1</getPanTilt>");
            SendToOne(connectionList.at(i).pClientSocket, sMessage);
            return;
        }
    }
}


void
ScoreController::onStopCamera() {
    QString sMessage = QString("<endlive>1</endlive>");
    SendToAll(sMessage);
}


void
ScoreController::onSetNewPanValue(QString sClientIp, int newPan) {
  QHostAddress hostAddress(sClientIp);
  for(int i=0; i<connectionList.count(); i++) {
      if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
          sMessage = tr("<pan>%1</pan>").arg(newPan);
          SendToOne(connectionList.at(i).pClientSocket, sMessage);
          return;
      }
  }
}


void
ScoreController::onSetNewTiltValue(QString sClientIp, int newTilt) {
  QHostAddress hostAddress(sClientIp);
  for(int i=0; i<connectionList.count(); i++) {
      if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
          sMessage = tr("<tilt>%1</tilt>").arg(newTilt);
          SendToOne(connectionList.at(i).pClientSocket, sMessage);
          return;
      }
  }
}


bool
ScoreController::PrepareLogFile() {
#ifdef LOG_MESG
    QFileInfo checkFile(logFileName);
    if(checkFile.exists() && checkFile.isFile()) {
        QDir renamed;
        renamed.remove(logFileName+QString(".bkp"));
        renamed.rename(logFileName, logFileName+QString(".bkp"));
    }
    logFile = new QFile(logFileName);
    if (!logFile->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Volley Controller"),
                                 tr("Impossibile aprire il file %1: %2.")
                                 .arg(logFileName).arg(logFile->errorString()));
        delete logFile;
        logFile = NULL;
    }
#endif
    return true;
}


bool
ScoreController::isConnectedToNetwork() {
    QString sFunctionName = " ScoreController::isConnectedToNetwork ";
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;

    for(int i=0; i<ifaces.count(); i++) {
        QNetworkInterface iface = ifaces.at(i);
        if(iface.flags().testFlag(QNetworkInterface::IsUp) &&
           iface.flags().testFlag(QNetworkInterface::IsRunning) &&
           iface.flags().testFlag(QNetworkInterface::CanBroadcast) &&
          !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            for(int j=0; j<iface.addressEntries().count(); j++) {
                if(!result) result = true;
            }
        }
    }
    logMessage(sFunctionName, result ? QString("true") : QString("false"));
    return result;
}

