/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import QtQuick.Window 2.0 // for Screen
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kirigami 2.12 as Kirigami

ColumnLayout {
    id: wallpaperInformation

    readonly property string provider: wallpaper.configuration.Provider
    readonly property string category: wallpaper.configuration.Category
    readonly property string identifier: provider === 'unsplash' && category ? provider + ':' + category : provider

    readonly property url localUrl: engine.data[identifier] ? engine.data[identifier].Url : ""
    readonly property string title: engine.data[identifier] ? engine.data[identifier].Title : ""
    readonly property string author: engine.data[identifier] ? engine.data[identifier].Author : ""

    PlasmaCore.DataSource {
        id: engine
        engine: "potd"
        connectedSources: [identifier]
    }

    // Wallpaper preview (including save button) and information
    WallpaperDelegate {
        id: wallpaperPreview
        Layout.preferredWidth: Screen.width / 10 + Kirigami.Units.smallSpacing * 2
        Layout.preferredHeight: Screen.height / 10 + Kirigami.Units.smallSpacing * 2
        Layout.alignment: Qt.AlignHCenter
        source: localUrl
    }

    // Title
    QQC2.Label {
        visible: text.length > 0
        Layout.minimumWidth: wallpaperPreview.Layout.preferredWidth
        Layout.maximumWidth: wallpaperPreview.Layout.preferredWidth
        horizontalAlignment: Text.AlignHCenter
        text: wallpaperInformation.title
        wrapMode: Text.Wrap
    }

    // Author
    QQC2.Label {
        visible: text.length > 0
        Layout.minimumWidth: wallpaperPreview.Layout.preferredWidth
        Layout.maximumWidth: wallpaperPreview.Layout.preferredWidth
        horizontalAlignment: Text.AlignHCenter
        text: wallpaperInformation.author
        wrapMode: Text.Wrap
    }
}
