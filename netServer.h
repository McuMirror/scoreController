#ifndef NETSERVER_H
#define NETSERVER_H

#include <QObject>
#include <QUrl>
#include <QWebSocketServer>
#include <QFile>

class NetServer : public QObject
{
    Q_OBJECT
public:
    explicit NetServer(QString _serverName, QFile* _logFile= NULL, QObject *parent = 0);
    int prepareServer(quint16 serverPort);

signals:
    void newConnection(QWebSocket *);
    void netServerError(QWebSocketProtocol::CloseCode);

private slots:
    void onNewServerConnection();
    void onServerError(QWebSocketProtocol::CloseCode closeCode);

public slots:

protected:
    void logMessage(QString sFunctionName, QString sMessage);

protected:
    QString           sServerName;
    QFile            *logFile;
    QWebSocketServer *pServerSocket;

private:
    QTextStream       sDebugInformation;
    QString           sDebugMessage;
    QDateTime         dateTime;
};

#endif // NETSERVER_H
