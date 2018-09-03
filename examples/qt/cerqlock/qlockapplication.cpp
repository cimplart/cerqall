/*!
 * \file
 * \brief     CerQall example - QT clock application class
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 *
 */

#include "qlockapplication.h"
#include <iostream>

QlockApplication::QlockApplication(int& argc, char** argv)
: QCoreApplication(argc, argv)
{
}

bool QlockApplication::notify(QObject* receiver, QEvent* event)
{
    bool done = true;
    try {
        done = QCoreApplication::notify(receiver, event);
    } catch (const std::exception& ex) {
        std::cerr << "Caught exception: " << ex.what() << '\n';
        QCoreApplication::exit(1);
    } catch (...) {
        std::cerr << "Caught unknown exception\n";
        QCoreApplication::exit(1);
    }
    return done;
}
