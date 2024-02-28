/*
    SPDX-FileCopyrightText: 2024 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kwin as KWinComponents
import org.kde.plasma.components as PlasmaComponents

Rectangle {
    color: Kirigami.Theme.backgroundColor

    TapHandler {
        onTapped: effect.deactivate();
    }

    ColumnLayout {
        anchors.centerIn: parent

        Kirigami.PlaceholderMessage {
            text: i18ndc("kwin_effect_cube", "@info:placeholder", "Not enough virtual desktops to display the Cube.\n Available %1, but at least 3 are required", KWinComponents.Workspace.desktops.length)
            icon.name: "virtual-desktops-symbolic"
        }

        PlasmaComponents.Button {
            Layout.alignment: Qt.AlignHCenter
            text: i18ndc("kwin_effect_cube", "@action:button", "Add Virtual Desktop")
            icon.name: "list-add-symbolic"
            onClicked: KWinComponents.Workspace.createDesktop(KWinComponents.Workspace.desktops.length, "")
        }
    }
}
