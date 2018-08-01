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

#include "netServer.h"
#include "utility.h"

#include <QtNetwork>
#include <QWebSocket>
#include <utility>
#include <utility>

#define LOG_MESG

NetServer::NetServer(QString _serverName, QFile* _logFile, QObject *parent)
    : QObject(parent)
    , sServerName(std::move(_serverName))
    , logFile(_logFile)
{
    pServerSocket = Q_NULLPTR;
}


bool
NetServer::prepareServer(quint16 serverPort) {
    pServerSocket = new QWebSocketServer(QStringLiteral("Server"),
                                         QWebSocketServer::NonSecureMode,
                                         this);
    connect(pServerSocket, SIGNAL(newConnection()),
            this, SLOT(onNewServerConnection()));
    connect(pServerSocket, SIGNAL(serverError(QWebSocketProtocol::CloseCode)),
            this, SLOT(onServerError(QWebSocketProtocol::CloseCode)));
    if (!pServerSocket->listen(QHostAddress::Any, serverPort)) {
        logMessage(logFile,
                   Q_FUNC_INFO,
                   QString("%1 - Impossibile to listen port %2 !")
                   .arg(sServerName)
                   .arg(serverPort));
        return false;
    }
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("%1 - listening on port:%2")
               .arg(sServerName)
               .arg(serverPort));

    return true;
}


void
NetServer::onServerError(QWebSocketProtocol::CloseCode closeCode){
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("%1 - %2 Close code: %3")
               .arg(sServerName, pServerSocket->serverAddress().toString())
               .arg(closeCode));
    emit netServerError(closeCode);
}


void
NetServer::onNewServerConnection() {
    QWebSocket *pClient = pServerSocket->nextPendingConnection();
    logMessage(logFile,
               Q_FUNC_INFO,
               QString("%1 - Client %2 connected")
               .arg(sServerName, pClient->peerAddress().toString()));

    emit newConnection(pClient);
}


void
NetServer::closeServer() {
    pServerSocket->disconnect();
    if(pServerSocket->isListening())
        pServerSocket->close();
    pServerSocket->deleteLater();
    pServerSocket = Q_NULLPTR;
    thread()->quit();
}
