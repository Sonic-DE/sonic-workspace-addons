/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2

import org.kde.plasma.components 3.0 as PlasmaComponents3 // For TextArea
import org.kde.kirigami 2.15 as Kirigami

QQC2.ScrollView {
    id: bodyTextContainer

    property alias text: bodyText.text
    property alias bold: bodyText.font.bold

    // Eliminate binding loop warnings
    contentWidth: parent.implicitWidth
    contentHeight: Math.min(bodyText.implicitHeight, Kirigami.Units.gridUnit * 5)

    // HACK: workaround for https://bugreports.qt.io/browse/QTBUG-83890
    QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

    Keys.onMenuPressed: contextMenu.popup(bodyText, bodyText.x, bodyText.y);
    onFocusChanged: if (!focus) bodyText.deselect()

    PlasmaComponents3.TextArea {
        id: bodyText
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        background: Item {}
        color: Kirigami.Theme.textColor

        persistentSelection: true
        selectByMouse: !Kirigami.Settings.tabletMode
        readOnly: true
        wrapMode: Text.Wrap
        textFormat: TextEdit.PlainText

        HoverHandler {
            cursorShape: Qt.IBeamCursor
        }

        TapHandler {
            acceptedButtons: Qt.RightButton
            onTapped: {
                contextMenu.popup(bodyText);
            }
        }

        TapHandler {
            acceptedDevices: PointerDevice.TouchScreen | PointerDevice.Stylus
            onLongPressed: {
                contextMenu.popup(bodyText);
            }
        }

        ActionContextMenu {
            id: contextMenu
            actions: [
                Kirigami.Action {
                    iconName: "edit-copy"
                    enabled: bodyText.selectedText.length > 0
                    text: i18ndc("plasma_wallpaper_org.kde.potd", "@action:inmenu", "Copy")
                    shortcut: StandardKey.Copy
                    onTriggered: bodyText.copy()
                },
                Kirigami.Action {
                    iconName: "edit-select-all"
                    enabled: true
                    text: i18ndc("plasma_wallpaper_org.kde.potd", "@action:inmenu", "Select All")
                    shortcut: StandardKey.SelectAll
                    onTriggered: bodyText.selectAll()
                }
            ]
        }
    }
}
