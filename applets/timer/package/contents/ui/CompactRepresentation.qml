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

MouseArea {
    id: compactRepresentation

    Layout.preferredWidth: {
        switch (layoutForm) {
        case 0:
        case 2:
            return iconItem.width + column.width;
        case 1:
        case 3:
            return parent.width;
        }
    }
    Layout.preferredHeight: {
        switch (layoutForm) {
        case 0:
        case 2:
            return parent.height;
        case 1:
            return parent.width;
        case 3:
            return iconItem.height + column.height
        }
    }

    readonly property int layoutForm: {
        if (root.inPanel) {
            return root.isVertical ? 1 : 0;
        }
        if (compactRepresentation.width - Math.min(parent.width, parent.height) >= remainingTimeLabel.contentWidth) {
            return 2; // Horizontal
        }
        return 3; // Vertical
    }

    acceptedButtons: Qt.LeftButton

    onClicked: Plasmoid.expanded = !Plasmoid.expanded

    PlasmaCore.IconItem {
        id: iconItem

        anchors.left: layoutForm === 3 ? undefined : parent.left
        anchors.horizontalCenter: layoutForm === 3 ? parent.horizontalCenter : undefined
        width: Math.min(compactRepresentation.parent.width, compactRepresentation.parent.height)
        height: width
        visible: !root.isVertical

        source: "chronometer"
    }

    ColumnLayout {
        id: column

        anchors.left: layoutForm === 0 || layoutForm === 2 ? iconItem.right : parent.left
        anchors.top: {
            switch (compactRepresentation.layoutForm) {
            case 0:
            case 1:
            case 2:
                return parent.top;
            case 3:
                return iconItem.bottom;
            }
        }
        width: {
            switch (compactRepresentation.layoutForm) {
            case 0:
                return Math.min(implicitWidth, PlasmaCore.Units.gridUnit * 10)
            case 2:
                return Math.min(implicitWidth, parent.parent.width - iconItem.width);
            case 1:
            case 3:
                return parent.width;
            }
        }
        height: {
            switch (compactRepresentation.layoutForm) {
            case 0:
            case 2:
                return parent.height;
            case 1:
            case 3:
                return implicitHeight;
            }
        }

        spacing: 0

        PlasmaComponents3.Label {
            id: titleLabel

            Layout.fillWidth: true
            visible: root.showTitle && root.title !== ""

            elide: Text.ElideRight
            font.bold: remainingTimeLabel.font.bold
            fontSizeMode: remainingTimeLabel.fontSizeMode
            horizontalAlignment: remainingTimeLabel.horizontalAlignment
            text: root.title
        }

        PlasmaComponents3.Label {
            id: remainingTimeLabel

            Layout.fillWidth: true

            elide: Text.ElideRight
            font.bold: root.alertMode
            horizontalAlignment: layoutForm === 1 || layoutForm === 3 ? Text.AlignHCenter : Text.AlignJustify
            text: {
                if (root.isVertical) {
                    return i18ncp("remaining time", "%1s", "%1s", root.seconds);
                }

                return root.showSeconds ? TimerPlasmoid.Timer.secondsToString(root.seconds, "hh:mm:ss") : TimerPlasmoid.Timer.secondsToString(root.seconds, "hh:mm");
            }
        }
    }
}
