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

#include <QDir>
#include <QTimer>
#include <QMessageBox>
#include <QThread>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QWebSocket>
#include <QBuffer>
#include <QFileDialog>
#include <QPushButton>
#include <QHostInfo>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QCloseEvent>
#include <QProcessEnvironment>
#include <QStandardPaths>

#include "scorecontroller.h"
#include "clientlistdialog.h"
#include "utility.h"
#include "fileserver.h"
#include "generalsetupdialog.h"


#define DISCOVERY_PORT      45453
#define SERVER_SOCKET_PORT  45454
#define SPOT_UPDATER_PORT   45455
#define SLIDE_UPDATER_PORT  45456


/*!
 * \brief ScoreController::ScoreController
 * This is the base class of all the game controllers
 * \param myPanelType
 * \param parent
 */
ScoreController::ScoreController(int myPanelType, QWidget *parent)
    : QWidget(parent)
    , pSettings(Q_NULLPTR)
    , pGeneralSetupDialog(Q_NULLPTR)
    , panelType(myPanelType)
    , pClientListDialog(Q_NULLPTR)
    , discoveryPort(DISCOVERY_PORT)
    , serverPort(SERVER_SOCKET_PORT)
    , discoveryAddress(QHostAddress("224.0.0.1"))
    , pSlideServerThread(Q_NULLPTR)
    , pSlideUpdaterServer(Q_NULLPTR)
    , slideUpdaterPort(SLIDE_UPDATER_PORT)
    , pSpotServerThread(Q_NULLPTR)
    , pSpotUpdaterServer(Q_NULLPTR)
    , spotUpdaterPort(SPOT_UPDATER_PORT)
    , pButtonClick(Q_NULLPTR)
{
    // For Message Logging...
    logFile = Q_NULLPTR;
    QIcon myIcon(WINDOW_ICON);
    setWindowIcon(myIcon);

    // Block until a network connection is available
    if(WaitForNetworkReady() != QMessageBox::Ok) {
        exit(0);
    }

    // Initialize some useful dialogs...
    pGeneralSetupDialog = new GeneralSetupDialog(panelType, this);
    pGeneralSetupDialog->setWindowFlags(Qt::Window);
    pClientListDialog = new ClientListDialog(this);
    pClientListDialog->setWindowFlags(Qt::Window);

    // The click sound for button press.
    // To have an acoustic feedback on touch screen tablets.
    pButtonClick = new QSoundEffect(this);
    pButtonClick->setSource(QUrl::fromLocalFile(":/key.wav"));

    // A List of IP Addresses of the connected Score Panels
    sIpAddresses = QStringList();

    // Logged messages (if enabled) will be written in the following folder
    sLogDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    if(!sLogDir.endsWith(QString("/"))) sLogDir+= QString("/");

    // The default Directories to look for the slides and spots
    sSlideDir   = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if(!sSlideDir.endsWith(QString("/"))) sSlideDir+= QString("/");
    sSpotDir    = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    if(!sSpotDir.endsWith(QString("/"))) sSpotDir+= QString("/");

    slideList     = QFileInfoList();
    spotList      = QFileInfoList();
    iCurrentSlide = 0;
    iCurrentSpot  = 0;

    if((panelType < FIRST_PANEL_TYPE) ||
       (panelType > LAST_PANEL_TYPE)) {
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Panel Type forced to FIRST_PANEL_TYPE"));
        panelType = FIRST_PANEL_TYPE;
    }

    // Pan-Tilt Camera management
    connect(pClientListDialog, SIGNAL(disableVideo()),
            this, SLOT(onStopCamera()));
    connect(pClientListDialog, SIGNAL(enableVideo(QString)),
            this, SLOT(onStartCamera(QString)));
    connect(pClientListDialog, SIGNAL(newPanValue(QString,int)),
            this, SLOT(onSetNewPanValue(QString,int)));
    connect(pClientListDialog, SIGNAL(newTiltValue(QString,int)),
            this, SLOT(onSetNewTiltValue(QString,int)));
    // Panel orientation management
    connect(pClientListDialog, SIGNAL(getDirection(QString)),
            this, SLOT(onGetPanelDirection(QString)));
    connect(pClientListDialog, SIGNAL(changeDirection(QString,PanelDirection)),
            this, SLOT(onChangePanelDirection(QString,PanelDirection)));
    // Score Only Panel management
    connect(pClientListDialog, SIGNAL(getScoreOnly(QString)),
            this, SLOT(onGetIsPanelScoreOnly(QString)));
    connect(pClientListDialog, SIGNAL(changeScoreOnly(QString,bool)),
            this, SLOT(onSetScoreOnly(QString,bool)));

    myStatus = showPanel;
}


/*!
 * \brief ScoreController::prepareServices Prepare all the services for the Score Panels
 *
 * It Starts the Discovery service as well as the Slide and Spot file transfer services
 * It start the ScorePanel Server too.
 */
void
ScoreController::prepareServices() {
    // Start listening to the discovery port
    if(!prepareDiscovery()) {
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("!prepareDiscovery()"));
        close();
    }
    // Prepare the Server port for the Panels to connect to
    else if(!prepareServer()) {
        close();
    }
    else {
        prepareSpotUpdateService();
        prepareSlideUpdateService();
    }
}


/*!
 * \brief ScoreController::prepareDirectories
 * To select the directories from which the Slides and the Spots will be taken.
 */
void
ScoreController::prepareDirectories() {
    QDir slideDir(sSlideDir);
    QDir spotDir(sSpotDir);

    if(!slideDir.exists() || !spotDir.exists()) {
        onButtonSetupClicked();
        slideDir.setPath(sSlideDir);
        if(!slideDir.exists())
            sSlideDir = QStandardPaths::displayName(QStandardPaths::GenericDataLocation);
        if(!sSlideDir.endsWith(QString("/"))) sSlideDir+= QString("/");
        spotDir.setPath(sSpotDir);
        if(!spotDir.exists())
            sSpotDir = QStandardPaths::displayName(QStandardPaths::GenericDataLocation);
        if(!sSpotDir.endsWith(QString("/"))) sSpotDir+= QString("/");
        pSettings->setValue("directories/slides", sSlideDir);
        pSettings->setValue("directories/spots", sSpotDir);
    }
    else {
        QStringList filter(QStringList() << "*.jpg" << "*.jpeg" << "*.png" << "*.JPG" << "*.JPEG" << "*.PNG");
        slideDir.setNameFilters(filter);
        slideList = slideDir.entryInfoList();
#ifdef LOG_VERBOSE
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Slides directory: %1 Found %2 Slides")
                   .arg(sSlideDir)
                   .arg(slideList.count()));
#endif
        QStringList nameFilter(QStringList() << "*.mp4"<< "*.MP4");
        spotDir.setNameFilters(nameFilter);
        spotDir.setFilter(QDir::Files);
        spotList = spotDir.entryInfoList();
#ifdef LOG_VERBOSE
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Spot directory: %1 Found %2 Spots")
                   .arg(sSpotDir)
                   .arg(spotList.count()));
#endif
    }
}


/*!
 * \brief ScoreController::prepareSpotUpdateService Starts a "Spot Update" Service
 */
void
ScoreController::prepareSpotUpdateService() {
    pSpotUpdaterServer = new FileServer(QString("SpotUpdater"), logFile, Q_NULLPTR);
    connect(pSpotUpdaterServer, SIGNAL(fileServerDone(bool)),
            this, SLOT(onSpotServerDone(bool)));
    pSpotUpdaterServer->setServerPort(spotUpdaterPort);
    pSpotServerThread = new QThread();
    pSpotUpdaterServer->moveToThread(pSpotServerThread);
    connect(this, SIGNAL(startSpotServer()),
            pSpotUpdaterServer, SLOT(onStartServer()));
    connect(this, SIGNAL(closeSpotServer()),
            pSpotUpdaterServer, SLOT(onCloseServer()));
    pSpotServerThread->start(QThread::LowestPriority);
}


/*!
 * \brief ScoreController::prepareSlideUpdateService Starts a "Slide Update" Service
 */
void
ScoreController::prepareSlideUpdateService() {
    pSlideUpdaterServer = new FileServer(QString("SlideUpdater"), logFile, Q_NULLPTR);
    connect(pSlideUpdaterServer, SIGNAL(fileServerDone(bool)),
            this, SLOT(onSlideServerDone(bool)));
    pSlideUpdaterServer->setServerPort(slideUpdaterPort);
    pSlideServerThread = new QThread();
    pSlideUpdaterServer->moveToThread(pSlideServerThread);
    connect(this, SIGNAL(startSlideServer()),
            pSlideUpdaterServer, SLOT(onStartServer()));
    connect(this, SIGNAL(closeSlideServer()),
            pSlideUpdaterServer, SLOT(onCloseServer()));
    pSlideServerThread->start(QThread::LowestPriority);
}


/*!
 * \brief ScoreController::WaitForNetworkReady Wait For Network Ready
 * \return
 */
int
ScoreController::WaitForNetworkReady() {
    int iResponse;
    while(!isConnectedToNetwork()) {
        iResponse = QMessageBox::critical(this,
                                          tr("Connessione Assente"),
                                          tr("Connettiti alla rete e ritenta"),
                                          QMessageBox::Retry,
                                          QMessageBox::Abort);

        if(iResponse == QMessageBox::Abort) {
            return iResponse;
        }
        QThread::sleep(1);
    }
    return QMessageBox::Ok;
}


// Do nothing. All the housekeeping is done in "closeEvent()" manager
ScoreController::~ScoreController() = default;


/*!
 * \brief ScoreController::onSlideServerDone
 * Called from the Slide Updater Server when a transfer with a client has completed
 * \param bError
 */
void
ScoreController::onSlideServerDone(bool bError) {
    Q_UNUSED(bError)
#ifdef LOG_VERBOSE
    // Log a Message just to inform
    if(bError) {
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Slide server stopped with errors"));
    }
    else {
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Slide server stopped without errors"));
    }
#endif
}


/*!
 * \brief ScoreController::onSpotServerDone Called from the Spot Updater Server when a
 * transfer with a client has completed
 * \param bError
 */
void
ScoreController::onSpotServerDone(bool bError) {
    Q_UNUSED(bError)
#ifdef LOG_VERBOSE
    // Log a Message just to inform
    if(bError) {
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Spot server stopped with errors"));
    }
    else {
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Spot server stopped without errors"));
    }
#endif
}


/*!
 * \brief ScoreController::prepareDiscover Start a "Discovery Service"y
 * \return
 *
 * Start a "Discovery Service" that make possible to clients to discover
 * the presence of this Score Controller Server, independently from its
 * network address.
 * It listen for a short message from clients and then
 * send back an appropriate answer.
 */
bool
ScoreController::prepareDiscovery() {
    bool bSuccess = false;
    sIpAddresses = QStringList();
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    for(int i=0; i<interfaceList.count(); i++)
    {
        const QNetworkInterface& interface = interfaceList.at(i);
        if(interface.flags().testFlag(QNetworkInterface::IsUp) &&
           interface.flags().testFlag(QNetworkInterface::IsRunning) &&
           interface.flags().testFlag(QNetworkInterface::CanMulticast) &&
          !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            QList<QNetworkAddressEntry> list = interface.addressEntries();
            for(int j=0; j<list.count(); j++)
            {
                auto* pDiscoverySocket = new QUdpSocket(this);
                if(list[j].ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    if(pDiscoverySocket->bind(QHostAddress::AnyIPv4, discoveryPort, QUdpSocket::ShareAddress)) {
                        pDiscoverySocket->joinMulticastGroup(discoveryAddress);
                        sIpAddresses.append(list[j].ip().toString());
                        discoverySocketArray.append(pDiscoverySocket);
                        connect(pDiscoverySocket, SIGNAL(readyRead()),
                                this, SLOT(onProcessConnectionRequest()));
                        bSuccess = true;
#ifdef LOG_VERBOSE
                        logMessage(logFile,
                                   Q_FUNC_INFO,
                                   QString("Listening for connections at address: %1 port:%2")
                                   .arg(discoveryAddress.toString())
                                   .arg(discoveryPort));
#endif
                    }
                    else {
                        logMessage(logFile,
                                   Q_FUNC_INFO,
                                   QString("Unable to bound %1")
                                   .arg(discoveryAddress.toString()));
                    }
                }
            }// for(int j=0; j<list.count(); j++)
        }
    }// for(int i=0; i<interfaceList.count(); i++)
    return bSuccess;
}


/*!
 * \brief ScoreController::onStartCamera
 * Called when the user asked to start the live camera
 * \param sClientIp
 */
void
ScoreController::onStartCamera(const QString& sClientIp) {
    QHostAddress hostAddress(sClientIp);
    for(int i=0; i<connectionList.count(); i++) {
        if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
            QString sMessage = QString("<live>1</live>");
            SendToOne(connectionList.at(i).pClientSocket, sMessage);
            sMessage = QString("<getPanTilt>1</getPanTilt>");
            SendToOne(connectionList.at(i).pClientSocket, sMessage);
            return;
        }
        myStatus = showCamera;
    }
}


/*!
 * \brief ScoreController::onStopCamera Called when the user asked to stop the live camera
 */
void
ScoreController::onStopCamera() {
    QString sMessage = QString("<endlive>1</endlive>");
    SendToAll(sMessage);
    myStatus = showPanel;
}


/*!
 * \brief ScoreController::onSetNewPanValue
 * Called when the user asked to pan the live camera on a given Panel
 * \param sClientIp
 * \param newPan
 */
void
ScoreController::onSetNewPanValue(const QString& sClientIp, int newPan) {
  QHostAddress hostAddress(sClientIp);
  for(int i=0; i<connectionList.count(); i++) {
      if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
          QString sMessage = QString("<pan>%1</pan>").arg(newPan);
          SendToOne(connectionList.at(i).pClientSocket, sMessage);
          return;
      }
  }
}


/*!
 * \brief ScoreController::onSetNewTiltValue
 * Called when the user asked to tilt the live camera on a given Panel.
 * \param sClientIp
 * \param newTilt
 */
void
ScoreController::onSetNewTiltValue(const QString& sClientIp, int newTilt) {
  QHostAddress hostAddress(sClientIp);
  for(int i=0; i<connectionList.count(); i++) {
      if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
          QString sMessage = QString("<tilt>%1</tilt>").arg(newTilt);
          SendToOne(connectionList.at(i).pClientSocket, sMessage);
          return;
      }
  }
}


/*!
 * \brief ScoreController::onSetScoreOnly
 * Called when the user asked to set the panel to show only the score:
 * No slides, spots or camera.
 * \param sClientIp
 * \param bScoreOnly
 */
void
ScoreController::onSetScoreOnly(const QString& sClientIp, bool bScoreOnly) {
#ifdef LOG_VERBOSE
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Client %1 ScoreOnly: %2")
               .arg(sClientIp)
               .arg(bScoreOnly));
#endif
    QHostAddress hostAddress(sClientIp);
    for(int i=0; i<connectionList.count(); i++) {
        if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
            QString sMessage = QString("<setScoreOnly>%1</setScoreOnly>").arg(bScoreOnly);
            SendToOne(connectionList.at(i).pClientSocket, sMessage);
            return;
        }
    }
}


/*!
 * \brief ScoreController::prepareLogFile
 * Prepare the file for logging (if enabled at compilation time)
 * \return
 */
bool
ScoreController::prepareLogFile() {
#if defined(LOG_MESG)
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
                                 .arg(logFileName, logFile->errorString()));
        delete logFile;
        logFile = Q_NULLPTR;
    }
#endif
    return true;
}


/*!
 * \brief ScoreController::isConnectedToNetwork
 * Chech if the computer is connected to a network
 * \return true if a network connection is available
 */
bool
ScoreController::isConnectedToNetwork() {
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;

    for(int i=0; i<ifaces.count(); i++) {
        const QNetworkInterface& iface = ifaces.at(i);
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
#ifdef LOG_VERBOSE
    logMessage(logFile,
               Q_FUNC_INFO,
               result ? QString("true") : QString("false"));
#endif
    return result;
}


/*!
 * \brief ScoreController::onProcessConnectionRequest Called when a new client ask to be connected
 */
void
ScoreController::onProcessConnectionRequest() {
    QByteArray datagram, request;
    QString sToken;
    auto* pDiscoverySocket = qobject_cast<QUdpSocket*>(sender());
    QString sNoData = QString("NoData");
    QString sMessage;
    Q_UNUSED(sMessage)
    QHostAddress hostAddress;
    quint16 port=0;

    while(pDiscoverySocket->hasPendingDatagrams()) {
        datagram.resize(int(pDiscoverySocket->pendingDatagramSize()));
        pDiscoverySocket->readDatagram(datagram.data(), datagram.size(), &hostAddress, &port);
        request.append(datagram.data());
/*!
 * \todo Do we have to limit the maximum amount of data that can be received ???
 */
    }
    sToken = XML_Parse(request.data(), "getServer");
    if(sToken != sNoData) {
        sendAcceptConnection(pDiscoverySocket, hostAddress, port);
#ifdef LOG_VERBOSE
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Connection request from: %1 at Address %2:%3")
                   .arg(sToken, hostAddress.toString())
                   .arg(port));
#endif
        // If a Client with the same address asked for a Server it means that
        // the connections has dropped (at least it think so). Then remove it
        // from the connected clients list
        RemoveClient(hostAddress);
#ifdef LOG_VERBOSE
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Sent: %1")
                   .arg(sMessage));
#endif
        UpdateUI();// To disable some buttons if this was the last client
    }
}


/*!
 * \brief ScoreController::sendAcceptConnection
 * Called to accept a request connection from a Panel
 * \param pDiscoverySocket
 * \param hostAddress
 * \param port
 *
 * It sends to the client the IP addresses that this server
 * listen to for connections and the Panel Type to show.
 */
void
ScoreController::sendAcceptConnection(QUdpSocket* pDiscoverySocket, const QHostAddress& hostAddress, quint16 port) {
    QString sString = QString("%1,%2").arg(sIpAddresses.at(0)).arg(panelType);
    for(int i=1; i<sIpAddresses.count(); i++) {
        sString += QString(";%1,%2").arg(sIpAddresses.at(i)).arg(panelType);
    }
    QString sMessage = "<serverIP>" + sString + "</serverIP>";
    QByteArray datagram = sMessage.toUtf8();
    qint64 bytesWritten = pDiscoverySocket->writeDatagram(datagram.data(), datagram.size(), hostAddress, port);
    Q_UNUSED(bytesWritten)
    if(bytesWritten != datagram.size()) {
      logMessage(logFile,
                 Q_FUNC_INFO,
                 QString("Unable to send data !"));
    }
}


/*!
 * \brief ScoreController::closeEvent
 * Manage the termination of this server
 * \param event
 */
void
ScoreController::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    QString sMessage;
#ifdef LOG_VERBOSE
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Closing"));
#endif
    delete pButtonClick;
    pButtonClick = Q_NULLPTR;

    // Close all the discovery sockets
    for(int i=0; i<discoverySocketArray.count(); i++) {
        QUdpSocket* pSocket = discoverySocketArray.at(i);
        if(pSocket != Q_NULLPTR) {
            pSocket->disconnect();
            if(pSocket->isValid())
                pSocket->abort();
            pSocket->deleteLater();
        }
    }
    discoverySocketArray.clear();

    //Close the file transfer servers (if active)
    emit closeSpotServer();
    emit closeSlideServer();

    // If there are Panels connected would we switch they off ?
    if(connectionList.count() > 0) {
        int answer = QMessageBox::question(this,
                                           Q_FUNC_INFO,
                                           tr("Vuoi spegnere anche i pannelli ?"),
                                           QMessageBox::Yes,
                                           QMessageBox::No|QMessageBox::Default);
        if(answer == QMessageBox::No) {
            for(int i=0; i<connectionList.count(); i++) {
                connectionList.at(i).pClientSocket->disconnect();
                if(connectionList.at(i).pClientSocket->isValid())
                    connectionList.at(i).pClientSocket->close(QWebSocketProtocol::CloseCodeNormal,
                                                              "Server Closed");
                connectionList.at(i).pClientSocket->deleteLater();
            }
            connectionList.clear();
        }
        else {
            sMessage = "<kill>1</kill>";
            SendToAll(sMessage);
        }
    }
    // Close and delete the Panel Server
    if(pPanelServer != Q_NULLPTR) {
        pPanelServer->closeServer();
        pPanelServer->deleteLater();
        pPanelServer = Q_NULLPTR;
    }
    // Close and delete the QSettings Object
    if(pSettings != Q_NULLPTR) {
        delete pSettings;
        pSettings = Q_NULLPTR;
    }
    // Close and delete the Clients List Dialog
    if(pClientListDialog != Q_NULLPTR) {
        pClientListDialog->disconnect();
        delete pClientListDialog;
        pClientListDialog = Q_NULLPTR;
    }
    // Close and delete the General Setup Dialog
    if(pGeneralSetupDialog != Q_NULLPTR) {
        pGeneralSetupDialog->disconnect();
        delete pGeneralSetupDialog;
        pGeneralSetupDialog = Q_NULLPTR;
    }
    // Close the Log File (if any) and delete the Log File Object
    if(logFile) {
        logFile->flush();
        logFile->close();
        delete logFile;
        logFile = Q_NULLPTR;
    }
#ifdef LOG_VERBOSE
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Closed !"));
#endif
    // Finally Close this widget
    close();
}


/*!
 * \brief ScoreController::prepareServer
 * Prepare the Server for accepting new connections
 * \return
 */
bool
ScoreController::prepareServer() {
    pPanelServer = new NetServer(QString("PanelServer"), logFile, this);
    if(!pPanelServer->prepareServer(serverPort)) {
#ifdef LOG_VERBOSE
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("prepareServer() Failed !"));
#endif
        pPanelServer->deleteLater();
        pPanelServer = Q_NULLPTR;
        return false;
    }
    connect(pPanelServer, SIGNAL(newConnection(QWebSocket*)),
            this, SLOT(onNewConnection(QWebSocket*)));
    return true;
}


/*!
 * \brief ScoreController::onProcessTextMessage
 * Called to process the Text messages received by this Server
 * \param sMessage
 */
void
ScoreController::onProcessTextMessage(QString sMessage) {
    QString sToken;
    QString sNoData = QString("NoData");

    // The Panel is asking for the Status
    sToken = XML_Parse(sMessage, "getStatus");
    if(sToken != sNoData) {
        auto *pClient = qobject_cast<QWebSocket *>(sender());
        SendToOne(pClient, FormatStatusMsg());
    }// getStatus

    // The Panel communicates the local Pan and Tilt values
    sToken = XML_Parse(sMessage, "pan_tilt");
    if(sToken != sNoData) {
        QStringList values = QStringList(sToken.split(",",QString::SkipEmptyParts));
        pClientListDialog->remotePanTiltReceived(values.at(0).toInt(), values.at(1).toInt());
    }// pan_tilt

    // The Panel communicates its orientation
    sToken = XML_Parse(sMessage, "orientation");
    if(sToken != sNoData) {
        bool ok;
        int iDirection = sToken.toInt(&ok);
        if(!ok) {
            logMessage(logFile,
                       Q_FUNC_INFO,
                       QString("Illegal Direction received: %1")
                       .arg(sToken));
            return;
        }
        auto direction = static_cast<PanelDirection>(iDirection);
        pClientListDialog->remoteDirectionReceived(direction);
    }// orientation

    // The Panel communicates if it shows only the score
    sToken = XML_Parse(sMessage, "isScoreOnly");
    if(sToken != sNoData) {
        bool ok;
        auto isScoreOnly = bool(sToken.toInt(&ok));
        if(!ok) {
            logMessage(logFile,
                       Q_FUNC_INFO,
                       QString("Illegal Score Only value received: %1")
                       .arg(sToken));
            return;
        }
        pClientListDialog->remoteScoreOnlyValueReceived(isScoreOnly);
    }// isScoreOnly
}


/*!
 * \brief ScoreController::SendToAll
 * Send the same message to all the connected clients
 * \param sMessage The message sent
 * \return
 */
int
ScoreController::SendToAll(const QString& sMessage) {
#ifdef LOG_VERBOSE
    logMessage(logFile,
               Q_FUNC_INFO,
               sMessage);
#endif
    for(int i=0; i< connectionList.count(); i++) {
        SendToOne(connectionList.at(i).pClientSocket, sMessage);
    }
    return 0;
}


/*!
 * \brief ScoreController::SendToOne
 * Send a message to a single connected client
 * \param pClient The client
 * \param sMessage The message
 * \return
 */
int
ScoreController::SendToOne(QWebSocket* pClient, const QString& sMessage) {
    if (pClient->isValid()) {
        for(int i=0; i< connectionList.count(); i++) {
           if(connectionList.at(i).clientAddress.toIPv4Address() ==
              pClient->peerAddress().toIPv4Address()) {
                qint64 written = pClient->sendTextMessage(sMessage);
                Q_UNUSED(written)
                if(written != sMessage.length()) {
                    logMessage(logFile,
                               Q_FUNC_INFO,
                               QString("Error writing %1").arg(sMessage));
                }
#ifdef LOG_VERBOSE
                else {
                    logMessage(logFile,
                               Q_FUNC_INFO,
                               QString("Sent %1 to: %2")
                               .arg(sMessage, pClient->peerAddress().toString()));
                }
#endif
                break;
            }
        }
    }
    else {
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Client socket is invalid !"));
        RemoveClient(pClient->peerAddress());
        UpdateUI();
    }
    return 0;
}


/*!
 * \brief ScoreController::RemoveClient
 * Remove a client from the list of connected clients rebuilding the list itself
 * \param hAddress Address of the client to remove
 */
void
ScoreController::RemoveClient(const QHostAddress& hAddress) {
    QString sFound = QString(" Not present");
    Q_UNUSED(sFound)
    QWebSocket *pClientToClose = Q_NULLPTR;

    pClientListDialog->clear();
    for(int i=connectionList.count()-1; i>=0; i--) {
        if(connectionList.at(i).clientAddress.toIPv4Address() ==
           hAddress.toIPv4Address())
        {
            pClientToClose = connectionList.at(i).pClientSocket;
            pClientToClose->disconnect(); // No more events from this socket
            if(pClientToClose->isValid())
                pClientToClose->close(QWebSocketProtocol::CloseCodeNormal,
                                      tr("Socket disconnection"));
            pClientToClose->deleteLater();
            pClientToClose = Q_NULLPTR;
            connectionList.removeAt(i);
#ifdef LOG_VERBOSE
            sFound = " Removed !";
            logMessage(logFile,
                       Q_FUNC_INFO,
                       QString("%1 %2")
                       .arg(hAddress.toString(), sFound));
#endif
        } else {
            pClientListDialog->addItem(connectionList.at(i).clientAddress.toString());
        }
    }
}


/*!
 * \brief ScoreController::UpdateUI
 * To update the buttons upon the first connection or last disconnection
 */
void
ScoreController::UpdateUI() {
    if(connectionList.count() == 1) {
        startStopLoopSpotButton->setEnabled(true);
        startStopSlideShowButton->setEnabled(true);
        startStopLiveCameraButton->setEnabled(true);
        panelControlButton->setEnabled(true);
        //>>>>>>>generalSetupButton->setDisabled(true);
        shutdownButton->setEnabled(true);
    }
    else if(connectionList.count() == 0) {
        startStopLoopSpotButton->setDisabled(true);
        QPixmap pixmap(":/buttonIcons/PlaySpots.png");
        QIcon ButtonIcon(pixmap);
        startStopLoopSpotButton->setIcon(ButtonIcon);
        startStopLoopSpotButton->setIconSize(pixmap.rect().size());

        startStopSlideShowButton->setDisabled(true);
        pixmap.load(":/buttonIcons/PlaySlides.png");
        ButtonIcon.addPixmap(pixmap);
        startStopSlideShowButton->setIcon(ButtonIcon);
        startStopSlideShowButton->setIconSize(pixmap.rect().size());

        startStopLiveCameraButton->setDisabled(true);
        pixmap.load(":/buttonIcons/Camera.png");
        ButtonIcon.addPixmap(pixmap);
        startStopLiveCameraButton->setIcon(ButtonIcon);
        startStopLiveCameraButton->setIconSize(pixmap.rect().size());

        panelControlButton->setDisabled(true);
        generalSetupButton->setEnabled(true);
        shutdownButton->setDisabled(true);
        myStatus = showPanel;
    }
}


/*!
 * \brief ScoreController::onNewConnection
 * Invoked when a new Panel ask to be connected
 * \param pClient The Panel WebSocket pointer
 */
void
ScoreController::onNewConnection(QWebSocket *pClient) {
    QHostAddress address = pClient->peerAddress();
    QString sAddress = address.toString();

    connect(pClient, SIGNAL(textMessageReceived(QString)),
            this, SLOT(onProcessTextMessage(QString)));
    connect(pClient, SIGNAL(binaryMessageReceived(QByteArray)),
            this, SLOT(onProcessBinaryMessage(QByteArray)));
    connect(pClient, SIGNAL(disconnected()),
            this, SLOT(onClientDisconnected()));

    RemoveClient(address);

    connection newConnection;
    newConnection.pClientSocket = pClient;
    newConnection.clientAddress = address;
    connectionList.append(newConnection);
    pClientListDialog->addItem(sAddress);
    UpdateUI();
#ifdef LOG_VERBOSE
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Client connected: %1")
               .arg(sAddress));
#endif
}


/*!
 * \brief ScoreController::onClientDisconnected
 * Invoked when a Panel disconnects (Usually because a network problem)
 */
void
ScoreController::onClientDisconnected() {
    auto* pClient = qobject_cast<QWebSocket *>(sender());
    QString sDiconnectedAddress = pClient->peerAddress().toString();
#ifdef LOG_VERBOSE
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("%1 disconnected because %2. Close code: %3")
               .arg(sDiconnectedAddress, pClient->closeReason())
               .arg(pClient->closeCode()));
#endif
    RemoveClient(pClient->peerAddress());
    UpdateUI();
}


/*!
 * \brief ScoreController::onProcessBinaryMessage
 * Should never be called !
 * \param message
 */
void
ScoreController::onProcessBinaryMessage(QByteArray message) {
    Q_UNUSED(message)
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Unexpected binary message received !"));
}


/*!
 * \brief ScoreController::CreateSpotButtons
 * Create the Button and they Layout
 * \return The Buttons Layout
 */
QHBoxLayout*
ScoreController::CreateSpotButtons() {
    auto* spotButtonLayout = new QHBoxLayout();

    QPixmap pixmap(":/buttonIcons/PlaySpots.png");
    QIcon ButtonIcon(pixmap);
    startStopLoopSpotButton = new QPushButton(ButtonIcon, "");
    startStopLoopSpotButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/PlaySlides.png");
    ButtonIcon.addPixmap(pixmap);
    startStopSlideShowButton = new QPushButton(ButtonIcon, "");
    startStopSlideShowButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/Camera.png");
    ButtonIcon.addPixmap(pixmap);
    startStopLiveCameraButton = new QPushButton(ButtonIcon, "");
    startStopLiveCameraButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/PanelSetup.png");
    ButtonIcon.addPixmap(pixmap);
    panelControlButton = new QPushButton(ButtonIcon, "");
    panelControlButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/GeneralSetup.png");
    ButtonIcon.addPixmap(pixmap);
    generalSetupButton = new QPushButton(ButtonIcon, "");
    generalSetupButton->setIconSize(pixmap.rect().size());

    pixmap.load(":/buttonIcons/video-display.png");
    ButtonIcon.addPixmap(pixmap);
    shutdownButton = new QPushButton(ButtonIcon, "");
    shutdownButton->setIconSize(pixmap.rect().size());

    startStopLoopSpotButton->setDisabled(true);
    startStopSlideShowButton->setDisabled(true);
    startStopLiveCameraButton->setDisabled(true);

    panelControlButton->setDisabled(true);
    generalSetupButton->setEnabled(true);
    shutdownButton->setDisabled(true);

    connect(panelControlButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));
    connect(panelControlButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonPanelControlClicked()));

    connect(startStopLoopSpotButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonStartStopSpotLoopClicked()));
    connect(startStopLoopSpotButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));

    connect(startStopSlideShowButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonStartStopSlideShowClicked()));
    connect(startStopSlideShowButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));

    connect(startStopLiveCameraButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonStartStopLiveCameraClicked()));
    connect(startStopLiveCameraButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));

    connect(generalSetupButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonSetupClicked()));
    connect(generalSetupButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));

    connect(shutdownButton, SIGNAL(clicked(bool)),
            this, SLOT(onButtonShutdownClicked()));
    connect(shutdownButton, SIGNAL(clicked()),
            pButtonClick, SLOT(play()));

    spotButtonLayout->addWidget(startStopLoopSpotButton);

    spotButtonLayout->addStretch();
    spotButtonLayout->addWidget(startStopSlideShowButton);

    spotButtonLayout->addStretch();
    spotButtonLayout->addWidget(startStopLiveCameraButton);

    spotButtonLayout->addStretch();
    spotButtonLayout->addWidget(panelControlButton);

    spotButtonLayout->addStretch();

    spotButtonLayout->addWidget(generalSetupButton);
    spotButtonLayout->addWidget(shutdownButton);

    return spotButtonLayout;
}


/*!
 * \brief ScoreController::onButtonStartStopSpotLoopClicked
 */
void
ScoreController::onButtonStartStopSpotLoopClicked() {
    QString sMessage;
    QPixmap pixmap;
    QIcon ButtonIcon;
    if(connectionList.count() == 0) {
        pixmap.load(":/buttonIcons/PlaySpots.png");
        ButtonIcon.addPixmap(pixmap);
        startStopLoopSpotButton->setIcon(ButtonIcon);
        startStopLoopSpotButton->setIconSize(pixmap.rect().size());
        startStopLoopSpotButton->setDisabled(true);
        myStatus = showPanel;
        return;
    }
    if(myStatus == showPanel) {
        sMessage = QString("<spotloop>1</spotloop>");
        SendToAll(sMessage);
        pixmap.load(":/buttonIcons/sign_stop.png");
        ButtonIcon.addPixmap(pixmap);
        startStopLoopSpotButton->setIcon(ButtonIcon);
        startStopLoopSpotButton->setIconSize(pixmap.rect().size());
        startStopSlideShowButton->setDisabled(true);
        startStopLiveCameraButton->setDisabled(true);
        myStatus = showSpots;
    }
    else {
        sMessage = "<endspotloop>1</endspotloop>";
        SendToAll(sMessage);
        pixmap.load(":/buttonIcons/PlaySpots.png");
        ButtonIcon.addPixmap(pixmap);
        startStopLoopSpotButton->setIcon(ButtonIcon);
        startStopLoopSpotButton->setIconSize(pixmap.rect().size());
        startStopSlideShowButton->setDisabled(false);
        startStopLiveCameraButton->setDisabled(false);
        myStatus = showPanel;
    }
}


/*!
 * \brief ScoreController::onButtonStartStopLiveCameraClicked
 */
void
ScoreController::onButtonStartStopLiveCameraClicked() {
    QString sMessage;
    QPixmap pixmap;
    QIcon ButtonIcon;
    if(connectionList.count() == 0) {
        pixmap.load(":/buttonIcons/Camera.png");
        ButtonIcon.addPixmap(pixmap);
        startStopLiveCameraButton = new QPushButton(ButtonIcon, "");
        startStopLiveCameraButton->setIconSize(pixmap.rect().size());
        startStopLiveCameraButton->setDisabled(true);
        myStatus = showPanel;
        return;
    }
    if(myStatus == showPanel) {
        sMessage = QString("<live>1</live>");
        SendToAll(sMessage);
        pixmap.load(":/buttonIcons/sign_stop.png");
        ButtonIcon.addPixmap(pixmap);
        startStopLiveCameraButton->setIcon(ButtonIcon);
        startStopLiveCameraButton->setIconSize(pixmap.rect().size());
        startStopLoopSpotButton->setDisabled(true);
        startStopSlideShowButton->setDisabled(true);
        myStatus = showCamera;
    }
    else {
        sMessage = "<endlive>1</endlive>";
        SendToAll(sMessage);
        pixmap.load(":/buttonIcons/Camera.png");
        ButtonIcon.addPixmap(pixmap);
        startStopLiveCameraButton->setIcon(ButtonIcon);
        startStopLiveCameraButton->setIconSize(pixmap.rect().size());
        startStopLoopSpotButton->setDisabled(false);
        startStopSlideShowButton->setDisabled(false);
        myStatus = showPanel;
    }
}


/*!
 * \brief ScoreController::onButtonStartStopSlideShowClicked
 */
void
ScoreController::onButtonStartStopSlideShowClicked() {
    QString sMessage;
    QPixmap pixmap;
    QIcon ButtonIcon;
    if(connectionList.count() == 0) {
        pixmap.load(":/buttonIcons/PlaySlides.png");
        ButtonIcon.addPixmap(pixmap);
        startStopSlideShowButton->setIcon(ButtonIcon);
        startStopSlideShowButton->setIconSize(pixmap.rect().size());
        startStopSlideShowButton->setDisabled(true);
        myStatus = showPanel;
        return;
    }
    if(myStatus == showPanel) {
        sMessage = "<slideshow>1</slideshow>";
        SendToAll(sMessage);
        startStopLoopSpotButton->setDisabled(true);
        startStopLiveCameraButton->setDisabled(true);
        pixmap.load(":/buttonIcons/sign_stop.png");
        ButtonIcon.addPixmap(pixmap);
        startStopSlideShowButton->setIcon(ButtonIcon);
        startStopSlideShowButton->setIconSize(pixmap.rect().size());
        myStatus = showSlides;
    }
    else {
        sMessage = "<endslideshow>1</endslideshow>";
        SendToAll(sMessage);
        startStopLoopSpotButton->setEnabled(true);
        startStopLiveCameraButton->setEnabled(true);
        pixmap.load(":/buttonIcons/PlaySlides.png");
        ButtonIcon.addPixmap(pixmap);
        startStopSlideShowButton->setIcon(ButtonIcon);
        startStopSlideShowButton->setIconSize(pixmap.rect().size());
        myStatus = showPanel;
    }
}


/*!
 * \brief ScoreController::onButtonShutdownClicked
 */
void
ScoreController::onButtonShutdownClicked() {
    int iRes = QMessageBox::question(this,
                                     tr("ScoreController"),
                                     tr("Vuoi Spegnere i Tabelloni ?"),
                                     QMessageBox::Yes,
                                     QMessageBox::No|QMessageBox::Default,
                                     QMessageBox::NoButton);
    if(iRes != QMessageBox::Yes) return;
    QString sMessage = "<kill>1</kill>";
    SendToAll(sMessage);
}


/*!
 * \brief ScoreController::onButtonPanelControlClicked
 */
void
ScoreController::onButtonPanelControlClicked() {
    pClientListDialog->exec();
}


/*!
 * \brief ScoreController::onButtonSetupClicked
 */
void
ScoreController::onButtonSetupClicked() {
    pGeneralSetupDialog->setSlideDir(sSlideDir);
    pGeneralSetupDialog->setSpotDir(sSpotDir);
    if(pGeneralSetupDialog->exec() == QDialog::Accepted) {
        sSlideDir = pGeneralSetupDialog->getSlideDir();
        if(!sSlideDir.endsWith(QString("/"))) sSlideDir+= QString("/");
        QDir slideDir(sSlideDir);
        if(sSlideDir != QString() && slideDir.exists()) {
            QStringList filter(QStringList() << "*.jpg" << "*.jpeg" << "*.png" << "*.JPG" << "*.JPEG" << "*.PNG");
            slideDir.setNameFilters(filter);
            slideList = slideDir.entryInfoList();
        }
        else {
            sSlideDir = QStandardPaths::displayName(QStandardPaths::GenericDataLocation);
            slideList = QFileInfoList();
        }
#ifdef LOG_MESSAGE
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Found %1 slides").arg(slideList.count()));
#endif
        sSpotDir = pGeneralSetupDialog->getSpotDir();
        if(!sSpotDir.endsWith(QString("/"))) sSpotDir+= QString("/");
        QDir spotDir(sSpotDir);
        if(sSpotDir != QString() && spotDir.exists()) {
            QStringList nameFilter(QStringList() << "*.mp4" << "*.MP4");
            spotDir.setNameFilters(nameFilter);
            spotDir.setFilter(QDir::Files);
            spotList = spotDir.entryInfoList();
        }
        else {
            sSpotDir = QStandardPaths::displayName(QStandardPaths::GenericDataLocation);
            spotList = QFileInfoList();
        }
#ifdef LOG_MESSAGE
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("Found %1 spots")
                   .arg(spotList.count()));
#endif
        SaveStatus();
    }
}


/*!
 * \brief ScoreController::SaveStatus
 * Just a place holder
 */
void
ScoreController::SaveStatus() {
}


/*!
 * \brief ScoreController::FormatStatusMsg
 * Just a place holder
 * \return
 */
QString
ScoreController::FormatStatusMsg() {
    QString sMessage = QString();
    return sMessage;
}


/*!
 * \brief ScoreController::onGetPanelDirection
 * \param sClientIp
 */
void
ScoreController::onGetPanelDirection(const QString& sClientIp) {
    QHostAddress hostAddress(sClientIp);
    for(int i=0; i<connectionList.count(); i++) {
        if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
            QString sMessage = "<getOrientation>1</getOrientation>";
            SendToOne(connectionList.at(i).pClientSocket, sMessage);
            return;
        }
    }
}


/*!
 * \brief ScoreController::onGetIsPanelScoreOnly
 * \param sClientIp
 */
void
ScoreController::onGetIsPanelScoreOnly(const QString& sClientIp) {
    QHostAddress hostAddress(sClientIp);
    for(int i=0; i<connectionList.count(); i++) {
        if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
            QString sMessage = "<getScoreOnly>1</getScoreOnly>";
            SendToOne(connectionList.at(i).pClientSocket, sMessage);
            return;
        }
    }
}


/*!
 * \brief ScoreController::onChangePanelDirection
 * \param sClientIp
 * \param direction
 */
void
ScoreController::onChangePanelDirection(const QString& sClientIp, PanelDirection direction) {
#ifdef LOG_VERBOSE
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("Client %1 Direction %2")
               .arg(sClientIp)
               .arg(static_cast<int>(direction)));
#endif
    QHostAddress hostAddress(sClientIp);
    for(int i=0; i<connectionList.count(); i++) {
        if(connectionList.at(i).clientAddress.toIPv4Address() == hostAddress.toIPv4Address()) {
            QString sMessage = QString("<setOrientation>%1</setOrientation>")
                                       .arg(static_cast<int>(direction));
            SendToOne(connectionList.at(i).pClientSocket, sMessage);
            return;
        }
    }
}
