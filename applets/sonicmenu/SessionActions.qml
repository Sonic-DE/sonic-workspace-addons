/*
    SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick

import org.kde.plasma.private.sessions

/**
 * Runs Sonic Menu items. An item with an "action" goes through SonicDE's
 * session management, like the Lock/Logout widget: it works with any init
 * system, honours the user's confirmation settings, and actions the system
 * can't do are reported unavailable so the menu can hide them. Any other item
 * runs its "command" through the shell.
 *
 * Replaces kMenu's Plasma5Support "executable" data engine; exec() keeps that
 * engine's name so the menu code calls it the same way.
 */
QtObject {
    id: root

    // Action name in the menu JSON → session slot and the property telling whether it can run.
    // The label is what the menu editor offers.
    readonly property var actions: ({
        "lock": { slot: "lock", available: "canLock", label: i18nc("@item:inlistbox session action", "Lock Screen") },
        "logout": { slot: "requestLogout", available: "canLogout", label: i18nc("@item:inlistbox session action", "Log Out") },
        "leave": { slot: "requestLogoutPrompt", available: "canLogout", label: i18nc("@item:inlistbox session action", "Leave… (all options)") },
        "switchUser": { slot: "switchUser", available: "canSwitchUser", label: i18nc("@item:inlistbox session action", "Switch User") },
        "suspend": { slot: "suspend", available: "canSuspend", label: i18nc("@item:inlistbox session action", "Sleep") },
        "hibernate": { slot: "hibernate", available: "canHibernate", label: i18nc("@item:inlistbox session action", "Hibernate") },
        "reboot": { slot: "requestReboot", available: "canReboot", label: i18nc("@item:inlistbox session action", "Restart") },
        "shutdown": { slot: "requestShutdown", available: "canShutdown", label: i18nc("@item:inlistbox session action", "Shut Down") },
    })

    // Emitted when what the system can do changes, so menus can be rebuilt
    signal availabilityChanged()

    property SessionManagement session: SessionManagement {
        onCanLockChanged: root.availabilityChanged()
        onCanLogoutChanged: root.availabilityChanged()
        onCanSwitchUserChanged: root.availabilityChanged()
        onCanSuspendChanged: root.availabilityChanged()
        onCanHibernateChanged: root.availabilityChanged()
        onCanRebootChanged: root.availabilityChanged()
        onCanShutdownChanged: root.availabilityChanged()
    }

    // True for a known action this system can do
    function available(action) {
        const entry = actions[action];
        return entry !== undefined && Boolean(session[entry.available]);
    }

    // Whether a JSON menu item belongs in the menu: items without an action always do
    function shown(item) {
        return !item || !item.action || available(item.action);
    }

    // Runs a menu item; true if something was started
    function run(item) {
        if (!item) {
            return false;
        }
        if (item.action) {
            if (!available(item.action)) {
                console.warn("Sonic Menu: session action not available:", item.action);
                return false;
            }
            session[actions[item.action].slot]();
            return true;
        }
        return exec(item.command);
    }

    function exec(command) {
        return command ? CommandRunner.run(String(command)) : false;
    }
}
