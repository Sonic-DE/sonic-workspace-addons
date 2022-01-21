/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.potd.private 0.1 as PotdPlugin

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

    PotdPlugin.Backend {
        id: backend
    }

    // Title
    QQC2.Label {
        visible: title.length > 0
        text: title
    }

    // Author
    QQC2.Label {
        visible: author.length > 0
        text: author
    }

    // Save button
    RowLayout {
        QQC2.Button {
            enabled: localUrl.toString().length > 0
            icon.name: "document-save"
            text: i18ndc("plasma_wallpaper_org.kde.potd", "@action:button save the picture of the day to local disk", "Save Picture…")
            onClicked: fileDialogLoader.active = true
        }

        QQC2.Label {
            visible: backend.saveStatus !== PotdPlugin.Global.None
            text: backend.saveStatus === PotdPlugin.Global.Succeeded ?
                i18ndc("plasma_wallpaper_org.kde.potd", "@info:status when saving picture succeeds", "Picture saved.")
              : i18ndc("plasma_wallpaper_org.kde.potd", "@info:status when saving picture fails", "Failed to save the picture!")
        }
    }


    Loader {
        id: fileDialogLoader
        active: false
        sourceComponent: FileDialog {
            id: fileDialog
            title: i18ndc("plasma_wallpaper_org.kde.potd", "@title:window", "Save Today's Picture")
            folder: shortcuts.pictures
            nameFilters: [ i18ndc("plasma_wallpaper_org.kde.potd", "Template for file dialog", "Image Files (*.jpg *.jpeg)") ]
            defaultSuffix: ".jpg"
            selectExisting: false
            Component.onCompleted: open()
            onAccepted: {
                if (fileUrl != "") {
                    backend.copy(localUrl, fileUrl);
                }
                fileDialogLoader.active = false
            }
            onRejected: fileDialogLoader.active = false
        }
    }
}
