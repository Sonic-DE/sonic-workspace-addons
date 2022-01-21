/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Dialogs 1.3
import org.kde.kirigami 2.12 as Kirigami

import org.kde.potd.private 0.1 as PotdPlugin

Kirigami.ShadowedRectangle {
    id: wallpaperDelegate

    property alias source: wallpaperImage.source
    property alias status: wallpaperImage.status

    // From plasma-desktop/kcms/landingpage/package/contents/ui/Thumbnail.qml
    radius: Kirigami.Units.smallSpacing
    Kirigami.Theme.inherit: false
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    shadow.xOffset: 0
    shadow.yOffset: 2
    shadow.size: 10
    shadow.color: Qt.rgba(0, 0, 0, 0.3)

    color: Kirigami.Theme.backgroundColor

    PotdPlugin.Backend {
        id: backend
    }

    Image {
        id: wallpaperImage
        anchors {
            fill: parent
            margins: Kirigami.Units.smallSpacing
        }
        fillMode: Image.PreserveAspectCrop

        layer.enabled: true
        layer.effect: FastBlur {
            source: wallpaperImage
            anchors.fill: source
            radius: 4
        }

    }

    // Save button
    QQC2.Button {
        anchors {
            right: wallpaperImage.right
            bottom: wallpaperImage.bottom
            margins: Kirigami.Units.smallSpacing
        }
        enabled: wallpaperImage.status === Image.Ready
        icon.name: backend.saveStatus === PotdPlugin.Global.None ? "document-save"
                 : backend.saveStatus === PotdPlugin.Global.Succeeded ? "checkmark" : "error"
        onClicked: fileDialogLoader.active = true

        QQC2.ToolTip {
            text: i18ndc("plasma_wallpaper_org.kde.potd", "@info:tooltip Save Picture button ", "Save Picture")
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        visible: wallpaperImage.status !== Image.Ready
        icon.name: "edit-none" // Use icon instead of text because in some languages the text is too long
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
                    backend.copy(wallpaperDelegate.source, fileUrl);
                }
                fileDialogLoader.active = false
            }
            onRejected: fileDialogLoader.active = false
        }
    }
}
