/*
 * SPDX-FileCopyrightText: 2018 Friedrich W. H. Kossebau <kossebau@kde.org>
 * SPDX-FileCopyrightText: 2023 Ismael Asensio <isma.af@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.plasma.components as PlasmaComponents
import org.kde.kirigami as Kirigami

ListView {
    id: root

    anchors.fill: parent
    anchors.rightMargin: scrollBar.visible ? scrollBar.width : 0
    interactive: scrollBar.visible

    section.property: 'type'
    section.delegate: Kirigami.Heading {
        level: 4
        width: ListView.view.width
        height: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: section == 'Warning'
            ? i18nc("@title:column weather warnings", "Warnings Issued")
            : i18nc("@title:column weather watches" ,"Watches Issued")
    }

    delegate: PlasmaComponents.Label {
        width: ListView.view.width
        horizontalAlignment: Text.AlignHCenter

        font.underline: true
        color: Kirigami.Theme.linkColor
        text: modelData.description

        TapHandler {
            cursorShape: Qt.PointingHandCursor
            onTapped: Qt.openUrlExternally(modelData.info)
        }
    }

    QQC2.ScrollBar.vertical: QQC2.ScrollBar {
        id: scrollBar
        anchors.left: parent.right
        visible: root.contentHeight > root.height
    }

    Item {
        Layout.fillHeight: true
    }
}
