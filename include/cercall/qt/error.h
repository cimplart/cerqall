/*!
 * \file
 * \brief     QT-dependent functions of cercall::Error
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#ifndef CERQALL_QT_ERROR_H
#define CERQALL_QT_ERROR_H

#include <QAbstractSocket>
#include "cercall/details/cpputil.h"
#include "cercall/error.h"

namespace cercall {

const Error& Error::operation_in_progress()
{
    static std::unique_ptr<Error> err = cercall::make_unique<Error>(QAbstractSocket::UnfinishedSocketOperationError,
                                                                    "Operation in progress");
    return *err;
}

}   //namespace cercall

#endif // CERQALL_QT_ERROR_H
