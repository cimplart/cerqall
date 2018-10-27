/*!
 * \file
 * \brief     CerQall example - Cereal setup for Qt clock comms
 *
 *  Copyright (c) 2018, Arthur Wisz
 *  All rights reserved.
 *
 * See the LICENSE file for the license terms and conditions.
 */

#ifndef CERCALL_CERQLOCK_CEREAL_SETUP_H
#define CERCALL_CERQLOCK_CEREAL_SETUP_H

#include "cercall/cereal/binary.h"
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/chrono.hpp>
#include "qcereal.h"
#include "qlockinterface.h"

CEREAL_REGISTER_TYPE(QlockAlarmEvent);

CEREAL_REGISTER_TYPE(QlockTickEvent);

CEREAL_REGISTER_POLYMORPHIC_RELATION(QlockEvent, QlockAlarmEvent);
CEREAL_REGISTER_POLYMORPHIC_RELATION(QlockEvent, QlockTickEvent);

using QlockSerialization = cercall::cereal::Binary;

#endif //CERCALL_CERQLOCK_CEREAL_SETUP_H
