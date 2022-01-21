/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQml 2.15 // For Date
import QtQuick.Layouts 1.15
import QtQuick.Window 2.0 // for Screen

import org.kde.plasma.core 2.0 as PlasmaCore // For DataSource
import org.kde.kirigami 2.12 as Kirigami // For Units

import org.kde.kquickcontrolsaddons 2.0 as KQCAddons // For Clipboard
import org.kde.potd.private 0.1 as PotdPlugin

ColumnLayout {
    id: wallpaperPreview

    spacing: 0

    property string fullProviderName
    readonly property string provider: wallpaper.configuration.Provider
    readonly property string category: wallpaper.configuration.Category
    readonly property string identifier: provider === 'unsplash' && category ? provider + ':' + category : provider

    readonly property bool isLoading: engine.data[identifier] && engine.data[identifier].ImageLoadingStatus || false

    readonly property url localUrl: engine.data[identifier] ? engine.data[identifier].Url : ""
    readonly property url infoUrl: engine.data[identifier] ? engine.data[identifier].InfoUrl : ""
    readonly property url remoteUrl: engine.data[identifier] ? engine.data[identifier].RemoteUrl : ""
    readonly property string title: engine.data[identifier] ? engine.data[identifier].Title : ""
    readonly property string author: engine.data[identifier] ? engine.data[identifier].Author : ""

    property QtObject __clipboard: KQCAddons.Clipboard { }

    PlasmaCore.DataSource {
        id: engine
        engine: "potd"
        connectedSources: [identifier]
    }

    PotdPlugin.Backend {
        id: backend
    }

    // Wallpaper preview (including save button)
    WallpaperDelegate {
        id: delegate
        Layout.preferredWidth: Screen.width / 10 + Kirigami.Units.smallSpacing * 2
        Layout.minimumHeight: Screen.height / 10 + Kirigami.Units.smallSpacing * 2
        source: localUrl
        thumbnailAvailable: delegate.status === Image.Ready || delegate.status === Image.Loading
        thumbnailLoading: wallpaperPreview.isLoading
        actions: [
            Kirigami.Action {
                iconName: backend.saveStatus === PotdPlugin.Global.None ? "document-save"
                        : backend.saveStatus === PotdPlugin.Global.Succeeded ? "checkmark" : "error"
                enabled: delegate.status === Image.Ready
                visible: enabled
                tooltip: i18ndc("plasma_wallpaper_org.kde.potd", "@info:tooltip Save Picture button", "Save Picture…")
                onTriggered: {
                    const capProvider = wallpaperPreview.fullProviderName.charAt(0).toUpperCase() + wallpaperPreview.fullProviderName.slice(1);
                    let filename;

                    // Filename sanitization is done in the backend
                    if (wallpaperPreview.title.length > 0 && wallpaperPreview.author.length > 0) {
                        filename = `${capProvider}-${wallpaperPreview.title}-${wallpaperPreview.author}`;
                    } else if (wallpaperPreview.title.length > 0) {
                        filename = `${capProvider}-${wallpaperPreview.title}`;
                    } else {
                        const currentDate = new Date();
                        filename = `${capProvider}-${currentDate.toLocaleDateString()}`;
                        delete currentDate;
                    }

                    backend.saveImage(wallpaperPreview.localUrl, filename);
                }
            },
            Kirigami.Action {
                iconName: "internet-services"
                enabled: wallpaperPreview.infoUrl.toString().length > 0
                visible: enabled
                tooltip: i18ndc("plasma_wallpaper_org.kde.potd", "@info:tooltip on a button", "Copy Web Page URL")
                onTriggered: __clipboard.content = wallpaperPreview.infoUrl.toString()
            },
            Kirigami.Action {
                iconName: "edit-copy-path"
                enabled: wallpaperPreview.remoteUrl.toString().length > 0
                visible: enabled
                tooltip: i18ndc("plasma_wallpaper_org.kde.potd", "@info:tooltip on a button", "Copy Image URL")
                onTriggered: __clipboard.content = wallpaperPreview.remoteUrl.toString()
            }
        ]
    }

    Item {
        Layout.preferredHeight: delegate.shadowOffset
    }
}
