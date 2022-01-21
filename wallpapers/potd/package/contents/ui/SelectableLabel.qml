/*
 *   SPDX-FileCopyrightText: 2011 Marco Martin <notmart@gmail.com>
 *   SPDX-FileCopyrightText: 2014, 2019 Kai Uwe Broulik <kde@privat.broulik.de>
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.plasma.components 2.0 as PlasmaComponents // For ContextMenu
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.kirigami 2.15 as Kirigami

PlasmaComponents3.ScrollView {
    id: bodyTextContainer

    property alias text: bodyText.text
    property alias bold: bodyText.font.bold

    property QtObject contextMenu: null

    implicitHeight: Math.min(bodyText.implicitHeight, Kirigami.Units.gridUnit * 5)
    Layout.fillWidth: true

    // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
    PlasmaComponents3.ScrollBar.horizontal.policy: PlasmaComponents3.ScrollBar.AlwaysOff

    PlasmaComponents3.TextArea {
        id: bodyText
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        background: Item {}
        color: Kirigami.Theme.textColor

        selectByMouse: !Kirigami.Settings.tabletMode
        readOnly: true
        wrapMode: Text.Wrap
        textFormat: TextEdit.PlainText

        MouseArea {
            anchors.fill: parent

            acceptedButtons: Qt.RightButton
            cursorShape: Qt.IBeamCursor

            onPressed: {
                contextMenu = contextMenuComponent.createObject(bodyText);

                contextMenu.closed.connect(function() {
                    contextMenu.destroy();
                    contextMenu = null;
                });
                contextMenu.open(mouse.x, mouse.y);
            }
        }
    }

    Component {
        id: contextMenuComponent

        EditContextMenu {
            target: bodyText
        }
    }
}
