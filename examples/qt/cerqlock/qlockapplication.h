/*!
 * \file
 * \brief     CerQall example - QT clock application class
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#ifndef CERQALL_QLOCKAPPLICATION_H
#define CERQALL_QLOCKAPPLICATION_H

#include <QCoreApplication>

/**
 * This class is needed to handle thrown exceptions, which Qt does not allow.
 */
class QlockApplication : public QCoreApplication
{
public:
    QlockApplication(const QlockApplication& other) = delete;
    QlockApplication& operator=(const QlockApplication& other) = delete;

    QlockApplication(int& argc, char** argv);

    bool notify(QObject* , QEvent* ) override;
};

#endif // CERQALL_QLOCKAPPLICATION_H
