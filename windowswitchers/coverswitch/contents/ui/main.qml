/*
 SPDX-FileCopyrightText: 2021 Ismael Asensio <isma.af@gmail.com>

 SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0

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

                preferredHighlightBegin: 0.49
                preferredHighlightEnd: 0.49
                highlightRangeMode: PathView.StrictlyEnforceRange
                highlightMoveDuration: PlasmaCore.Units.longDuration * 2.5

                movementDirection: (count == 2) ? PathView.Positive : PathView.Shortest

                path: Path {
                    startX: thumbnailView.width * 0.1; startY: thumbnailView.height * 0.55
                    PathAttribute { name: "z"; value: 0 }
                    PathAttribute { name: "scale"; value: 0.7 }
                    PathAttribute { name: "rotation"; value: 70 }
                    PathPercent { value: 0 }

                    PathLine { x: thumbnailView.width * 0.25 ; y: thumbnailView.height * 0.55 }
                    PathAttribute { name: "z"; value: 90 }
                    PathAttribute { name: "scale"; value: 0.7 }
                    PathAttribute { name: "rotation"; value: 70 }
                    PathPercent { value: 0.4 }

                    // Center Item
                    PathQuad {
                        x: thumbnailView.width * 0.5 ; y: thumbnailView.height * 0.65
                        controlX: thumbnailView.width * 0.45; controlY: thumbnailView.height * 0.6
                    }
                    PathAttribute { name: "z"; value: 100 }
                    PathAttribute { name: "scale"; value: 1 }
                    PathAttribute { name: "rotation"; value: 0 }
                    PathPercent { value: 0.49 } // A bit less than 50% so items preferrably stack on the right side

                    PathQuad {
                        x: thumbnailView.width * 0.75 ; y: thumbnailView.height * 0.55
                        controlX: thumbnailView.width * 0.55; controlY: thumbnailView.height * 0.6
                    }
                    PathAttribute { name: "z"; value: 90 }
                    PathAttribute { name: "scale"; value: 0.7 }
                    PathAttribute { name: "rotation"; value: -70 }
                    PathPercent { value: 0.6 }

                    PathLine { x: thumbnailView.width * 0.9 ; y: thumbnailView.height * 0.55 }
                    PathAttribute { name: "z"; value: 0 }
                    PathAttribute { name: "scale"; value: 0.7 }
                    PathAttribute { name: "rotation"; value: -70 }
                    PathPercent { value: 1 }
                }

                model: tabBox.model

                delegate: Item {
                    id: delegateItem

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

                    width: tabBox.screenGeometry.width / 2
                    height: tabBox.screenGeometry.height / 2
                    scale: PathView.scale
                    z: PathView.z

                    KWin.ThumbnailItem {
                        id: thumbnail
                        wId: windowId
                        anchors.fill: parent
                    }

                    transform: Rotation {
                        origin { x: delegateItem.width/2; y: delegateItem.height/2 }
                        axis { x: 0; y: 1; z: 0 }
                        angle: delegateItem.PathView.rotation
                    }
                }

                highlight: PlasmaCore.FrameSvgItem {
                    imagePath: "widgets/viewitem"
                    prefix: "hover"
                    visible: thumbnailView.currentItem

                    anchors.centerIn: thumbnailView.currentItem
                    width: thumbnailView.currentItem.thumbnailSize.width + PlasmaCore.Units.largeSpacing
                    height: thumbnailView.currentItem.thumbnailSize.height + PlasmaCore.Units.largeSpacing

                    transform: thumbnailView.currentItem.transform
                    scale: thumbnailView.currentItem.scale
                    z: thumbnailView.currentItem.z - 1
                    opacity: Math.max(0, (thumbnailView.currentItem.z - 90) / 10)
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
