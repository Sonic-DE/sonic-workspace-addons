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
import org.kde.kirigami 2.12 as Kirigami // For Action and Units

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

    PlasmaCore.DataSource {
        id: engine
        engine: "potd"
        connectedSources: [identifier]
    }

    // Wallpaper preview (including save button)
    WallpaperDelegate {
        id: delegate
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: Screen.width / 10 + Kirigami.Units.smallSpacing * 2
        Layout.minimumHeight: Screen.height / 10 + Kirigami.Units.smallSpacing * 2

        source: wallpaperPreview.localUrl

        thumbnailAvailable: delegate.status === Image.Ready || delegate.status === Image.Loading
        thumbnailLoading: wallpaperPreview.isLoading

        actions: [
            Kirigami.Action {
                iconName: "document-save"
                enabled: delegate.status === Image.Ready
                visible: false
                tooltip: i18ndc("plasma_wallpaper_org.kde.potd", "@action:inmenu wallpaper preview menu", "Save Image as…")
                onTriggered: saveButton.savedUrl = wallpaperPreview.saveImage()
            },
            Kirigami.Action {
                iconName: "internet-services"
                enabled: wallpaperPreview.infoUrl.toString().length > 0
                visible: false
                tooltip: i18ndc("plasma_wallpaper_org.kde.potd", "@action:inmenu wallpaper preview menu, will open the information page of the wallpaper", "Open Link in Browser…")
                onTriggered: Qt.openUrlExternally(wallpaperPreview.infoUrl)
            }
        ]
    }

    // Reload the image when a new one is provided
    onTitleChanged: {
        delegate.source = "";
        delegate.source = Qt.binding(() => wallpaperPreview.localUrl);
    }

    function saveImage() {
        let filename;
        // Filename sanitization and ".jpg" suffix is done in the backend
        if (wallpaperPreview.title.length > 0 && wallpaperPreview.author.length > 0) {
            filename = `${wallpaperPreview.fullProviderName}-${wallpaperPreview.author}-${wallpaperPreview.title}`;
        } else if (wallpaperPreview.title.length > 0) {
            filename = `${wallpaperPreview.fullProviderName}-${wallpaperPreview.title}`;
        } else {
            const currentDate = new Date();
            filename = `${wallpaperPreview.fullProviderName}-${currentDate.toLocaleDateString()}`;
            delete currentDate;
        }

        return backend.saveImage(wallpaperPreview.localUrl, filename);
    }
}
