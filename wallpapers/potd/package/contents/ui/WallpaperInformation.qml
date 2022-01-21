/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.0 // for Screen

import org.kde.plasma.core 2.0 as PlasmaCore // For DataSource
import org.kde.kirigami 2.12 as Kirigami // For Units

import org.kde.potd.private 0.1 as PotdPlugin

ColumnLayout {
    id: wallpaperInformation

    spacing: 0

    readonly property string provider: wallpaper.configuration.Provider
    readonly property string category: wallpaper.configuration.Category
    readonly property string identifier: provider === 'unsplash' && category ? provider + ':' + category : provider

    readonly property bool isLoading: engine.data[identifier] && engine.data[identifier].ImageLoadingStatus || false

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

    // Wallpaper preview (including save button) and information
    WallpaperDelegate {
        id: wallpaperPreview
        Layout.preferredWidth: Screen.width / 10 + Kirigami.Units.smallSpacing * 2
        Layout.minimumHeight: Screen.height / 10 + Kirigami.Units.smallSpacing * 2
        source: localUrl
        thumbnailAvailable: wallpaperPreview.status === Image.Ready || wallpaperPreview.status === Image.Loading
        thumbnailLoading: wallpaperInformation.isLoading
        actions: [
            Kirigami.Action {
                iconName: backend.saveStatus === PotdPlugin.Global.None ? "document-save"
                        : backend.saveStatus === PotdPlugin.Global.Succeeded ? "checkmark" : "error"
                enabled: wallpaperPreview.status === Image.Ready
                visible: enabled
                tooltip: i18ndc("plasma_wallpaper_org.kde.potd", "@info:tooltip Save Picture button ", "Save Picture")
                onTriggered: {
                    let filename;
                    const capProvider = wallpaperInformation.provider.charAt(0).toUpperCase() + wallpaperInformation.provider.slice(1);

                    // Filename sanitization is done in the backend
                    if (wallpaperInformation.title.length > 0 && wallpaperInformation.author.length > 0) {
                        filename = `${capProvider}-${wallpaperInformation.title}-${wallpaperInformation.author}`;
                    } else if (wallpaperInformation.title.length > 0) {
                        filename = `${capProvider}-${wallpaperInformation.title}`;
                    }

                    backend.saveImage(wallpaperInformation.localUrl, filename);
                }
            }
        ]
    }

    Item {
        Layout.preferredHeight: wallpaperPreview.shadowOffset
    }
}
