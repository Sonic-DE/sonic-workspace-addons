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

                preferredHighlightBegin: 0.49
                preferredHighlightEnd: 0.49
                highlightRangeMode: PathView.StrictlyEnforceRange
                highlightMoveDuration: PlasmaCore.Units.longDuration * 2.5

                pathItemCount: 13

                path: Path {
                    // Left stack
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

                    // Right stack
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

                    readonly property string caption: model.caption
                    readonly property var icon: model.icon

                    readonly property bool isWider: thumbnail.ratio > tabBox.screenGeometry.width / tabBox.screenGeometry.height
                    width: (isWider ? tabBox.screenGeometry.width : tabBox.screenGeometry.height * thumbnail.ratio) / 2
                    height: (isWider ? tabBox.screenGeometry.width / thumbnail.ratio : tabBox.screenGeometry.height) / 2
                    scale: PathView.scale
                    z: Math.round(PathView.z)

                    // TODO: Reduce opacity at the ends of the path, so items appear and dissapear from the back more naturally
                    // It doesn't work well for even items as the last one is positioned at the right end of the path
                    // It would be nice to do it only when items are moving, but I don't see how to get that information from PathView
                    // opacity: Math.min(1, z/20)

                    KWin.ThumbnailItem {
                        id: thumbnail
                        readonly property double ratio: implicitWidth / implicitHeight

                        wId: windowId
                        anchors.fill: parent
                    }

                    Kirigami.ShadowedRectangle {
                        anchors.fill: parent
                        z: -1

                        color: "transparent"
                        shadow.size: 30
                        shadow.color: "#202020"
                    }

                    transform: Rotation {
                        origin { x: delegateItem.width/2; y: delegateItem.height/2 }
                        axis { x: 0; y: 1; z: 0 }
                        angle: delegateItem.PathView.rotation
                    }

                    TapHandler {
                        grabPermissions: PointerHandler.TakeOverForbidden
                        gesturePolicy: TapHandler.WithinBounds
                        onSingleTapped: {
                            if (index === thumbnailView.currentIndex) {
                                thumbnailView.model.activate(index);
                                return;
                            }
                            thumbnailView.movementDirection = (delegateItem.PathView.rotation < 0) ? PathView.Positive : PathView.Negative
                            thumbnailView.currentIndex = index
                        }
                    }
                }

                highlight: PlasmaCore.FrameSvgItem {
                    id: highlightItem

                    readonly property Item target: thumbnailView.currentItem
                    visible: target && target.z > 80

                    imagePath: "widgets/viewitem"
                    prefix: "hover"

                    anchors.centerIn: target
                    width: target ? target.width + PlasmaCore.Units.largeSpacing : 0
                    height: target ? target.height + PlasmaCore.Units.largeSpacing : 0
                    scale: target ? target.scale : 1
                    z: target ? target.z - 1 : -1
                    // The transform cannot be directly assigned as the transform origin is different
                    transform: Rotation {
                        origin { x: highlightItem.width/2; y: highlightItem.height/2 }
                        axis { x: 0; y: 1; z: 0 }
                        angle: target ? target.PathView.rotation : 0
                    }
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
                    source: thumbnailView.currentItem ? thumbnailView.currentItem.icon : ""
                    width: PlasmaCore.Units.iconSizes.large
                    height: width
                    Layout.alignment: Qt.AlignCenter
                }

                PC3.Label {
                    font.bold: true
                    font.pointSize: Math.round(PlasmaCore.Theme.defaultFont.pointSize * 1.6)
                    text: thumbnailView.currentItem ? thumbnailView.currentItem.caption : ""
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
        } else if (currentIndex === (thumbnailView.count - 1) && thumbnailView.currentIndex === 0) {
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
