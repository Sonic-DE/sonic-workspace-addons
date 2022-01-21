/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.12 as Kirigami

import org.kde.potd.private 0.1 as PotdPlugin

Item {
    id: delegate

    property alias source: wallpaperImage.source
    property alias status: wallpaperImage.status
    property alias title: titleLabel.text
    property alias author: authorLabel.text

    PotdPlugin.Backend {
        id: backend
    }

    Kirigami.ShadowedRectangle {
        id: thumbnail
        // From kdeclarative/src/qmlcontrols/kcmcontrols/qml/GridDelegate.qml
        anchors {
           fill: parent
           verticalCenterOffset: Math.ceil(-labelLayout.height/2)
        }
        radius: Kirigami.Units.smallSpacing
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.View

        shadow.xOffset: 0
        shadow.yOffset: 2
        shadow.size: 10
        shadow.color: Qt.rgba(0, 0, 0, 0.3)

        color: Kirigami.Theme.backgroundColor

        Rectangle {
            id: thumbnailArea
            radius: Kirigami.Units.smallSpacing / 2
            anchors {
                fill: parent
                margins: Kirigami.Units.smallSpacing
            }

            color: Kirigami.Theme.backgroundColor

            Image {
                id: wallpaperImage

                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop

                // Reduce memory usage
                sourceSize: Qt.size(implicitWidth, implicitHeight)

                layer.enabled: true
                layer.effect: FastBlur {
                    source: wallpaperImage
                    anchors.fill: source
                    radius: 4
                }
            }

            // "None/There's nothing here" indicator
            Kirigami.Icon {
                visible: wallpaperImage.status !== Image.Ready
                anchors.centerIn: parent
                width: Kirigami.Units.iconSizes.large
                height: width
                source: "edit-none"
            }

            RowLayout {
                anchors {
                    right: parent.right
                    rightMargin: Kirigami.Units.smallSpacing
                    bottom: parent.bottom
                    bottomMargin: Kirigami.Units.smallSpacing
                }

                // Always show above thumbnail content
                z: 9999

                QQC2.Button {
                    icon.name: backend.saveStatus === PotdPlugin.Global.None ? "document-save"
                            : backend.saveStatus === PotdPlugin.Global.Succeeded ? "checkmark" : "error"
                    activeFocusOnTab: false
                    onClicked: fileDialogLoader.active = true
                    enabled: wallpaperImage.status === Image.Ready
                    visible: enabled
                    //NOTE: there aren't any global settings where to take "official" tooltip timeouts
                    QQC2.ToolTip.delay: 1000
                    QQC2.ToolTip.timeout: 5000
                    QQC2.ToolTip.visible: (Kirigami.Settings.isMobile ? pressed : hovered)
                    QQC2.ToolTip.text: i18ndc("plasma_wallpaper_org.kde.potd", "@info:tooltip Save Picture button ", "Save Picture")
                }
            }
        }

        ColumnLayout {
            id: labelLayout
            spacing: 0
            height: Kirigami.Units.gridUnit * 2
            anchors {
                left: thumbnail.left
                right: thumbnail.right
                top: thumbnail.bottom
                topMargin: Kirigami.Units.largeSpacing
            }

            QQC2.Label {
                id: titleLabel

                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                visible: text.length > 0
                verticalAlignment: Text.AlignTop
                elide: Text.ElideRight
                font.bold: true
            }
            QQC2.Label {
                id: authorLabel
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                visible: text.length > 0
                opacity: 0.6
                font.pointSize: Kirigami.Theme.smallFont.pointSize
                font.bold: true
                elide: Text.ElideRight
            }

            Item { Layout.fillWidth: true; Layout.fillHeight: true; }
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
                    backend.copy(delegate.source, fileUrl);
                }
                fileDialogLoader.active = false
            }
            onRejected: fileDialogLoader.active = false
        }
    }
}
