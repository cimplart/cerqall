/*!
 * \file
 * \brief     CerQall TCP Transport for Qt socket
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#ifndef CERCALL_QT_TCPTRANSPORT_H
#define CERCALL_QT_TCPTRANSPORT_H

#include <QTcpSocket>
#include "cercall/transport.h"
#include "cercall/qt/error.h"
#include "cercall/log.h"

namespace cercall {
namespace qt {

class TcpTransport : public Transport
{
public:
    using SocketType = QTcpSocket*;

    /**
     * For use by the acceptor.
     */
    TcpTransport(QTcpSocket* s) : mySocket { s }
    {
        log<trace>(O_LOG_TOKEN, "socket param");
        o_assert(s != nullptr);
        s->setParent(nullptr);
        connect_signals();
    }

    /**
     * For client-side connections.
     */
    TcpTransport(const QHostAddress &hostAddr, quint16 port) : mySocket(nullptr), myHostAddress(hostAddr), myPort(port)
    {
        log<trace>(O_LOG_TOKEN, "host,port params");
    }

    TcpTransport() = delete;
    TcpTransport(const TcpTransport&) = delete;
    TcpTransport& operator=(const TcpTransport&) = delete;
    virtual ~TcpTransport()
    {
        log<trace>(O_LOG_TOKEN, "");
        close();
    }

    bool is_open() override
    {
        return mySocket != nullptr && mySocket->state() == QTcpSocket::ConnectedState;
    }

    bool open() override
    {
        log<trace>(O_LOG_TOKEN, "");
        if ( !is_open()) {
            log<debug>(O_LOG_TOKEN, "new socket");
            mySocket = new QTcpSocket(nullptr);
            connect_signals();
            log<debug>(O_LOG_TOKEN, "connect to host %s:%d", myHostAddress.toString().toStdString().c_str(), myPort);
            mySocket->connectToHost(myHostAddress, myPort);
            return mySocket->waitForConnected(-1);
        } else {
            return false;
        }
    }

    void open(const cercall::Closure<bool>& cl) override
    {
        log<trace>(O_LOG_TOKEN, "");
        if ( !is_open()) {
            log<debug>(O_LOG_TOKEN, "new socket");
            mySocket = new QTcpSocket(nullptr);
            connect_signals();
            myOpenClosure = cl;
            log<debug>(O_LOG_TOKEN, "connect to host %s:%d", myHostAddress.toString().toStdString().c_str(), myPort);
            mySocket->connectToHost(myHostAddress, myPort);
        } else {
            Result<bool> result { false, Error { QAbstractSocket::UnknownSocketError, "Socket is already connected" } };
            cl(result);
        }
    }

    void close() override
    {
        log<trace>(O_LOG_TOKEN, "");
        if (mySocket != nullptr) {
            if (mySocket->state() == QTcpSocket::ConnectedState) {
                log<debug>(O_LOG_TOKEN, "disconnect from host");
                mySocket->disconnectFromHost();
            }
            mySocket->deleteLater();
        }
    }

    void read(uint32_t len) override
    {
        o_assert(len > 0);
        if ( is_open()) {
            mySocket->setReadBufferSize(len);
            myReadLength = len;
        } else {
            throw std::runtime_error("cercall::qt::TcpTransport: cannot read from a closed transport");
        }
    }

    const std::string& get_read_data() override
    {
        if (mySocket != nullptr && myReadLength > 0) {
            /* QIODevice has an internal buffer and offers no means to access its contents without copying data.
            * Hence a copy of the data (into myReadData) has to be made.
            */
            QByteArray data = mySocket->read(myReadLength);
            myReadData = std::string(data.constData(), data.length());
            myReadLength = 0u;
        } else {
            log<error>(O_LOG_TOKEN, "no data to read");
        }
        return myReadData;
    }

    Error write(const std::string& msg) override
    {
        Error result;   //no error by default
        if ( is_open()) {
            if (mySocket->write(msg.data(), msg.length()) < 0) {
                Error err { mySocket->error(), mySocket->errorString().toStdString() };
                result = err;
            }
        } else {
            Error err { QAbstractSocket::UnknownSocketError, "Socket is not connected" };
            result = err;
        }
        if ( result) {
            log<error>(O_LOG_TOKEN, "write error - %s", result.message().c_str());
        }
        return result;
    }


private:

    QTcpSocket* mySocket;
    uint32_t myReadLength;
    std::string myReadData;

    QHostAddress myHostAddress;
    quint16 myPort;
    cercall::Closure<bool> myOpenClosure;

    void connect_signals()
    {
        QObject::connect(mySocket, &QTcpSocket::readyRead, [this]() { notify_incoming_data(); });
        QObject::connect(mySocket, &QTcpSocket::connected, [this]() { notify_connected(); });
        QObject::connect(mySocket, &QTcpSocket::disconnected, [this]() { notify_disconnected(); });
        QObject::connect(mySocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                                         [this](QAbstractSocket::SocketError e) { notify_error(e); });
    }

    void notify_connected()
    {
        if (mySocket != nullptr) {
            o_assert(myListener != nullptr);
            log<debug>(O_LOG_TOKEN, "tcp socket connected");
            myListener->on_connected(*this);
        }
    }

    void notify_disconnected()
    {
        if (mySocket != nullptr) {
            o_assert(myListener != nullptr);
            log<debug>(O_LOG_TOKEN, "tcp socket disconnected");
            myListener->on_disconnected(*this);
        }
    }

    void notify_incoming_data()
    {
        if (mySocket != nullptr && mySocket->bytesAvailable() >= myReadLength) {
            o_assert(myListener != nullptr);
            myListener->on_incoming_data(*this, mySocket->bytesAvailable());
        }
    }

    void notify_error(QAbstractSocket::SocketError e)
    {
        if (mySocket != nullptr) {
            o_assert(myListener != nullptr);
            Error err { e, mySocket->errorString().toStdString() };
            log<error>(O_LOG_TOKEN, "tcp socket error - %s", err.message().c_str());
            myListener->on_connection_error(*this, err);
            if (myOpenClosure) {
                myOpenClosure(Result<bool> { false, err });
                myOpenClosure = nullptr;
            }
        }
    }
};

}   //namespace qt
}   //namespace cercall

#endif //CERCALL_QT_TCPTRANSPORT_H
