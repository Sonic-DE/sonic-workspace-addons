/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.12 as Kirigami

Rectangle {
    id: wallpaperDelegate

    property alias source: wallpaperImage.source
    property alias status: wallpaperImage.status

    color: PlasmaCore.ColorScope.backgroundColor

    Image {
        id: wallpaperImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop

        layer.enabled: true
        layer.effect: FastBlur {
            source: wallpaperImage
            anchors.fill: source
            radius: 4
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        visible: wallpaperImage.status !== Image.Ready
        icon.name: "edit-none" // Use icon instead of text because in some languages the text is too long
    }
}
