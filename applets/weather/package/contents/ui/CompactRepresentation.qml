/*
 * SPDX-FileCopyrightText: 2018 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick

import QtQuick.Layouts

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami
import org.kde.plasma.workspace.components as WorkspaceComponents

Loader {
    id: compactRoot

    property var generalModel
    property var observationModel

    readonly property bool vertical: (Plasmoid.formFactor == PlasmaCore.Types.Vertical)
    readonly property bool showTemperature: Plasmoid.configuration.showTemperatureInCompactMode
    readonly property bool useBadge: Plasmoid.configuration.showTemperatureInBadge || Plasmoid.needsToBeSquare

    sourceComponent: (showTemperature && !useBadge) ? iconAndTextComponent : iconComponent
    Layout.fillWidth: compactRoot.vertical
    Layout.fillHeight: !compactRoot.vertical
    Layout.minimumWidth: item.Layout.minimumWidth
    Layout.minimumHeight: item.Layout.minimumHeight

    MouseArea {
        id: compactMouseArea
        anchors.fill: parent

        hoverEnabled: true

        onClicked: {
            root.expanded = !root.expanded;
        }
    }

    Component {
        id: iconComponent

        Kirigami.Icon {
            readonly property int minIconSize: Math.max((compactRoot.vertical ? compactRoot.width : compactRoot.height), Kirigami.Units.iconSizes.small)

            source: Plasmoid.icon
            active: compactMouseArea.containsMouse
            // reset implicit size, so layout in free dimension does not stop at the default one
            implicitWidth: Kirigami.Units.iconSizes.small
            implicitHeight: Kirigami.Units.iconSizes.small
            Layout.minimumWidth: compactRoot.vertical ? Kirigami.Units.iconSizes.small : minIconSize
            Layout.minimumHeight: compactRoot.vertical ? minIconSize : Kirigami.Units.iconSizes.small

            WorkspaceComponents.BadgeOverlay {
                anchors.bottom: parent.bottom

                visible: showTemperature && useBadge && text.length > 0

                text: observationModel.temperature
                icon: parent

                // Imperative to unset the previous anchor before setting a new one
                readonly property bool shouldCenter : width >= parent.width
                onShouldCenterChanged: positionBadge()
                Component.onCompleted: positionBadge()

                function positionBadge() : void {
                    if (shouldCenter) {
                        anchors.right = undefined
                        anchors.horizontalCenter = parent.horizontalCenter
                    } else {
                        anchors.horizontalCenter = undefined
                        anchors.right = parent.right
                    }
                }

            }
        }
    }

    Component {
        id: iconAndTextComponent

        IconAndTextItem {
            vertical: compactRoot.vertical
            iconSource: Plasmoid.icon
            active: compactMouseArea.containsMouse
            text: observationModel.temperature
        }
    }
}
