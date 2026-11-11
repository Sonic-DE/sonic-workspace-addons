/*
    SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.plasma.plasmoid
import org.sonicde.terminal.app

// The terminal of the Terminal Panel widget, in a popup from a panel button. The widget's keyboard
// shortcut opens and closes the popup.
PlasmoidItem {
    id: root

    Plasmoid.icon: "utilities-terminal"
    activationTogglesExpanded: true
    // A click elsewhere closes it, as it lets the Terminal Panel hide.
    hideOnWindowDeactivate: true

    fullRepresentation: Item {
        id: representation

        Layout.minimumWidth: Kirigami.Units.gridUnit * 20
        Layout.minimumHeight: Kirigami.Units.gridUnit * 12
        Layout.preferredWidth: Kirigami.Units.gridUnit * 40
        Layout.preferredHeight: Kirigami.Units.gridUnit * 24

        // The popup draws no background behind the text, so the color scheme's opacity and blur
        // apply there. Only popup and panel windows have these properties.
        readonly property rect cutout: terminal.item ? Qt.rect(Kirigami.ScenePosition.x + terminal.x + terminal.item.textArea.x, Kirigami.ScenePosition.y + terminal.y + terminal.item.textArea.y, terminal.item.textArea.width, terminal.item.textArea.height) : Qt.rect(0, 0, 0, 0)
        readonly property bool cutoutBlur: terminal.item?.blur ?? false
        property QtObject hostWindow: Window.window && "backgroundCutout" in Window.window ? Window.window : null

        function updateCutout() {
            if (hostWindow) {
                hostWindow.backgroundCutout = cutout;
                hostWindow.backgroundCutoutBlur = cutoutBlur;
            }
        }
        onCutoutChanged: updateCutout()
        onCutoutBlurChanged: updateCutout()
        onHostWindowChanged: Qt.callLater(updateCutout)
        Component.onDestruction: {
            if (hostWindow) {
                hostWindow.backgroundCutout = Qt.rect(0, 0, 0, 0);
            }
        }

        Loader {
            id: terminal
            anchors.fill: parent
            focus: true
            sourceComponent: TerminalPanelArea {
                profile: Plasmoid.configuration.profile
                toolBarsAtBottom: Plasmoid.location === PlasmaCore.Types.TopEdge
                // In a panel only: on the desktop it isn't what opening a terminal should show.
                servesNewTerminals: [PlasmaCore.Types.TopEdge, PlasmaCore.Types.BottomEdge, PlasmaCore.Types.LeftEdge, PlasmaCore.Types.RightEdge].includes(Plasmoid.location)

                onNewTerminalOpened: root.expanded = true
                // A dialog of the terminal took the focus and the popup closed: back, unless
                // another program has the focus now.
                onOwnWindowsClosed: keepInput => {
                    if (keepInput) {
                        root.expanded = true;
                    }
                }
                onToolBarContextMenuRequested: (item, position) => {
                    const place = item.mapToItem(representation, position);
                    toolBarMenu.open(place.x, place.y);
                }
                // The last shell ended: the popup closes.
                onFinished: {
                    root.expanded = false;
                    restart.start();
                }
            }
        }

        // The last shell ended: the widget keeps a terminal. From here, as the handler above goes
        // with what the Loader destroys.
        Timer {
            id: restart
            interval: 0
            onTriggered: {
                terminal.active = false;
                terminal.active = true;
            }
        }

        // The widget's own settings are in the panel button's menu, not here.
        PlasmaExtras.Menu {
            id: toolBarMenu
            visualParent: representation

            PlasmaExtras.MenuItem {
                text: i18nc("@action:inmenu", "Configure Terminal…")
                icon: "configure"
                onClicked: terminal.item?.configureTerminal()
            }
        }
    }
}
