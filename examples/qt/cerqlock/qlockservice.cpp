/*!
 * \file
 * \brief     CerQall example - QT clock service implementation
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#include <QtCore>
#include <csignal>
#include <cassert>
#include "debug.h"
#include "cercall/qt/tcpacceptor.h"
#include "cercall/service.h"
#include "qlockservice.h"
#include "qlockapplication.h"

static ClockAlarmId nextAlarmId = 1U;

using cercall::log;
using cercall::debug;
using cercall::error;

QlockService::QlockService(std::unique_ptr<cercall::Acceptor> ac)
    : Service<QlockInterface, QlockSerialization>(std::move(ac)), myTickTimer()
{
    O_ADD_SERVICE_FUNCTIONS_OF(QlockInterface, false, get_time, set_tick_interval, set_alarm, cancel_alarm);
    O_ADD_SERVICE_FUNCTIONS_OF(QlockInterface, false, close_service);
    QObject::connect(&myTickTimer, &QTimer::timeout, [this] () { tickTimer(); });
}

void QlockService::get_time(cercall::Closure<QTime> closure)
{
    log<debug>(O_LOG_TOKEN, "");
    closure(QTime::currentTime());
}

void QlockService::tickTimer()
{
    broadcast_event<QlockTickEvent>(QTime::currentTime());
}

void QlockService::set_tick_interval(std::chrono::milliseconds tickInterval, cercall::Closure<void> closure)
{
    log<debug>(O_LOG_TOKEN, "");
    if (tickInterval != std::chrono::milliseconds::zero()) {
        myTickTimer.setInterval(tickInterval);
        myTickTimer.start();
    } else {
        myTickTimer.stop();
    }
    closure();
}

auto QlockService::find_alarm(ClockAlarmId alarmId) -> std::list<QlockService::Alarm>::iterator
{
    return std::find_if(myAlarms.begin(), myAlarms.end(), [alarmId](const Alarm& a) {
                                   return a.myId == alarmId;
                               });
}

void QlockService::set_alarm(QString tag, QTime after, cercall::Closure<ClockAlarmId> closure)
{
    log<debug>(O_LOG_TOKEN, " in %d seconds", QTime(0,0,0).secsTo(after));
    std::chrono::milliseconds interval = static_cast<std::chrono::milliseconds>(QTime(0,0,0).msecsTo(after));
    auto shared_this = shared_from_this();
    myAlarms.emplace(myAlarms.end(), nextAlarmId++, interval, tag);
    Alarm& theAlarm = myAlarms.back();
    theAlarm.set_action([shared_this, &theAlarm](){
        log<debug>(O_LOG_TOKEN, "alarm timer for %s", theAlarm.myTag.toStdString().c_str());
        shared_this->broadcast_event<QlockAlarmEvent>(theAlarm.myId, theAlarm.myTag);
        //Purge the alarm.
        auto it = shared_this->find_alarm(theAlarm.myId);
        assert(it != shared_this->myAlarms.end());
        shared_this->myAlarms.erase(it);
        log<debug>(O_LOG_TOKEN, "alarm purged");
    });
    closure(theAlarm.myId);
}

void QlockService::cancel_alarm(ClockAlarmId alarm, cercall::Closure<void> closure)
{
    auto it = find_alarm(alarm);
    if (it != myAlarms.end()) {
        it->myTimer.stop();
    }
    closure();
}

void QlockService::on_connection_error(cercall::Transport&, const cercall::Error& e)
{
    log<error>(O_LOG_TOKEN, "client connection error: ", e.message().c_str());
}

void QlockService::close_service(cercall::Closure<int> closure)
{
    QTimer::singleShot(100u, [](){ QCoreApplication::instance()->quit(); });
    closure(0);
}

int main(int argc, char *argv[])
{
    int res;

    cercall_user_log::programName = "qlockservice";

    try {
        QlockApplication app(argc, argv);

        auto sigHandler = [](int) { QCoreApplication::instance()->quit(); };
        signal(SIGTERM, sigHandler);
        signal(SIGINT, sigHandler);

        log<debug>(O_LOG_TOKEN, "Start qlock service");
        auto acceptor = cercall::make_unique<cercall::qt::TcpAcceptor>(QHostAddress::LocalHost, 4321);
        std::shared_ptr<QlockService> service = std::make_shared<QlockService>(std::move(acceptor));

        service->start();
        res = app.exec();
        log<debug>(O_LOG_TOKEN, "finished app loop");
        service->stop();
    } catch (const QException& e) {
        std::cerr << "QT exception: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Unknown exception\n";
    }
    log<debug>(O_LOG_TOKEN, "Exit qlock service");
    return res;
}
