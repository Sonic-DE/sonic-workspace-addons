/*
 *   SPDX-FileCopyrightText: 2023 Oliver Beard <olib141@outlook.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import QtQuick.Controls 2.8 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

import org.kde.kirigami 2.20 as Kirigami

Kirigami.FormLayout {
    id: root
    twinFormLayouts: parentLayout

    property int cfg_Interval
    property alias formLayout: root

    QQC2.SpinBox {
        Kirigami.FormData.label: i18ndc("plasma_wallpaper_org.kde.hunyango", "@label:spinbox", "Color change interval:")

        from: 5
        to: 300
        editable: true
        textFromValue: value => i18np("%1 second", "%1 seconds", value)
        valueFromText: text => parseInt(text)
        value: cfg_Interval
        onValueModified: cfg_Interval = value
    }
}
