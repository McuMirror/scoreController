#ifndef FILESERVER_H
#define FILESERVER_H

#include <QObject>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfoList>

#include "netServer.h"

QT_FORWARD_DECLARE_CLASS(QFile)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class FileServer : public NetServer
{
    Q_OBJECT
public:
    explicit FileServer(QFile *_logFile = Q_NULLPTR, QObject *parent = 0);
    void setServerPort(quint16 _port);
    bool setDirs(QString _sSlideDir, QString _sSpotDir);

private:
    int SendToOne(QWebSocket* pSocket, QString sMessage);

protected:
    QString XML_Parse(QString input_string, QString token);

signals:
    void fileServerDone(bool);
    void goTransfer();
    void serverAddress(QString);

public slots:
    void startServer();
    void senderThreadFinished();
    void onFileTransferDone(bool bSuccess);

private slots:
    void onNewConnection(QWebSocket *pClient);
    void onClientDisconnected();
    void onProcessTextMessage(QString sMessage);
    void onProcessBinaryMessage(QByteArray message);
    void onClientSocketError(QAbstractSocket::SocketError error);
    void onFileServerError(QWebSocketProtocol::CloseCode);

private:
    QString       sNoData;
    quint16       port;
    QString       sSlideDir;
    QStringList   slideList;
    QString       sSpotDir;
    QFileInfoList spotList;

    QVector<QWebSocket*> connections;

    QList<QThread*> senderThreads;
};

#endif // FILESERVER_H
