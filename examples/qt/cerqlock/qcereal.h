/*!
 * \file
 * \brief     CerQall example - cereal functions for Qt
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#ifndef CERCALL_QCEREAL_H
#define CERCALL_QCEREAL_H

#include <QTime>
#include <QString>
#include <cereal/types/string.hpp>

namespace cereal {

template<class Archive>
void save(Archive& ar, const QString& s)
{
    ar(s.toStdString());
}

template<class Archive>
void load(Archive& ar, QString& s)
{
    std::string stds;
    ar(stds);
    s = QString::fromStdString(stds);
}

template<class Archive>
void save(Archive& archive, const QTime& t)
{
    QString tStr = t.toString();
    archive(tStr);
}

template<class Archive>
void load(Archive& archive, QTime& t)
{
    QString tStr;
    archive(tStr);
    t = QTime::fromString(tStr);
}

}   //namespace cereal

#endif //CERCALL_QCEREAL_H
