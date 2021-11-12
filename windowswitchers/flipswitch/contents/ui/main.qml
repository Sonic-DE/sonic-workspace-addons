/*
 SPDX-FileCopyrightText: 2021 Ismael Asensio <isma.af@gmail.com>

 SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.15 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PC3

import org.kde.kwin 2.0 as KWin


KWin.Switcher {
    id: tabBox
    currentIndex: thumbnailView.currentIndex

    PlasmaCore.Dialog {
        id: dialog
        location: PlasmaCore.Types.Floating
        visible: tabBox.visible
        //backgroundHints: PlasmaCore.Dialog.NoBackground
        flags: Qt.X11BypassWindowManagerHint
        x: 0
        y: 0

        mainItem: ColumnLayout {
            width: tabBox.screenGeometry.width - dialog.margins.left - dialog.margins.right
            height: tabBox.screenGeometry.height - dialog.margins.top - dialog.margins.bottom

            PathView {
                id: thumbnailView

                focus: true
                Layout.fillWidth: true
                Layout.fillHeight: true

                preferredHighlightBegin: 1/(count + 1)
                preferredHighlightEnd: preferredHighlightBegin
                highlightRangeMode: PathView.StrictlyEnforceRange
                highlightMoveDuration: PlasmaCore.Units.longDuration * 2

                pathItemCount: 12

                path: Path {
                    // Nearest point of the path
                    startX: Math.round(thumbnailView.width * 0.75)
                    startY: Math.round(thumbnailView.height * 0.85)
                    PathAttribute { name: "z"; value: 100 }
                    PathAttribute { name: "scale"; value: 1 }

                    // Back point of the path on top-left corner
                    PathLine {
                        x: Math.round(thumbnailView.width * 0.25)
                        y: Math.round(thumbnailView.height * 0.25)
                    }
                    PathAttribute { name: "z"; value: 0 }
                    PathAttribute { name: "scale"; value: 0.7 }
                }

                model: tabBox.model

                delegate: Item {
                    readonly property string caption: thumbnail.client.caption
                    readonly property var icon: thumbnail.client.icon

                    // Make thumbnails slightly smaller the more there are, so it doesn't feel too crowded
                    // The sizeFactor curve parameters have been calculated experimentally
                    readonly property real sizeFactor: 0.35 + (0.5 / (thumbnailView.count + 1))
                    readonly property bool isWider: thumbnail.ratio > tabBox.screenGeometry.width / tabBox.screenGeometry.height

                    width: Math.round((isWider ? tabBox.screenGeometry.width : tabBox.screenGeometry.height * thumbnail.ratio) * sizeFactor)
                    height: Math.round((isWider ? tabBox.screenGeometry.width / thumbnail.ratio : tabBox.screenGeometry.height) * sizeFactor)
                    scale: PathView.scale
                    z: PathView.z

                    // Reduce opacity on the end so items dissapear more naturally
                    opacity: Math.min(1, (1 - z/100) / thumbnailView.preferredHighlightBegin);

                    KWin.ThumbnailItem {
                        id: thumbnail
                        wId: windowId
                        anchors.fill: parent

                        // TODO: Expose this property in the ThumbnailItem API, to avoid the warning
                        // QQmlExpression: depends on non-NOTIFYable properties: KWin::X11Client::frameGeometry
                        readonly property double ratio: thumbnail.client.frameGeometry.width / thumbnail.client.frameGeometry.height
                    }

                    Kirigami.ShadowedRectangle {
                        anchors.fill: parent
                        z: -1

                        color: "transparent"
                        shadow.size: 30
                        shadow.color: "#202020"
                        shadow.yOffset: 1
                    }

                    TapHandler {
                        grabPermissions: PointerHandler.TakeOverForbidden
                        gesturePolicy: TapHandler.WithinBounds
                        onSingleTapped: {
                            if (index === thumbnailView.currentIndex) {
                                thumbnailView.model.activate(index);
                                return;
                            }
                            thumbnailView.movementDirection = PathView.Positive
                            thumbnailView.currentIndex = index
                        }
                    }
                }

                transform: Rotation {
                    origin { x: thumbnailView.width/2; y: thumbnailView.height/2 }
                    axis { x: 0; y: 1; z: -0.15 }
                    angle: 10
                }

                highlight: PlasmaCore.FrameSvgItem {
                    id: highlightItem

                    readonly property Item target: thumbnailView.currentItem
                    visible: target != null && target.z > 20

                    imagePath: "widgets/viewitem"
                    prefix: "hover"

                    anchors.centerIn: target
                    width: target.width + PlasmaCore.Units.largeSpacing
                    height: target.height + PlasmaCore.Units.largeSpacing
                    scale: target.scale
                    z: target.z - 1
                }

                Keys.onUpPressed: decrementCurrentIndex()
                Keys.onLeftPressed: decrementCurrentIndex()
                Keys.onDownPressed: incrementCurrentIndex()
                Keys.onRightPressed: incrementCurrentIndex()
            }

            RowLayout {
                Layout.preferredHeight: PlasmaCore.Units.iconSizes.large
                Layout.margins: PlasmaCore.Units.gridUnit
                Layout.alignment: Qt.AlignCenter
                spacing: PlasmaCore.Units.largeSpacing

                PlasmaCore.IconItem {
                    source: thumbnailView.currentItem.icon
                    width: PlasmaCore.Units.iconSizes.large
                    height: width
                    Layout.alignment: Qt.AlignCenter
                }

                PC3.Label {
                    font.bold: true
                    font.pointSize: Math.round(PlasmaCore.Theme.defaultFont.pointSize * 1.6)
                    text: thumbnailView.currentItem.caption
                    maximumLineCount: 1
                    elide: Text.ElideMiddle
                    Layout.maximumWidth: tabBox.screenGeometry.width * 0.8
                    Layout.alignment: Qt.AlignCenter
                }
            }
        }
    }

    onCurrentIndexChanged: {
        if (currentIndex === thumbnailView.currentIndex) {
            return
        }

        // WindowSwitcher always changes currentIndex in increments of 1.
        // Detect the change direction and set the PathView movement accordingly, so fast changes
        // in the same direction don't result into a combination of forward and backward movements.
        if (thumbnailView.count === 2 || (currentIndex === 0 && thumbnailView.currentIndex === thumbnailView.count - 1)) {
            thumbnailView.movementDirection = PathView.Positive
        } else if (currentIndex === thumbnailView.count - 1 && thumbnailView.currentIndex === 0) {
            thumbnailView.movementDirection = PathView.Negative
        } else {
            thumbnailView.movementDirection = (currentIndex > thumbnailView.currentIndex) ? PathView.Positive : PathView.Negative
        }

        thumbnailView.currentIndex = tabBox.currentIndex
    }

    onVisibleChanged: {
        // Reset the PathView index when hiding to avoid unwanted animations on relaunch
        if (!visible) {
            thumbnailView.currentIndex = 0;
        }
    }
}
