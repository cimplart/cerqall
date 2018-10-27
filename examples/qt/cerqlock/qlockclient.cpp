/*!
 * \file
 * \brief     CerQall example - Qt clock client implementation
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#include <QtCore>
#include <QHostAddress>
#include "debug.h"
#include "qlockclient.h"
#include "cercall/qt/tcptransport.h"
#include "qlockapplication.h"

static ClockAlarmId stopAlarm = 0;

using cercall::log;
using cercall::debug;
using cercall::error;

struct QlockListener : public QlockClient::ServiceListener
{
    QlockListener(std::shared_ptr<QlockClient>& cc) : myClient(cc) {}
    void on_service_event(std::unique_ptr<QlockClient::EventType> event) override
    {
        const QlockAlarmEvent* clockEvent = event->get_as<QlockAlarmEvent>();
        if (clockEvent != nullptr) {
            log<debug>(O_LOG_TOKEN, "received alarm event %s", clockEvent->myTag.toStdString().c_str());
            if (clockEvent->myAlarmId == stopAlarm) {
                myClient->close_service([](const cercall::Result<int>& result) {
                    if (!result) {
                        log<error>(O_LOG_TOKEN, "close_service error - %s",
                                   result.error().message().c_str());
                    }
                    if (result.get_value() != 0) {
                        log<error>(O_LOG_TOKEN, "close_service result: %d", result.get_value());
                    }
                    log<debug>(O_LOG_TOKEN, "finish client program now");
                    QTimer::singleShot(10u, [](){ QCoreApplication::instance()->quit(); });
                });
            }
        } else {
            const QlockTickEvent* clockTickEvent = event->get_as<QlockTickEvent>();
            if (clockTickEvent != nullptr) {
                log<debug>(O_LOG_TOKEN, "tick time: %s",
                           clockTickEvent->myTickTime.toString().toStdString().c_str());
            }
        }
    }
private:
    std::shared_ptr<QlockClient> myClient;
};

void get_time(std::shared_ptr<QlockClient>& cc)
{
    QTimer::singleShot(10, [cc]() {
        cc->get_time([](const cercall::Result<QTime>& res){
            if ( !res) {
                log<error>(O_LOG_TOKEN, "error - %s", res.error().message().c_str());
                throw std::runtime_error(res.error().message());
            } else {
                log<debug>(O_LOG_TOKEN, "Current time: %s", res.get_value().toString().toStdString().c_str());
            }
        });
    });
}

void set_stop_alarm(std::shared_ptr<QlockClient>& cc, QTime after)
{
    QTimer::singleShot(10, [cc, after]() {
        log<debug>(O_LOG_TOKEN, "call set_alarm");
        cc->set_alarm("stopClient", after, [after](const cercall::Result<ClockAlarmId>& result){
            if (result.error()) {
                log<error>(O_LOG_TOKEN, "set_alarm failed: %s", result.error().message().c_str());
                throw std::runtime_error(result.error().message());
            } else {
                log<debug>(O_LOG_TOKEN, "stop alarm set in %s", after.toString().toStdString().c_str());
                stopAlarm = result.get_value();
            }
        });
    });
}

void set_clock_tick(std::shared_ptr<QlockClient>& cc, std::chrono::milliseconds interval)
{
    QTimer::singleShot(10, [cc, interval]() {
        log<debug>(O_LOG_TOKEN, "call set_tick_interval");
        cc->set_tick_interval(interval, [](const cercall::Result<void>& result){
            if ( result.error() ) {
                log<error>(O_LOG_TOKEN, "set_clock_tick failed: %s", result.error().message().c_str());
                throw std::runtime_error ( result.error().message() );
            }
        });
    });
}

int main(int ac, char **av)
{
    int res;

    cercall_user_log::programName = "qlockclient";

    try {
        QlockApplication app(ac, av);

        auto clientTransport = cercall::make_unique<cercall::qt::TcpTransport>(QHostAddress::LocalHost, 4321);
        auto client = std::make_shared<QlockClient>(std::move(clientTransport));

        if( !client->open()) {
            log<error>(O_LOG_TOKEN, "Could not connect to clock server");
            return 1;
        }

        QlockListener clockListener(client);

        client->add_listener(clockListener);

        while ( !client->is_open()) {
            app.processEvents();
        }

        log<debug>(O_LOG_TOKEN, "client is open");

        get_time(client);

        set_stop_alarm(client, QTime(0, 0, 16, 0));

        set_clock_tick(client, std::chrono::milliseconds(2000));

        res = app.exec();
    } catch (std::exception& e) {
        log<error>(O_LOG_TOKEN, "Exception: %s", e.what());
    }
    return res;
}
