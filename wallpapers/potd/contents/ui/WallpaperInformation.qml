/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore

ColumnLayout {
    id: wallpaperInformation

    readonly property string provider: wallpaper.configuration.Provider
    readonly property string category: wallpaper.configuration.Category
    readonly property string identifier: provider === 'unsplash' && category ? provider + ':' + category : provider

    readonly property url localUrl: engine.data[identifier] && engine.data[identifier].Url ? engine.data[identifier].Url : ""
    readonly property string title: engine.data[identifier] && engine.data[identifier].Title ? engine.data[identifier].Title : ""
    readonly property string author: engine.data[identifier] && engine.data[identifier].Author ? engine.data[identifier].Author : ""

    PlasmaCore.DataSource {
        id: engine
        engine: "potd"
        connectedSources: [identifier]
    }

    QQC2.Label {
        visible: title.length > 0
        text: title
    }

    QQC2.Label {
        visible: author.length > 0
        text: author
    }

    QQC2.Button {
        enabled: localUrl.toString().length > 0
        icon.name: "document-save"
        text: i18ndc("plasma_wallpaper_org.kde.potd", "@button", "Save the image")
    }
}
