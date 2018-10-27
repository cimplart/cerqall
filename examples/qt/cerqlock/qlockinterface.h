/*!
 * \file
 * \brief     CerQall example - Qt Clock service interface
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 *  See the LICENSE file for the license terms and conditions.
 */

#ifndef CERCALL_QLOCKINTERFACE_H
#define CERCALL_QLOCKINTERFACE_H

#include <QEvent>
#include <QString>
#include <QTime>
#include <chrono>
#include "cercall/cercall.h"

using ClockAlarmId = qint32;

enum EventNames {
    ClockAlarmEvent = QEvent::User + 100U,
    ClockTickEvent = QEvent::User + 101U
};

class QlockEvent : public QEvent
{
public:
    QlockEvent() : QEvent(QEvent::None)
    {
    }

    QlockEvent(QEvent::Type et) : QEvent(et)
    {
    }

    //QEvent already has a virtual destructor.

    template<class DerivedEvent>
    const DerivedEvent* get_as() const
    {
        return dynamic_cast<const DerivedEvent*>(this);
    }

    template<class A>
    void serialize(A& ar)
    {
        ar(t);
    }
};

O_REGISTER_TYPE(QlockAlarmEvent);

class QlockAlarmEvent : public QlockEvent
{
    static QEvent::Type myEventType;
public:
    QlockAlarmEvent() : QlockEvent(myEventType) {}
    QlockAlarmEvent(ClockAlarmId alarm, const QString &tag) : QlockEvent(myEventType), myAlarmId(alarm), myTag(tag) {}

    ClockAlarmId myAlarmId;
    QString myTag;

    template<class A>
    void serialize(A& ar)
    {
        ar(myTag, myAlarmId);
    }
};

QEvent::Type QlockAlarmEvent::myEventType
    = static_cast<QEvent::Type>(QEvent::registerEventType(QEvent::User + 100U));


O_REGISTER_TYPE(QlockTickEvent);

class QlockTickEvent : public QlockEvent
{
    static QEvent::Type myEventType;
public:
    QlockTickEvent() : QlockEvent(myEventType) {}
    QlockTickEvent(const QTime& time) : QlockEvent(myEventType), myTickTime(time) {}

    QTime myTickTime;

    template<class A>
    void serialize(A& ar)
    {
        ar(myTickTime);
    }
};

QEvent::Type QlockTickEvent::myEventType
    = static_cast<QEvent::Type>(QEvent::registerEventType(QEvent::User + 101U));

O_REGISTER_TYPE(QlockInterface);

class QlockInterface
{
public:

    template<typename T>
    using Closure = typename cercall::Closure<T>;

    //QEvent can't be the EventType because it has no default (parameterless) constructor.
    using EventType = QlockEvent;

    virtual void get_time(Closure<QTime> closure) = 0;

    virtual void set_tick_interval(std::chrono::milliseconds tickInterval, Closure<void> closure) = 0;

    virtual void set_alarm(QString tag, QTime after, Closure<ClockAlarmId> closure) = 0;

    virtual void cancel_alarm(ClockAlarmId alarm, Closure<void> closure) = 0;

    virtual void close_service(cercall::Closure<int> closure) = 0;
};

#endif //CERCALL_QLOCKINTERFACE_H
