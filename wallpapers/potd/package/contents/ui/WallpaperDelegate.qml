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
import org.kde.kquickcontrolsaddons 2.0 as KQCAddons // For QImageItem

FocusScope {
    // FocusScope can pass Tab to inlined buttons
    id: delegate

    readonly property int shadowOffset: thumbnail.shadow.size - thumbnail.shadow.yOffset
    readonly property alias isNull: wallpaperImage.null

    /**
     * The wallpaper image
     */
    property alias image: wallpaperImage.image

    /**
     * The website of the wallpaper
     */
    property url infoUrl

    /**
     * The title of the wallpaper
     */
    property string title

    /**
     * The author of the wallpaper
     */
    property string author

    /**
     * Set it to true when a thumbnail is actually available: when false,
     * only an icon ("edit-none") will be shown instead of the actual thumbnail.
     */
    property bool thumbnailAvailable: false

    /**
     * Set it to true when a thumbnail is still being loaded: when false,
     * the BusyIndicator will be shown.
     */
    property bool thumbnailLoading: false

    /**
     * A list of extra actions for the thumbnails. They will be shown as
     * icons on the bottom-right corner of the thumbnail on mouse over
     */
    property list<QtObject> actions

    ActionContextMenu {
        id: contextMenu
        showAllActions: thumbnailAvailable
        actions: delegate.actions
    }

    Keys.onMenuPressed: contextMenu.popup(delegate, thumbnail.x, thumbnail.y + thumbnail.height)
    Keys.onSpacePressed: contextMenu.popup(delegate, thumbnail.x, thumbnail.y + thumbnail.height)

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: contextMenu.popup()
    }

    TapHandler {
        acceptedDevices: PointerDevice.TouchScreen | PointerDevice.Stylus
        onLongPressed: contextMenu.popup()
    }

    TapHandler {
        id: openUrlTapHandler
        enabled: hoverHandler.enabled
        acceptedButtons: Qt.LeftButton
        onTapped: Qt.openUrlExternally(delegate.infoUrl)
    }

    HoverHandler {
        id: hoverHandler
        enabled: delegate.thumbnailAvailable && delegate.infoUrl.toString().length > 0
        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    QQC2.ToolTip.delay: Kirigami.Units.veryLongDuration * 2
    QQC2.ToolTip.text: delegate.infoUrl.toString()
    QQC2.ToolTip.timeout: Kirigami.Units.veryLongDuration * 10
    QQC2.ToolTip.visible: !contextMenu.opened
        && (hoverHandler.hovered
            || thumbnailArea.activeFocus
            || (Kirigami.Settings.isMobile && openUrlTapHandler.pressed))

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

        color: thumbnailArea.activeFocus ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor

        Rectangle {
            id: thumbnailArea
            radius: Kirigami.Units.smallSpacing / 2
            anchors {
                fill: parent
                margins: Kirigami.Units.smallSpacing
            }

            color: Kirigami.Theme.backgroundColor

            activeFocusOnTab: true
            Accessible.name: delegate.thumbnailAvailable ? i18nc("@info:whatsthis", "Today's picture")
                           : delegate.thumbnailLoading ? i18nc("@info:whatsthis", "Loading")
                                                       : i18nc("@info:whatsthis", "Unavailable")
            Accessible.description: delegate.thumbnailAvailable ? i18nc("@info:whatsthis for an image %1 title %2 author", "%1 Author: %2. Right-click on the image to see more actions.", delegate.title, delegate.author)
                                  : delegate.thumbnailLoading ? i18nc("@info:whatsthis", "The wallpaper is being fetched from the Internet.")
                                                              : i18nc("@info:whatsthis", "Failed to fetch the wallpaper from the Internet.")

            KQCAddons.QImageItem {
                id: wallpaperImage

                anchors.fill: parent
                fillMode: wallpaper.configuration.FillMode

                layer.enabled: true
                layer.effect: FastBlur {
                    source: wallpaperImage
                    anchors.fill: source
                    radius: 4
                }
            }

            Loader {
                active: delegate.thumbnailLoading || !delegate.thumbnailAvailable

                anchors.centerIn: parent
                opacity: 0.5
                visible: active

                width: Kirigami.Units.iconSizes.large
                height: width

                sourceComponent: delegate.thumbnailLoading ? busyIndicator : fallbackIcon

                Component {
                    id: busyIndicator

                    QQC2.BusyIndicator {
                        anchors.fill: parent
                    }
                }

                // "None/There's nothing here" indicator
                Component {
                    id: fallbackIcon

                    Kirigami.Icon {
                        anchors.fill: parent
                        source: "edit-none"
                    }
                }
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
                        activeFocusOnTab: visible
                        onClicked: modelData.trigger()
                        enabled: modelData.enabled
                        visible: modelData.visible

                        Accessible.name: modelData.tooltip
                        Accessible.description: modelData.Accessible.description

                        //NOTE: there aren't any global settings where to take "official" tooltip timeouts
                        QQC2.ToolTip.delay: Kirigami.Units.veryLongDuration * 2
                        QQC2.ToolTip.timeout: Kirigami.Units.veryLongDuration * 10
                        QQC2.ToolTip.visible: modelData.tooltip.length > 0
                            &&((Kirigami.Settings.isMobile ? pressed : hovered)
                                || (activeFocus && focusReason === Qt.TabFocusReason
                                    || focusReason === Qt.BacktabFocusReason))
                        QQC2.ToolTip.text: modelData.tooltip
                    }
                }
            }
        }
    }
}
