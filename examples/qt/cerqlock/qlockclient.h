/*!
 * \file
 * \brief     CerQall example - Qt clock client class
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#ifndef CERQALL_QLOCKCLIENT_H
#define CERQALL_QLOCKCLIENT_H

#include "qlockinterface.h"
#include "cercall/client.h"
#include "cereal_setup.h"

class QlockClient : public cercall::Client<QlockInterface, QlockSerialization>
{
public:
    QlockClient(std::unique_ptr<cercall::Transport> tr)
        : cercall::Client<QlockInterface, QlockSerialization>(std::move(tr)) {}

    void get_time(Closure<QTime> closure) override
    {
        send_call(__func__, closure);
    }

    void set_tick_interval(std::chrono::milliseconds tickInterval, Closure<void> closure) override
    {
        send_call(__func__, closure, tickInterval);
    }

    void set_alarm(QString tag, QTime after, Closure<ClockAlarmId> closure) override
    {
        send_call(__func__, closure, tag, after);
    }

    void cancel_alarm(ClockAlarmId alarm, Closure<void> closure) override
    {
        send_call(__func__, closure, alarm);
    }

    void close_service(cercall::Closure<int> closure) override
    {
        send_call(__func__, closure);
    }
};

#endif // CERQALL_QLOCKCLIENT_H

