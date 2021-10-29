/*
 SPDX-FileCopyrightText: 2021 Ismael Asensio <isma.af@gmail.com>

 SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0

//TODO: import fails with latest version 3.0
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

                movementDirection: PathView.Positive

                // Make thumbnails slightly smaller the more there are, so it doesn't feel too crowded
                // The sizeFactor curve parameters have been calculated experimentally
                readonly property real sizeFactor: 0.2 + (0.5 / Math.log(thumbnailView.count + 0.5))
                path: Path {
                    startX: Math.round(thumbnailView.width * 0.75)
                    startY: Math.round(thumbnailView.height * 0.75)
                    PathAttribute { name: "z"; value: 100 }
                    PathAttribute { name: "scale"; value: 1 }

                    PathLine {
                        x: Math.round(thumbnailView.width * 0.2)
                        y: Math.round(thumbnailView.height * 0.2)
                    }
                    PathAttribute { name: "z"; value: 0 }
                    PathAttribute { name: "scale"; value: 0.7 }
                }

                model: tabBox.model

                delegate: Item {

                    readonly property size thumbnailSize: {
                        let thumbnailRatio = thumbnail.client.frameGeometry.width / thumbnail.client.frameGeometry.height;
                        let boxRatio = width / height;
                        if (thumbnailRatio > boxRatio) {
                            return Qt.size(width, width / thumbnailRatio);
                        } else {
                            return Qt.size(height * thumbnailRatio, height);
                        }
                    }

                    width: Math.round(tabBox.screenGeometry.width * thumbnailView.sizeFactor)
                    height: Math.round(tabBox.screenGeometry.height * thumbnailView.sizeFactor)
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

                QQC2.Label {
                    font.bold: true
                    font.pointSize: 16
                    text: tabBox.model.data(tabBox.model.index(tabBox.currentIndex, 0), Qt.UserRole + 1) // CaptionRole
                    Layout.alignment: Qt.AlignCenter
                }
            }
        }
    }

    onCurrentIndexChanged: {
        thumbnailView.currentIndex = tabBox.currentIndex
    }
}
