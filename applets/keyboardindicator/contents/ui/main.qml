/*
 *    SPDX-FileCopyrightText: 2018 Aleix Pol Gonzalez <aleixpol@kde.org>
 *    SPDX-FileCopyrightText: 2022 ivan tkachenko <me@ratijas.tk>
 *
 *    SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami
import org.kde.plasma.private.keyboardindicator as KeyboardIndicator
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.extras as PlasmaExtras

PlasmoidItem {
    id: root

    readonly property KeyboardIndicator.keyState capsLockState: Qt.Key_CapsLock
    readonly property KeyboardIndicator.keyState numLockState: Qt.Key_NumLock
    readonly property bool capsLocked: Plasmoid.configuration.key.includes("Caps Lock") && capsLockState.locked
    readonly property bool numLocked: Plasmoid.configuration.key.includes("Num Lock") && numLockState.locked

    Plasmoid.icon: {
        if (capsLocked && numLocked) {
            return "input-combo-on";
        } else if (capsLocked) {
            return "input-caps-on";
        } else if (numLocked) {
            return "input-num-on";
        } else {
            return "input-caps-on";
        }
    }

    // Only exists because the default CompactRepresentation doesn't expose a
    // way to mark its icon as disabled.
    // TODO remove once it gains that feature.
    compactRepresentation: MouseArea {
        id: compactMouse

        activeFocusOnTab: true
        hoverEnabled: true

        Accessible.name: Plasmoid.title
        Accessible.description: root.toolTipSubText
        Accessible.role: Accessible.Button

        property bool wasExpanded: false
        onPressed: wasExpanded = root.expanded
        onClicked: root.expanded = !wasExpanded

        Kirigami.Icon {
            anchors.fill: parent
            source: Plasmoid.icon
            active: compactMouse.containsMouse
            enabled: root.capsLocked || root.numLocked
        }
    }

    fullRepresentation: PlasmaComponents.Page {
        implicitWidth: Kirigami.Units.gridUnit * 12
        implicitHeight: Kirigami.Units.gridUnit * 12

        PlasmaExtras.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - (Kirigami.Units.gridUnit * 4)
            iconName: Plasmoid.icon
            text: root.toolTipSubText
        }
    }

    switchWidth: Kirigami.Units.gridUnit * 12
    switchHeight: Kirigami.Units.gridUnit * 12

    Plasmoid.status: root.capsLocked || root.numLocked
        ? PlasmaCore.Types.ActiveStatus
        : PlasmaCore.Types.PassiveStatus

    toolTipSubText: {
        let text = [];
        if (root.capsLocked) {
            text.push(i18n("Caps Lock activated"));
        }
        if (root.numLocked) {
            text.push(i18n("Num Lock activated"));
        }
        if (text.length > 0) {
            // Not using KUIT markup for these newline characters because those
            // get translated into HTML, and this text is displayed in the applet's
            // tooltip which does not render HTML at all for security reasons
            return text.join("\n");
        } else {
            return i18n("No lock keys activated");
        }
    }
}
