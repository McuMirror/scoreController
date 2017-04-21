#include "netServer.h"
#include "fileserver.h"
#include <QFile>
#include <QDir>
#include <QThread>
#include <QWebSocket>
#include <QImage>
#include <QBuffer>


#define LOG_MESG


FileServer::FileServer(QFile* _logFile, QObject *parent)
    : NetServer(QString("FileServer"), _logFile, parent)
{
    sNoData   = QString();
    port      = 0;
    sSlideDir = QString();
    sSpotDir  = QString();
    slideList = QStringList();
    spotList  = QList<QFileInfo>();
    connections.clear();
}


void
FileServer::setServerPort(quint16 _port) {
    port = _port;
}


bool
FileServer::setDirs(QString _sSlideDir, QString _sSpotDir) {
    QString sFunctionName = " FileServer::setDirs ";
    sSlideDir = _sSlideDir;
    sSpotDir  = _sSpotDir;

    QDir slideDir(sSlideDir);
    if(slideDir.exists()) {
        QStringList filter(QStringList() << "*.jpg" << "*.jpeg" << "*.png");
        slideDir.setNameFilters(filter);
        slideList = slideDir.entryList();
    }
    logMessage(sFunctionName, QString("Found %1 slides").arg(slideList.count()));

    QDir spotDir(sSpotDir);
    if(spotDir.exists()) {
        QStringList nameFilter(QStringList() << "*.mp4");
        spotDir.setNameFilters(nameFilter);
        spotDir.setFilter(QDir::Files);
        spotList = spotDir.entryInfoList();
    }
    logMessage(sFunctionName, QString("Found %1 spots").arg(spotList.count()));

    return true;
}


void
FileServer::startServer() {
    QString sFunctionName = " FileServer::startServer ";
    Q_UNUSED(sFunctionName)

    if(port == 0) {
        logMessage(sFunctionName, QString("Error! Server port not set."));
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
    for(int i=0; i<connections.count(); i++) {
        delete connections.at(i);
    }
    emit fileServerDone(true);// Close File Server with errors !
}


void
FileServer::onNewConnection(QWebSocket *pClient) {
    QString sFunctionName = " FileServer::onNewConnection ";
    int nConnections = connections.count();
    for(int i=0; i<nConnections; i++) {
        if(connections.at(i)->peerAddress() == pClient->peerAddress()) {
            logMessage(sFunctionName,
                       QString("%1 Duplicate requests from %2")
                       .arg(sServerName)
                       .arg(pClient->peerAddress().toString()));
            if(connections.at(i)->isValid() && pClient->isValid()) {
                logMessage(sFunctionName,
                           QString("Both sockets are valid! Removing the old connection"));
                connections.at(i)->close(QWebSocketProtocol::CloseCodeNormal, QString("Duplicated request"));
                connections.at(i)->deleteLater();
            }
            else {
                if(pClient->isValid()) {
                    logMessage(sFunctionName,
                               QString("Only present socket is valid. Removing the old one"));
                    connections.at(i)->close(QWebSocketProtocol::CloseCodeNormal, QString("Duplicated request"));
                    connections.at(i)->deleteLater();
                }
                else {
                    logMessage(sFunctionName,
                               QString("Present socket is not valid."));
                    pClient->close(QWebSocketProtocol::CloseCodeNormal, QString("Duplicated request"));
                    pClient->deleteLater();
                    return;
                }
            }
        }
    }
    logMessage(sFunctionName, QString("Client connected: %1").arg(pClient->peerAddress().toString()));
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
    logMessage(sFunctionName,
               QString("%1 Error %2 %3")
               .arg(pClient->peerAddress().toString())
               .arg(error)
               .arg(pClient->errorString()));
    if(!disconnect(pClient, 0, 0, 0)) {
        logMessage(sFunctionName, QString("Unable to disconnect signals from WebSocket"));
    }
    pClient->abort();
    if(!connections.removeOne(pClient)) {
        logMessage(sFunctionName,
                   QString("Unable to remove %1 from list !")
                   .arg(pClient->peerAddress().toString()));
    }
    pClient->deleteLater();
}


void
FileServer::onProcessTextMessage(QString sMessage) {
    QString sFunctionName = " FileServer::onProcessTextMessage ";
    QString sToken;

    // The pointer is valid only during the execution of the slot
    // that calls this function from this object's thread context.
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    sToken = XML_Parse(sMessage, "get");
    if(sToken != sNoData) {
        QStringList argumentList = sToken.split(",");
        if(argumentList.count() < 3) {
            logMessage(sFunctionName,
                       QString("Bad formatted requests: %1")
                       .arg(sToken));
            return;
        }
        QString sFileName = argumentList.at(0);
        qint64 startPos = argumentList.at(1).toInt();
        qint64 length   = argumentList.at(2).toInt();
        logMessage(sFunctionName,
                   QString("%1 asked for: %2 %3 bytes, starting form %4 ")
                   .arg(pClient->peerAddress().toString())
                   .arg(sFileName)
                   .arg(length)
                   .arg(startPos));
        QFile file;
        QString sFilePath = sSpotDir + sFileName;
        file.setFileName(sFilePath);
        if(!file.exists()) {
            sFilePath = sSlideDir + sFileName;
            file.setFileName(sFilePath);
        }
        if(file.exists()) {
            qint64 filesize = file.size();
            if(filesize <= startPos) {
                logMessage(sFunctionName,
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
                        logMessage(sFunctionName,
                                   QString("Error reading %1")
                                   .arg(sFileName));
                        return;
                    }
                    if(pClient->isValid()) {
                        int bytesSent = pClient->sendBinaryMessage(ba);
                        if(bytesSent != ba.count()) {
                            logMessage(sFunctionName,
                                       QString("Unable to send the file %1")
                                       .arg(sFileName));
                        }
                        else {
                            logMessage(sFunctionName,
                                       QString("File %1 correctly sent")
                                       .arg(sFileName));
                        }
                    }
                    else { // Client disconnected
                        file.close();
                        logMessage(sFunctionName,
                                   QString("Client disconnected while sending %1")
                                   .arg(sFileName));
                        return;
                    }
                    return;
                }
                else {// Unsuccesful seek
                    file.close();
                    logMessage(sFunctionName,
                               QString("Error seeking %1 to %2")
                               .arg(sFileName)
                               .arg(startPos));
                    return;
                }
            }
            else {// file.open failed !
                logMessage(sFunctionName,
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
            logMessage(sFunctionName,
                       QString("Missing File: %1")
                       .arg(sFileName));
        }
    }

    sToken = XML_Parse(sMessage, "send_spot_list");
    if(sToken != sNoData) {
        if(spotList.isEmpty())
            return;
        if(pClient->isValid()) {
            sMessage = QString("<spot_list>");
            for(int i=0; i<spotList.count()-1; i++) {
                sMessage += spotList.at(i).fileName();
                sMessage += QString(";%1,").arg(spotList.at(i).size());
            }
            int i = spotList.count()-1;
            sMessage += spotList.at(i).fileName();
            sMessage += QString(";%1</spot_list>").arg(spotList.at(i).size());
            SendToOne(pClient, sMessage);
        }
    }// send_spot_list
}


void
FileServer::senderThreadFinished() {
    QString sFunctionName = " FileServer::senderThreadFinished ";
    QThread *pThread = qobject_cast<QThread *>(sender());
    logMessage(sFunctionName,
               QString("Sender Thread terminated"));
    senderThreads.removeOne(pThread);
    pThread->deleteLater();
}


void
FileServer::onFileTransferDone(bool bSuccess) {
    QString sFunctionName = " FileServer::onFileTransferDone ";
    logMessage(sFunctionName,
               QString("File Transfer terminated with code %1")
               .arg(bSuccess));
}


int
FileServer::SendToOne(QWebSocket* pClient, QString sMessage) {
    QString sFunctionName = " FileServer::SendToOne ";
    if (pClient->isValid()) {
        qint64 written = pClient->sendTextMessage(sMessage);
        if(written != sMessage.length()) {
            logMessage(sFunctionName, QString("Error writing %1").arg(sMessage));
        }
        else {
            logMessage(sFunctionName,
                       QString("Sent %1 to: %2")
                       .arg(sMessage)
                       .arg(pClient->peerAddress().toString()));
        }
    }
    else {
        logMessage(sFunctionName, QString("Client socket is invalid !"));
    }
    return 0;
}


void
FileServer::onProcessBinaryMessage(QByteArray message) {
    QString sFunctionName = " FileServer::onProcessBinaryMessage ";
    Q_UNUSED(message)
    logMessage(sFunctionName, QString("Unexpected binary message received !"));
}


void
FileServer::onClientDisconnected() {
    QString sFunctionName = " FileServer::onClientDisconnected ";
    QWebSocket* pClient = qobject_cast<QWebSocket *>(sender());
    QString sDiconnectedAddress = pClient->peerAddress().toString();
    logMessage(sFunctionName,
               QString("%1 disconnected because %2. Close code: %3")
               .arg(sDiconnectedAddress)
               .arg(pClient->closeReason())
               .arg(pClient->closeCode()));
    if(!connections.removeOne(pClient)) {
        logMessage(sFunctionName,
                   QString("Unable to remove %1 from list !")
                   .arg(sDiconnectedAddress));
    }
}


QString
FileServer::XML_Parse(QString input_string, QString token) {
    // simple XML parser
    //   xml_parse("<token>10</token>","token")   will return "10"
    //   returns "" on error
    QString start_token, end_token, result = sNoData;
    start_token = "<" + token + ">";
    end_token = "</" + token + ">";

    int start_pos=-1, end_pos=-1, len=0;

    start_pos = input_string.indexOf(start_token);
    end_pos   = input_string.indexOf(end_token);

    if(start_pos < 0 || end_pos < 0) {
        result = sNoData;
    } else {
        start_pos += start_token.length();
        len = end_pos - start_pos;
        if(len>0)
            result = input_string.mid(start_pos, len);
        else
            result = "";
    }

    return result;
}
