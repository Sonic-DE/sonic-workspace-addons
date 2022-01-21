/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtGraphicalEffects 1.15 // For FastBlur
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.15 as Kirigami

Item {
    id: delegate

    property alias source: wallpaperImage.source
    property alias status: wallpaperImage.status
    readonly property int shadowOffset: thumbnail.shadow.size - thumbnail.shadow.yOffset

    /**
     * thumbnailAvailable: bool
     * Set it to true when a thumbnail is actually available: when false,
     * only an icon ("edit-none") will be shown instead of the actual thumbnail.
     */
    property bool thumbnailAvailable: false

    /**
     * thumbnailLoading: bool
     * Set it to true when a thumbnail is still being loaded: when false,
     * the BusyIndicator will be shown.
     */
    property bool thumbnailLoading: false

    /**
     * actions: list<Action>
     * A list of extra actions for the thumbnails. They will be shown as
     * icons on the bottom-right corner of the thumbnail on mouse over
     */
    property list<QtObject> actions

    // From kdeclarative/src/qmlcontrols/kcmcontrols/qml/GridDelegate.qml
    Kirigami.ShadowedRectangle {
        id: thumbnail
        anchors.fill: parent
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

            QQC2.BusyIndicator {
                visible: delegate.thumbnailLoading
                anchors.centerIn: parent
                running: parent.visible
                width: Kirigami.Units.iconSizes.large
                height: width
                opacity: 0.5
            }

            // "None/There's nothing here" indicator
            Kirigami.Icon {
                visible: !(delegate.thumbnailAvailable || delegate.thumbnailLoading)
                anchors.centerIn: parent
                width: Kirigami.Units.iconSizes.large
                height: width
                source: "edit-none"
                opacity: 0.5
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

                Repeater {
                    model: delegate.actions
                    delegate: QQC2.Button {
                        icon.name: modelData.iconName
                        activeFocusOnTab: false
                        onClicked: modelData.trigger()
                        enabled: modelData.enabled
                        visible: modelData.visible
                        //NOTE: there aren't any global settings where to take "official" tooltip timeouts
                        QQC2.ToolTip.delay: 1000
                        QQC2.ToolTip.timeout: 5000
                        QQC2.ToolTip.visible: (Kirigami.Settings.isMobile ? pressed : hovered) && modelData.tooltip.length > 0
                        QQC2.ToolTip.text: modelData.tooltip
                    }
                }
            }
        }
    }

    // Loader {
    //     id: fileDialogLoader
    //     active: false
    //     sourceComponent: FileDialog {
    //         id: fileDialog
    //         title: i18ndc("plasma_wallpaper_org.kde.potd", "@title:window", "Save Today's Picture")
    //         folder: shortcuts.pictures
    //         nameFilters: [ i18ndc("plasma_wallpaper_org.kde.potd", "Template for file dialog", "Image Files (*.jpg *.jpeg)") ]
    //         defaultSuffix: ".jpg"
    //         selectExisting: false
    //         Component.onCompleted: open()
    //         onAccepted: {
    //             if (fileUrl != "") {
    //                 backend.copy(delegate.source, fileUrl);
    //             }
    //             fileDialogLoader.active = false
    //         }
    //         onRejected: fileDialogLoader.active = false
    //     }
    // }
}
