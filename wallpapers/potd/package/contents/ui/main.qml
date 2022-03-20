/*
 *   SPDX-FileCopyrightText: 2016 Weng Xuetian <wengxt@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.5
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0

import org.kde.plasma.wallpapers.potd 1.0

Rectangle {
    id: root

    readonly property string provider: wallpaper.configuration.Provider
    readonly property string category: wallpaper.configuration.Category

    PotdProviderModel {
        id: engine
        identifier: root.provider
        arguments: root.provider === "unsplash" ? [category] : []
        running: true
    }

    Rectangle {
        id: backgroundColor
        anchors.fill: parent
        color: wallpaper.configuration.Color
        Behavior on color {
            ColorAnimation { duration: PlasmaCore.Units.longDuration }
        }
    }

    QImageItem {
        anchors.fill: parent
        image: engine.image
        fillMode: wallpaper.configuration.FillMode
        smooth: true
    }
}
