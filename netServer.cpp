#include "netServer.h"
#include <QtNetwork>
#include <QWebSocket>

#define LOG_MESG

NetServer::NetServer(QString _serverName, QFile* _logFile, QObject *parent)
    : QObject(parent)
    , sServerName(_serverName)
    , logFile(_logFile)
{
    pServerSocket = Q_NULLPTR;
    sDebugInformation.setString(&sDebugMessage);
    sDebugMessage = QString();
}


void
NetServer::logMessage(QString sFunctionName, QString sMessage) {
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


int
NetServer::prepareServer(quint16 serverPort) {
    QString sFunctionName = " NetServer::prepareServer ";
    sDebugMessage = QString();
    pServerSocket = new QWebSocketServer(QStringLiteral("Server"),
                                         QWebSocketServer::NonSecureMode,
                                         this);
    connect(pServerSocket, SIGNAL(newConnection()),
            this, SLOT(onNewServerConnection()));
    connect(pServerSocket, SIGNAL(serverError(QWebSocketProtocol::CloseCode)),
            this, SLOT(onServerError(QWebSocketProtocol::CloseCode)));
    if (!pServerSocket->listen(QHostAddress::Any, serverPort)) {
        logMessage(sFunctionName, QString("Impossibile ascoltare la porta di update !"));
        return -7;
    }
    logMessage(sFunctionName,
               QString("%1 listening on port:%2")
               .arg(sServerName)
               .arg(serverPort));

    return 0;
}


void
NetServer::onServerError(QWebSocketProtocol::CloseCode closeCode){
    QString sFunctionName = " NetServer::onServerError ";
    sDebugMessage = QString();
    logMessage(sFunctionName,
               QString("%1 %2 Close code: %3")
               .arg(sServerName)
               .arg(pServerSocket->serverAddress().toString())
               .arg(closeCode));
    emit netServerError(closeCode);
}


void
NetServer::onNewServerConnection() {
    QString sFunctionName = " NetServer::onNewServerConnection ";
    Q_UNUSED(sFunctionName)

    QWebSocket *pClient = pServerSocket->nextPendingConnection();
    logMessage(sFunctionName,
               QString("%1: Client %2 connected")
               .arg(sServerName)
               .arg(pClient->peerAddress().toString()));

    emit newConnection(pClient);
}

