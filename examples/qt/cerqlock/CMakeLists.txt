#
# cmake configuration for CerQall Qt clock example
#
#  Copyright (c) 2018, Arthur Wisz
#  All rights reserved.
#
# See the LICENSE file for the license terms and conditions.
#

add_executable(qlockservice qlockservice.cpp qlockapplication.cpp)
target_link_libraries(qlockservice Qt5::Network Qt5::Core ${CMAKE_THREAD_LIBS_INIT})

add_executable(qlockclient qlockclient.cpp qlockapplication.cpp)
target_link_libraries(qlockclient Qt5::Core Qt5::Network ${CMAKE_THREAD_LIBS_INIT})
