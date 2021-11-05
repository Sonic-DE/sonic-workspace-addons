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
                    // TODO: Expose this in the ThumbnailItem API, to avoid the warning
                    // QQmlExpression: depends on non-NOTIFYable properties: KWin::X11Client::frameGeometry
                    readonly property size thumbnailSize: {
                        let thumbnailRatio = thumbnail.client.frameGeometry.width / thumbnail.client.frameGeometry.height;
                        let boxRatio = width / height;
                        if (thumbnailRatio > boxRatio) {
                            return Qt.size(width, width / thumbnailRatio);
                        } else {
                            return Qt.size(height * thumbnailRatio, height);
                        }
                    }

                    // Make thumbnails slightly smaller the more there are, so it doesn't feel too crowded
                    // The sizeFactor curve parameters have been calculated experimentally
                    readonly property real sizeFactor: 0.35 + (0.5 / (thumbnailView.count + 1))

                    width: Math.round(tabBox.screenGeometry.width * sizeFactor)
                    height: Math.round(tabBox.screenGeometry.height * sizeFactor)
                    scale: PathView.scale
                    z: PathView.z

                    // Reduce opacity on the end so items dissapear more naturally
                    opacity: Math.min(1, (1 - z/100) / thumbnailView.preferredHighlightBegin);

                    KWin.ThumbnailItem {
                        id: thumbnail
                        wId: windowId
                        anchors.fill: parent
                    }

                    Kirigami.ShadowedRectangle {
                        anchors.centerIn: parent
                        width: thumbnailSize.width
                        height: thumbnailSize.height
                        z: -1

                        color: "transparent"
                        shadow.size: 30
                        shadow.color: "#202020" // TODO: Select shadow color (is it themable?)
                        shadow.yOffset: 1
                    }

                    TapHandler {
                        grabPermissions: PointerHandler.TakeOverForbidden
                        gesturePolicy: TapHandler.WithinBounds
                        onSingleTapped: {
                            thumbnailView.movementDirection = PathView.Positive
                            thumbnailView.currentIndex = index
                            thumbnailView.movementDirection = PathView.Shortest
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
                    width: target.thumbnailSize.width + PlasmaCore.Units.largeSpacing
                    height: target.thumbnailSize.height + PlasmaCore.Units.largeSpacing
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
                    font.pointSize: 16
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

        // If there are only two items prefer the Positive direction
        if (thumbnailView.count === 2) {
            thumbnailView.incrementCurrentIndex()
            return
        }

        // HACK: With 3 thumbnails, the shortest path is not always the expected one
        // BUG https://bugreports.qt.io/browse/QTBUG-15314 (marked as resolved but not really)
        if (thumbnailView.count === 3) {
            if ((thumbnailView.currentIndex === 0 && currentIndex === 1)
                 || (thumbnailView.currentIndex === 1 && currentIndex === 2)
                 || (thumbnailView.currentIndex === 2 && currentIndex === 0)) {
                thumbnailView.incrementCurrentIndex()
            } else {
                thumbnailView.decrementCurrentIndex()
            }
            return
        }

        thumbnailView.currentIndex = tabBox.currentIndex
    }
}
