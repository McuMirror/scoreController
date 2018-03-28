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
#include "fileserver.h"
#include "utility.h"

#include <QFile>
#include <QDir>
#include <QThread>
#include <QWebSocket>
#include <QImage>
#include <QBuffer>



FileServer::FileServer(QString sName, QFile* _logFile, QObject *parent)
    : NetServer(sName, _logFile, parent)
    , serverName(sName)
{
    port      = 0;
    sFileDir  = QString();
    fileList  = QList<QFileInfo>();
    connections.clear();
}


void
FileServer::setServerPort(quint16 _port) {
    port = _port;
}


bool
FileServer::setDir(QString sDirectory, QString sExtensions) {
    QString sFunctionName = " FileServer::setDir ";
    sFileDir = sDirectory;
    if(!sFileDir.endsWith(QString("/")))  sFileDir+= QString("/");

    QDir sDir(sFileDir);
    if(sDir.exists()) {
        QStringList nameFilter(sExtensions.split(" "));
        sDir.setNameFilters(nameFilter);
        sDir.setFilter(QDir::Files);
        fileList = sDir.entryInfoList();
    }
#ifdef LOG_VERBOSE
    logMessage(logFile,
               sFunctionName,
               serverName +
               QString(" Found %1 files in %2")
               .arg(fileList.count())
               .arg(sFileDir));
#endif
    return true;
}


void
FileServer::onStartServer() {
    QString sFunctionName = " FileServer::onStartServer ";
    Q_UNUSED(sFunctionName)

    if(port == 0) {
        logMessage(logFile,
                   sFunctionName,
                   serverName +
                   QString(" Error! Server port not set."));
        emit fileServerDone(true);// Close with errors
        return;
    }

    prepareServer(port);

    connect(this, SIGNAL(newConnection(QWebSocket*)),
            this, SLOT(onNewConnection(QWebSocket*)));
    connect(this, SIGNAL(netServerError(QWebSocketProtocol::CloseCode)),
            this, SLOT(onFileServerError(QWebSocketProtocol::CloseCode)));
}


void
FileServer::onFileServerError(QWebSocketProtocol::CloseCode) {
    for(int i=connections.count()-1; i>=0; i--) {
        if(connections.at(i))
            delete connections.at(i);
    }
    connections.clear();
    emit fileServerDone(true);// Close File Server with errors !
}


void
FileServer::onNewConnection(QWebSocket *pClient) {
    QString sFunctionName = " FileServer::onNewConnection ";
    int nConnections = connections.count();
    for(int i=nConnections-1; i>=0; i--) {
        if(connections.at(i)->peerAddress() == pClient->peerAddress()) {
            logMessage(logFile,
                       sFunctionName,
                       serverName +
                       QString(" %1 Duplicate requests from %2")
                       .arg(sServerName)
                       .arg(pClient->peerAddress().toString()));
            if(connections.at(i)->isValid() && pClient->isValid()) {
                logMessage(logFile,
                           sFunctionName,
                           serverName +
                           QString(" Both sockets are valid! Removing the old connection"));
                connections.at(i)->close(QWebSocketProtocol::CloseCodeNormal, QString("Duplicated request"));
                delete connections.at(i);
            }
            else {
                if(pClient->isValid()) {
                    logMessage(logFile,
                               sFunctionName,
                               serverName +
                               QString(" Only present socket is valid. Removing the old one"));
                    connections.at(i)->close(QWebSocketProtocol::CloseCodeNormal, QString("Duplicated request"));
                    delete connections.at(i);
                }
                else {
                    logMessage(logFile,
                               sFunctionName,
                               serverName +
                               QString(" Present socket is not valid."));
                    pClient->close(QWebSocketProtocol::CloseCodeNormal, QString("Duplicated request"));
                    delete pClient;
                    return;
                }
            }
        }
    }
    logMessage(logFile,
               sFunctionName,
               serverName +
               QString(" Client connected: %1")
               .arg(pClient->peerAddress().toString()));
    connections.append(pClient);

    connect(pClient, SIGNAL(textMessageReceived(QString)),
            this, SLOT(onProcessTextMessage(QString)));
    connect(pClient, SIGNAL(binaryMessageReceived(QByteArray)),
            this, SLOT(onProcessBinaryMessage(QByteArray)));
    connect(pClient, SIGNAL(disconnected()),
            this, SLOT(onClientDisconnected()));
    connect(pClient, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onClientSocketError(QAbstractSocket::SocketError)));
}


void
FileServer::onClientSocketError(QAbstractSocket::SocketError error) {
    QString sFunctionName = " FileServer::onClientSocketError ";
    Q_UNUSED(error)
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    logMessage(logFile,
               sFunctionName,
               serverName +
               QString(" %1 Error %2 %3")
               .arg(pClient->peerAddress().toString())
               .arg(error)
               .arg(pClient->errorString()));
    if(!disconnect(pClient, 0, 0, 0)) {
        logMessage(logFile,
                   sFunctionName,
                   serverName +
                   QString(" Unable to disconnect signals from WebSocket"));
    }
    pClient->abort();
    if(!connections.removeOne(pClient)) {
        logMessage(logFile,
                   sFunctionName,
                   serverName +
                   QString(" Unable to remove %1 from list !")
                   .arg(pClient->peerAddress().toString()));
    }
    delete pClient;
}


void
FileServer::onProcessTextMessage(QString sMessage) {
    QString sFunctionName = " FileServer::onProcessTextMessage ";
    QString sNoData = QString("NoData");
    QString sToken;

    // The pointer is valid only during the execution of the slot
    // that calls this function from this object's thread context.
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    sToken = XML_Parse(sMessage, "get");
    if(sToken != sNoData) {
        QStringList argumentList = sToken.split(",");
        if(argumentList.count() < 3) {
            logMessage(logFile,
                       sFunctionName,
                       QString("Bad formatted requests: %1")
                       .arg(sToken));
            return;
        }
        QString sFileName = argumentList.at(0);
        qint64 startPos = argumentList.at(1).toInt();
        qint64 length   = argumentList.at(2).toInt();
#ifdef LOG_VERBOSE
        logMessage(logFile,
                   sFunctionName,
                   QString("%1 asked for: %2 %3 bytes, starting form %4 ")
                   .arg(pClient->peerAddress().toString())
                   .arg(sFileName)
                   .arg(length)
                   .arg(startPos));
#endif
        QFile file;
        QString sFilePath = sFileDir + sFileName;
        file.setFileName(sFilePath);
        if(file.exists()) {
            qint64 filesize = file.size();
            if(filesize <= startPos) {
                logMessage(logFile,
                           sFunctionName,
                           QString("File size %1 is less than requested start position: %2")
                           .arg(filesize)
                           .arg(startPos));
                return;
            }
            QByteArray ba;
            if(startPos == 0) {
                ba.append(sFileName);
                ba.append(QString(",%1").arg(filesize));
                ba.append(QString(1024-ba.length(), '\0'));
            }
            if(file.open(QIODevice::ReadOnly)) {
                if(file.seek(startPos)) {
                    ba.append(file.read(length));
                    if(ba.count() == 1024) {// Read error !
                        file.close();
                        logMessage(logFile,
                                   sFunctionName,
                                   QString("Error reading %1")
                                   .arg(sFileName));
                        return;
                    }
                    if(pClient->isValid()) {
                        int bytesSent = pClient->sendBinaryMessage(ba);
                        if(bytesSent != ba.count()) {
                            logMessage(logFile,
                                       sFunctionName,
                                       QString("Unable to send the file %1")
                                       .arg(sFileName));
                        }
#ifdef LOG_VERBOSE
                        else {
                            logMessage(logFile,
                                       sFunctionName,
                                       QString("File %1 correctly sent")
                                       .arg(sFileName));
                        }
#endif
                    }
                    else { // Client disconnected
                        file.close();
                        logMessage(logFile,
                                   sFunctionName,
                                   QString("Client disconnected while sending %1")
                                   .arg(sFileName));
                        return;
                    }
                    return;
                }
                else {// Unsuccesful seek
                    file.close();
                    logMessage(logFile,
                               sFunctionName,
                               QString("Error seeking %1 to %2")
                               .arg(sFileName)
                               .arg(startPos));
                    return;
                }
            }
            else {// file.open failed !
                logMessage(logFile,
                           sFunctionName,
                           QString("Unable to open the file %1")
                           .arg(sFileName));
                return;
            }
        }
        else {
            sMessage = QString("<missingFile>%1</missingFile>").arg(sToken);
            if(pClient->isValid()) {
                pClient->sendTextMessage(sMessage);
            }
            logMessage(logFile,
                       sFunctionName,
                       QString("Missing File: %1")
                       .arg(sFileName));
        }
    }

    sToken = XML_Parse(sMessage, "send_file_list");
    if(sToken != sNoData) {
        if(fileList.isEmpty())
            return;
        if(pClient->isValid()) {
            sMessage = QString("<file_list>");
            for(int i=0; i<fileList.count()-1; i++) {
                sMessage += fileList.at(i).fileName();
                sMessage += QString(";%1,").arg(fileList.at(i).size());
            }
            int i = fileList.count()-1;
            sMessage += fileList.at(i).fileName();
            sMessage += QString(";%1</file_list>").arg(fileList.at(i).size());
            SendToOne(pClient, sMessage);
        }
    }// send_spot_list
}


void
FileServer::senderThreadFinished() {
    QString sFunctionName = " FileServer::senderThreadFinished ";
    QThread *pThread = qobject_cast<QThread *>(sender());
    logMessage(logFile,
               sFunctionName,
               serverName +
               QString(" Sender Thread terminated"));
    senderThreads.removeOne(pThread);
    delete pThread;
}


void
FileServer::onFileTransferDone(bool bSuccess) {
    QString sFunctionName = " FileServer::onFileTransferDone ";
    logMessage(logFile,
               sFunctionName,
               serverName +
               QString(" File Transfer terminated with code %1")
               .arg(bSuccess));
}


int
FileServer::SendToOne(QWebSocket* pClient, QString sMessage) {
    QString sFunctionName = " FileServer::SendToOne ";
    if (pClient->isValid()) {
        qint64 written = pClient->sendTextMessage(sMessage);
        if(written != sMessage.length()) {
            logMessage(logFile,
                       serverName +
                       sFunctionName,
                       QString(" Error writing %1").arg(sMessage));
        }
#ifdef LOG_VERBOSE
        else {
            logMessage(logFile,
                       sFunctionName,
                       serverName +
                       QString(" Sent %1 to: %2")
                       .arg(sMessage)
                       .arg(pClient->peerAddress().toString()));
        }
#endif
    }
    else {
        logMessage(logFile,
                   sFunctionName,
                   serverName +
                   QString(" Client socket is invalid !"));
    }
    return 0;
}


void
FileServer::onProcessBinaryMessage(QByteArray message) {
    QString sFunctionName = " FileServer::onProcessBinaryMessage ";
    Q_UNUSED(message)
    logMessage(logFile,
               sFunctionName,
               QString("Unexpected binary message received !"));
}


void
FileServer::onClientDisconnected() {
    QString sFunctionName = " FileServer::onClientDisconnected ";
    QWebSocket* pClient = qobject_cast<QWebSocket *>(sender());
    QString sDiconnectedAddress = pClient->peerAddress().toString();
    logMessage(logFile,
               sFunctionName,
               serverName +
               QString(" %1 disconnected because %2. Close code: %3")
               .arg(sDiconnectedAddress)
               .arg(pClient->closeReason())
               .arg(pClient->closeCode()));
    if(!connections.removeOne(pClient)) {
        logMessage(logFile,
                   sFunctionName,
                   serverName +
                   QString(" Unable to remove %1 from list !")
                   .arg(sDiconnectedAddress));
    }
}


void
FileServer::onCloseServer() {
    QString sFunctionName = " FileServer::onCloseServer ";
    Q_UNUSED(sFunctionName)
    for(int i=0; i<connections.count(); i++) {
        disconnect(connections.at(i), 0, 0, 0);
        connections.at(i)->close();
    }
    for(int i=0; i<senderThreads.count(); i++) {
        senderThreads.at(i)->requestInterruption();
        if(senderThreads.at(i)->wait(3000)) {
            logMessage(logFile,
                       sFunctionName,
                       serverName +
                       QString(" File Server Thread %1 regularly closed")
                       .arg(i));
        }
        else {
            logMessage(logFile,
                       sFunctionName,
                       serverName +
                       QString(" File Server Thread %1 forced to close")
                       .arg(i));
            senderThreads.at(i)->terminate();
        }
    }
    NetServer::closeServer();
}
