/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.plasmoid 2.0

import org.kde.plasma.private.timer 0.1 as TimerPlasmoid

Item {
    id: compactRepresentation

    Layout.minimumHeight: root.inPanel ? Layout.preferredHeight : undefined

    Layout.preferredWidth: {
        switch (layoutForm) {
        case 0:
        case 2:
            return iconItem.width + column.width;
        case 1:
        case 3:
            return parent.width;
        case 4:
            return undefined;
        }
    }
    Layout.preferredHeight: {
        switch (layoutForm) {
        case 0:
        case 2:
            return parent.height;
        case 1:
        case 3:
            return iconItem.height + column.height;
        case 4:
            return undefined;
        }
    }

    readonly property int layoutForm: {
        if (root.inPanel) {
            return root.isVertical ? 1 : 0;
        }
        if (compactRepresentation.parent.width - Math.min(parent.width, parent.height) >= remainingTimeLabel.contentWidth) {
            return 2; // Horizontal
        }
        if (compactRepresentation.parent.height - iconItem.height >= remainingTimeLabel.contentHeight) {
            return 3; // Vertical
        }
        return 4; // Icon-only
    }

    function adjustSecond(value) {
        if (value > 5) {
            if (root.seconds + 1 < 24*60*60) {
                root.seconds += 1;
            }
        } else if (value < -5) {
            if (root.seconds - 1 >= 0) {
                root.seconds -= 1;
            }
        }
    }

    Keys.onUpPressed: adjustSecond(10)
    Keys.onDownPressed: adjustSecond(-10)

    WheelHandler {
        enabled: !root.running
        onWheel: {
            event.accepted = true;
            compactRepresentation.adjustSecond(event.angleDelta.y);
        }
    }

    PlasmaComponents3.ToolButton {
        id: iconItem

        anchors.left: parent.left
        anchors.horizontalCenter: compactRepresentation.layoutForm >= 3 ? parent.horizontalCenter : undefined
        width: Math.min(compactRepresentation.parent.width, compactRepresentation.parent.height)
        height: width

        display: PlasmaComponents3.AbstractButton.IconOnly
        icon.name: {
            if (root.running) {
                return "chronometer-start";
            }
            return root.seconds > 0 ? "chronometer-pause" : "chronometer";
        }
        text: root.running ? i18nc("@action:button", "Pause Timer") : i18nc("@action:button", "Start Timer")

        onClicked: root.toggleTimer()
    }

    ColumnLayout {
        id: column

        anchors.left: layoutForm === 0 || layoutForm === 2 ? iconItem.right : parent.left
        anchors.top: layoutForm === 0 || layoutForm === 2 ? parent.top : iconItem.bottom
        width: {
            switch (compactRepresentation.layoutForm) {
            case 0:
                return Math.min(implicitWidth, PlasmaCore.Units.gridUnit * 10);
            case 2:
                return Math.min(implicitWidth, compactRepresentation.parent.width - iconItem.width);
            case 1:
            case 3:
                return compactRepresentation.parent.width;
            case 4:
                return 0;
            }
        }
        height: {
            switch (compactRepresentation.layoutForm) {
            case 0:
            case 2:
                return compactRepresentation.parent.height;
            case 1:
            case 3:
                return implicitHeight;
            case 4:
                return 0;
            }
        }

        // Use opacity not visible to make contentHeight work
        opacity: compactRepresentation.layoutForm !== 4 ? 1 : 0
        spacing: 0

        PlasmaComponents3.Label {
            id: titleLabel

            Layout.fillWidth: true
            visible: root.showTitle && root.title !== ""

            elide: Text.ElideRight
            font.bold: remainingTimeLabel.font.bold
            fontSizeMode: remainingTimeLabel.fontSizeMode
            horizontalAlignment: remainingTimeLabel.horizontalAlignment
            minimumPointSize: remainingTimeLabel.minimumPointSize
            text: root.title
        }

        PlasmaComponents3.Label {
            id: remainingTimeLabel

            Layout.fillWidth: true

            activeFocusOnTab: true
            elide: Text.ElideRight
            font.bold: root.alertMode
            fontSizeMode: layoutForm === 0 || layoutForm === 2 ? Text.VerticalFit : Text.HorizontalFit
            horizontalAlignment: layoutForm === 0 || layoutForm === 2 ? Text.AlignJustify : Text.AlignHCenter
            minimumPointSize: PlasmaCore.Theme.smallestFont.pointSize

            text: {
                if (root.isVertical) {
                    return i18ncp("remaining time", "%1s", "%1s", root.seconds);
                }

                return root.showSeconds ? TimerPlasmoid.Timer.secondsToString(root.seconds, "hh:mm:ss") : TimerPlasmoid.Timer.secondsToString(root.seconds, "hh:mm");
            }

            Accessible.name: Plasmoid.toolTipMainText
            Accessible.description: Plasmoid.toolTipSubText
            Accessible.role: Accessible.Button
        }

        TapHandler {
            acceptedButtons: Qt.LeftButton
            onTapped: Plasmoid.expanded = !Plasmoid.expanded
        }
    }
}
