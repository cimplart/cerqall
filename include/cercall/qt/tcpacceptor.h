/*!
 * \file
 * \brief     CerQall TCP Acceptor class for Qt TCP sockets
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#ifndef CERCALL_QT_TCPACCEPTOR_H
#define CERCALL_QT_TCPACCEPTOR_H

#include <QTcpServer>
#include "cercall/acceptor.h"
#include "cercall/qt/tcptransport.h"

namespace cercall {
namespace qt {

class TcpAcceptor : public cercall::Acceptor
{
public:

    TcpAcceptor(const QHostAddress &address = QHostAddress::Any, quint16 port = 0)
        : myHostAddr { address }, myPort { port }, myServer {}
    {
        QObject::connect(&myServer, &QTcpServer::newConnection, [this]() {
            notify_new_connection();
        });
        QObject::connect(&myServer, &QTcpServer::acceptError, [this](QAbstractSocket::SocketError e) {
            if (myListener != nullptr) {
                Error err { e, myServer.errorString().toStdString() };
                myListener->on_accept_error(err);
            }
        });
    }

    bool is_open() const override
    {
        return myServer.isListening();
    }

    void open(int maxPendingClientConnections = -1) override
    {
        if (myListener == nullptr) {
            throw std::logic_error("cercall::qt::TcpAcceptor::open(): listener is NULL");
        }
        if ( !is_open()) {
            //On Unix the SO_REUSEADDR is set by default by QTcpServer.
            if (maxPendingClientConnections > 0) {
                myServer.setMaxPendingConnections(maxPendingClientConnections);
            }
            if (!myServer.listen(myHostAddr, myPort)) {
                Error err { myServer.serverError(), myServer.errorString().toStdString() };
                myListener->on_accept_error(err);
                return;
            }
        }
    }

    void close() override
    {
        if (is_open()) {
            myServer.close();
        }
    }

private:
    QHostAddress myHostAddr;
    quint16 myPort;
    QTcpServer myServer;

    void notify_new_connection()
    {
        if (myListener == nullptr) {
            throw std::logic_error("cercall::qt::TcpAcceptor::notify_new_connection(): listener is NULL");
        }
        QTcpSocket* newClientSock = myServer.nextPendingConnection();
        if (newClientSock != nullptr) {
            newClientSock->setParent(nullptr);  //cercall::Service class manages its transport objects.
            myListener->on_client_accepted(std::make_shared<TcpTransport>(newClientSock));
        } else {
            //Silently ignore ?
        }
    }
};


}   //namespace qt
}   //namespace cercall

#endif // CERCALL_QT_TCPACCEPTOR_H


