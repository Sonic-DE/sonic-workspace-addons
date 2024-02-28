/*
    SPDX-FileCopyrightText: 2024 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.kwin as KWinComponents
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.extras as PlasmaExtras

Rectangle {
    color: Kirigami.Theme.backgroundColor
    focus: true

    TapHandler {
        onTapped: effect.deactivate();
    }

    PlasmaExtras.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 2
        iconName: "virtual-desktops"
        text: i18ndc("kwin_effect_cube", "@info:placeholder", "Not enough virtual desktops to display the Cube.\n Available %1, but at least 3 are required", KWinComponents.Workspace.desktops.length)
        helpfulAction: Action {
            text: i18ndc("kwin_effect_cube", "@action:button", "Add Virtual Desktop")
            icon.name: "list-add-symbolic"
            onTriggered: KWinComponents.Workspace.createDesktop(KWinComponents.Workspace.desktops.length, "")
        }
    }

    Keys.onEscapePressed: effect.deactivate();
}
