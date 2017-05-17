#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QMap>
#include <QDebug>

class HttpServer: public QObject
{
    Q_OBJECT

    QTcpServer *server = 0;

    bool listenError = false;
    QString m_port;

public:
    explicit HttpServer(QObject *parent = 0)
        : QObject(parent)
    {}

    Q_INVOKABLE QString port() {
        return m_port;
    }

    bool isOk() const
    {
        return !listenError;
    }

public slots:
    // starts server
    void start() {
        if (server) {
            stop();
        }

        server = new QTcpServer(this);

        /// IMPORTANT!
        quint16 port = 8979;

        connect(server, &QTcpServer::newConnection, this, &HttpServer::onConnect);
        if (!server->listen(QHostAddress::LocalHost, port)) {
            emit error();
        }

        m_port = QString::number(port);

        qDebug() << "listening port" << m_port;
    }

    // destroys server
    void stop() {
        if (server) {
            qDebug() << "Stopping server";
            server->deleteLater();
            server = 0;
        }
    }

    void onConnect() {
        qDebug()<<"Connected";
        QTcpSocket *socket = server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, &HttpServer::onRead);
    }

    void onRead() {
        qDebug() << "Reading request...";
        QTcpSocket *socket = (QTcpSocket*) this->sender();
        socket->connect(socket, &QTcpSocket::disconnected, &QObject::deleteLater);

        /// Read data
        QString code;
        QStringList tokens = QString(socket->readAll()).split(QRegExp("[ \r\n][ \r\n]*"));
        if (tokens[0] == "GET") {
            if (tokens.length() >= 1) {
                QString params = tokens[1];

                //params to map
                QMap<QString,QString> map;
                params = params.mid(params.indexOf("?")+1);

                foreach (const QString & s, params.split("&")) {
                    QStringList pair = s.split("=");
                    if (pair.length() == 2){
                        map.insert(pair[0], pair[1]);
                    }
                }

                if (map.contains("access_token")) {
                    //Code found
                    qDebug() << "Found code!";
                    code = map["access_token"];
                }
            }
        }

        // Respond with 200
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);

        // http payload message body
        QByteArray content;
        if (code.isEmpty()) {
            content = "<!DOCTYPE html><html><script>"
                      "var uri = '' + window.location.href;"
                      "window.location.href = uri.replace('#','?');"
                      "</script><body></body></html>";
        }

        else {
            content = "<!DOCTYPE html><html>"
                      "<body><h1>Success!</h1></body></html>";
        }

        QString response = "HTTP/1.1 200 OK\n";
        response += "Content-Type: text/html; charset=utf-8\n";
        response += "Connection: Closed\n";
        response += "Content-Length: " + QString::number(content.length()) + "\n";
        response += "\n" + content;

        out << response.toUtf8();

        socket->write(block);
        socket->waitForBytesWritten();
        socket->disconnectFromHost();

        // Check if we have the api code ready
        if (!code.isEmpty()) {
            qDebug() << "Got code" << code;
            emit codeReceived(code);

            // Spin down server
            stop();
        }
    }

signals:
    void codeReceived(QString code);
    void error();
};

#endif // HTTPSERVER_H
