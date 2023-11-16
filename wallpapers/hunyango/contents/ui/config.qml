/*
 *   SPDX-FileCopyrightText: 2023 Oliver Beard <olib141@outlook.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kirigami 2.20 as Kirigami

Kirigami.FormLayout {
    id: root
    twinFormLayouts: parentLayout

    property alias cfg_SlowTransition: slowTransitionBox.checked

    QQC2.CheckBox {
        id: slowTransitionBox
        Kirigami.FormData.label: i18n("Slow transition:")
    }
}
