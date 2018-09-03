/*!
 * \file
 * \brief     CerQall example - QT clock service class
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#ifndef CERQALL_QLOCKSERVICE_H
#define CERQALL_QLOCKSERVICE_H

#include <QList>
#include "qlockinterface.h"
#include "cercall/service.h"
#include "cereal_setup.h"

class QlockService : public cercall::Service<QlockInterface, QlockSerialization>,
                     public std::enable_shared_from_this<QlockService>
{
public:
    QlockService(std::unique_ptr<cercall::Acceptor> ac);

    void get_time(Closure<QTime> closure) override;

    void set_tick_interval(std::chrono::milliseconds tickInterval, Closure<void> closure) override;

    void set_alarm(QString tag, QTime after, Closure<ClockAlarmId> closure) override;

    void cancel_alarm(ClockAlarmId alarm, Closure<void> closure) override;

    void on_connection_error(cercall::Transport&, const cercall::Error&) override;

    void close_service(cercall::Closure<int> closure) override;

private:
    struct Alarm
    {
        QTimer myTimer;
        QString myTag;
        ClockAlarmId myId;
        std::function<void()> myAlarmAction;
        explicit Alarm(ClockAlarmId id, std::chrono::milliseconds interval, QString tag)
        : myTimer(), myTag(tag), myId(id)
        {
            myTimer.singleShot(interval, [this] () { timeout(); });
        }

        void set_action(std::function<void()> action)
        {
            myAlarmAction = action;
        }

        void timeout()
        {
            myAlarmAction();
        }
    };

    std::list<Alarm> myAlarms;

    QTimer myTickTimer;

    std::chrono::milliseconds myTickInterval { 0 };

    std::list<Alarm>::iterator find_alarm(ClockAlarmId alarmId);

    void tickTimer();
};

#endif //CERQALL_QLOCKSERVICE_H

