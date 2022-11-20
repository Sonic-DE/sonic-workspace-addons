/*
 * SPDX-FileCopyrightText: 2018 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15

import QtQuick.Layouts 1.15

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

GridLayout {
    id: root

    property alias model: repeater.model
    property bool showNightRow: false
    readonly property int preferredIconSize: PlasmaCore.Units.iconSizes.large
    readonly property bool hasContent: model && model.length > 0

    Layout.minimumWidth: implicitWidth

    columnSpacing: PlasmaCore.Units.smallSpacing
    rowSpacing: PlasmaCore.Units.largeSpacing

    rows: showNightRow ? 2 : 1
    flow: showNightRow ? GridLayout.TopToBottom : GridLayout.LeftToRight

    Repeater {
        id: repeater

        delegate: ColumnLayout {
            // Allow to set placeholder items by leaving the data empty
            readonly property bool hasData: modelData && modelData.icon && modelData.icon.length > 0

            Layout.fillWidth: true
            Layout.preferredWidth: Math.max(preferredIconSize, periodLabel.implicitWidth)

            PlasmaComponents.Label {
                id: periodLabel
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                visible: hasData

                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                text: hasData ? modelData.period.replace(" nt", "") : ""
            }

            PlasmaCore.IconItem {
                Layout.fillWidth: true
                Layout.preferredHeight: preferredIconSize
                Layout.preferredWidth: preferredIconSize
                visible: hasData

                source: hasData ? modelData.icon : ""

                PlasmaCore.ToolTipArea {
                    id: iconToolTip
                    anchors.fill: parent
                    mainText: hasData ? modelData.condition : ""
                }
            }

            PlasmaComponents.Label {
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                horizontalAlignment: Text.AlignHCenter
                text: hasData && modelData.tempHigh || i18nc("Short for no data available", "-")
                visible: hasData && (modelData.tempHigh || !showNightRow)
            }

            PlasmaComponents.Label {
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                horizontalAlignment: Text.AlignHCenter
                text: modelData && modelData.tempLow || i18nc("Short for no data available", "-")
                visible: hasData && (modelData.tempLow || !showNightRow)
            }
        }
    }
}
