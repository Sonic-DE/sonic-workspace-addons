/*
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2

import org.kde.kirigami 2.15 as Kirigami

QQC2.ScrollView {
    id: bodyTextContainer

    property alias text: bodyText.text
    property alias horizontalAlignment: bodyText.horizontalAlignment
    property alias bold: bodyText.font.bold
    property alias fontPointSize: bodyText.font.pointSize

    contentHeight: Math.min(bodyText.implicitHeight, Kirigami.Units.gridUnit * 5)

    QQC2.TextArea {
        id: bodyText
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        background: Item {}

        readOnly: true
        wrapMode: Text.Wrap
        textFormat: TextEdit.PlainText

        HoverHandler {
            cursorShape: Qt.IBeamCursor
        }
    }
}
