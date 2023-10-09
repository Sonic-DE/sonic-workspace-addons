/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import org.kde.kwin as KWinComponents

KWinComponents.SceneEffect {
    id: effect

    property bool activated: false
    readonly property int animationDuration: 500

    delegate: ScreenView {}

    KWinComponents.ScreenEdgeHandler {
        enabled: true
        edge: KWinComponents.ScreenEdgeHandler.TopEdge
        onActivated: toggle();
    }

    KWinComponents.ShortcutHandler {
        name: "Cube"
        text: "Toggle Cube"
        sequence: "Meta+C"
        onActivated: toggle();
    }

    Timer {
        id: deactivateTimer
        interval: effect.animationDuration
        onTriggered: effect.visible = false
    }

    function toggle() {
        if (activated) {
            deactivate();
        } else {
            activate();
        }
    }

    function activate() {
        if (activated) {
            return;
        }
        if (KWinComponents.Workspace.desktops.length < 3) {
            return;
        }
        activated = true;
        visible = true;
    }

    function deactivate() {
        if (!activated) {
            return;
        }
        activated = false;
        deactivateTimer.start();
    }
}
