# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

find_package(Threads REQUIRED)
include(ECMAddTests)

ecm_add_test(spellcheckerrunnertest.cpp TEST_NAME spellcheckerrunnertest
    LINK_LIBRARIES
    Qt::Test
    krunner_spellcheck_static
)
