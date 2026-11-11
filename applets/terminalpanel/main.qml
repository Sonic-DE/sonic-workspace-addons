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

// The terminal in a panel. Showing, hiding, the shortcut and the screen edge are the panel's;
// this only asks for keyboard input through its status, as the Notes widget does.
PlasmoidItem {
    id: root

    Plasmoid.backgroundHints: PlasmaCore.Types.NoBackground
    preferredRepresentation: fullRepresentation

    fullRepresentation: Item {
        id: representation

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumWidth: Plasmoid.configuration.minimumWidth
        Layout.minimumHeight: 100

        // The panel draws no background behind the text, so the color scheme's opacity and blur
        // apply there. Only a panel's window has these properties.
        readonly property rect cutout: terminal.item ? Qt.rect(Kirigami.ScenePosition.x + terminal.x + terminal.item.textArea.x, Kirigami.ScenePosition.y + terminal.y + terminal.item.textArea.y,terminal.item.textArea.width, terminal.item.textArea.height) : Qt.rect(0, 0, 0, 0)
        readonly property bool cutoutBlur: terminal.item?.blur ?? false
        property QtObject panelWindow:Window.window && "backgroundCutout" in Window.window ? Window.window : null

        function updateCutout() {
            if (panelWindow) {
                panelWindow.backgroundCutout = cutout;
                panelWindow.backgroundCutoutBlur = cutoutBlur;
            }
        }
        onCutoutChanged: updateCutout()
        onCutoutBlurChanged: updateCutout()
        onPanelWindowChanged: Qt.callLater(updateCutout)
        Component.onDestruction: {
            if (panelWindow) {
                panelWindow.backgroundCutout = Qt.rect(0, 0, 0, 0);
            }
        }

        // Clicked elsewhere: the keyboard goes back, and the panel can hide again. The terminal's
        // own menus and dialogs aren't elsewhere.
        Window.onActiveChanged: {
            if (!Window.active && Plasmoid.status === PlasmaCore.Types.AcceptingInputStatus && !terminal.item?.ownWindowOpen()) {
                Plasmoid.status = PlasmaCore.Types.ActiveStatus;
            }
        }

        Loader {
            id: terminal
            anchors.fill: parent
            // The panel's edges and rounded corners stay visible.
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            focus: true
            sourceComponent: TerminalPanelArea {
                profile: Plasmoid.configuration.profile
                toolBarsAtBottom: Plasmoid.location === PlasmaCore.Types.TopEdge
                // In a panel only: on the desktop it isn't what opening a terminal should show.
                servesNewTerminals: [PlasmaCore.Types.TopEdge, PlasmaCore.Types.BottomEdge, PlasmaCore.Types.LeftEdge, PlasmaCore.Types.RightEdge].includes(Plasmoid.location)

                // Attention brings an auto-hide panel out, and input keeps it out.
                onNewTerminalOpened: {
                    if (Plasmoid.containment.status !== PlasmaCore.Types.AcceptingInputStatus) {
                        Plasmoid.status = PlasmaCore.Types.NeedsAttentionStatus;
                        Plasmoid.status = PlasmaCore.Types.AcceptingInputStatus;
                    }
                }
                onOwnWindowsClosed: keepInput => {
                    if (representation.Window.active) {
                        return;
                    }
                    if (keepInput) {
                        Plasmoid.status = PlasmaCore.Types.NeedsAttentionStatus;
                        Plasmoid.status = PlasmaCore.Types.AcceptingInputStatus;
                    } else {
                        Plasmoid.status = PlasmaCore.Types.ActiveStatus;
                    }
                }
                onPressedInside: {
                    const window = Window.window;
                    if (window && (window.flags & Qt.WindowDoesNotAcceptFocus)) {
                        Plasmoid.status = PlasmaCore.Types.AcceptingInputStatus;
                    }
                }
                onToolBarContextMenuRequested: (item, position) => {
                    const place = item.mapToItem(representation, position);
                    toolBarMenu.open(place.x, place.y);
                }
                // The last shell ended: the panel closes, as Escape closes it.
                onFinished: {
                    Plasmoid.status = PlasmaCore.Types.PassiveStatus;
                    restart.start();
                }
            }
        }

        // The last shell ended: the panel keeps a terminal. From here, as the handler above goes
        // with what the Loader destroys.
        Timer {
            id: restart
            interval: 0
            onTriggered: {
                terminal.active = false;
                terminal.active = true;
            }
        }

        PlasmaExtras.Menu {
            id: toolBarMenu
            visualParent: representation

            PlasmaExtras.MenuItem {
                action: Plasmoid.containment.internalAction("configure")
                visible: action !== null
            }
            PlasmaExtras.MenuItem {
                text: i18nc("@action:inmenu", "Configure Terminal…")
                icon: "configure"
                onClicked: terminal.item?.configureTerminal()
            }
        }
    }
}
