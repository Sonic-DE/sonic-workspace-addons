/*
 SPDX-FileCopyrightText: 2021 Ismael Asensio <isma.af@gmail.com>

 SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

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
            id: dialogMainItem
            width: tabBox.screenGeometry.width
            height: tabBox.screenGeometry.height

            PathView {
                id: thumbnailView

                focus: true
                Layout.fillWidth: true
                Layout.fillHeight: true

                movementDirection: (count == 2) ? PathView.Positive : PathView.Shortest

                path: Path {
                    // Selected thumbnail. Center it a little bit and reserve space for the Y rotation
                    startX: Math.round(thumbnailView.width * 0.65)
                    startY: Math.round(thumbnailView.height * 0.65)
                    PathAttribute { name: "z"; value: 100 }
                    PathAttribute { name: "scale"; value: 1 }

                    // Last item on top-left corner
                    PathLine {
                        x: Math.round(thumbnailView.width * 0.2)
                        y: Math.round(thumbnailView.height * 0.2)
                    }
                    PathAttribute { name: "z"; value: 0 }
                    PathAttribute { name: "scale"; value: 0.7 }
                }

                model: tabBox.model

                delegate: Item {
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
                    readonly property real sizeFactor: 0.4 + (0.5 / (thumbnailView.count + 3))

                    width: Math.round(tabBox.screenGeometry.width * sizeFactor)
                    height: Math.round(tabBox.screenGeometry.height * sizeFactor)
                    scale: PathView.scale
                    z: PathView.z

                    KWin.ThumbnailItem {
                        id: thumbnail
                        wId: windowId
                        anchors.fill: parent
                    }
                }

                transform: Rotation {
                    origin { x: thumbnailView.width/2; y: thumbnailView.height/2 }
                    axis { x: 0; y: 1; z: 0 }
                    angle: 10
                }

                highlight: PlasmaCore.FrameSvgItem {
                    imagePath: "widgets/viewitem"
                    prefix: "hover"

                    anchors.centerIn: thumbnailView.currentItem
                    width: thumbnailView.currentItem.thumbnailSize.width + PlasmaCore.Units.largeSpacing
                    height: thumbnailView.currentItem.thumbnailSize.height + PlasmaCore.Units.largeSpacing

                    scale: thumbnailView.currentItem.scale
                    z: thumbnailView.currentItem.z - 1
                    opacity: Math.max(0, (thumbnailView.currentItem.z - 80) / 20)
                }

                Keys.onUpPressed: decrementCurrentIndex()
                Keys.onLeftPressed: decrementCurrentIndex()
                Keys.onDownPressed: incrementCurrentIndex()
                Keys.onRightPressed: incrementCurrentIndex()
            }

            RowLayout {
                Layout.preferredHeight: PlasmaCore.Units.iconSizes.huge
                Layout.margins: PlasmaCore.Units.gridUnit
                Layout.alignment: Qt.AlignCenter
                spacing: PlasmaCore.Units.largeSpacing

                PlasmaCore.IconItem {
                    source: tabBox.model.data(tabBox.model.index(tabBox.currentIndex, 0), Qt.UserRole + 3) // IconRole
                    Layout.alignment: Qt.AlignCenter
                }

                PC3.Label {
                    font.bold: true
                    font.pointSize: 16
                    text: tabBox.model.data(tabBox.model.index(tabBox.currentIndex, 0), Qt.UserRole + 1) // CaptionRole
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
