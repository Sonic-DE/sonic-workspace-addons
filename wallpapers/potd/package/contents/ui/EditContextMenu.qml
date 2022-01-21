/*
 *   SPDX-FileCopyrightText: 2019 Kai Uwe Broulik <kde@privat.broulik.de>
 *   SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15

import org.kde.plasma.components 2.0 as PlasmaComponents // For ContextMenu

import org.kde.kquickcontrolsaddons 2.0 as KQCAddons

PlasmaComponents.ContextMenu {
    id: contextMenu

    signal closed

    property QtObject __clipboard: KQCAddons.Clipboard { }

    // can be a Text or TextEdit
    property Item target

    onStatusChanged: {
        if (status === PlasmaComponents.DialogStatus.Closed) {
            closed();
        }
    }

    PlasmaComponents.MenuItem {
        text: i18ndc("plasma_wallpaper_org.kde.potd", "@action:inmenu", "Copy")
        icon: "edit-copy"
        enabled: typeof target.selectionStart !== "undefined"
               ? target.selectionStart !== target.selectionEnd
               : (target.text || "").length > 0
        onClicked: {
            if (typeof target.copy === "function") {
                target.copy();
            } else {
                __clipboard.content = target.text;
            }
        }
    }

    PlasmaComponents.MenuItem {
        id: selectAllAction
        icon: "edit-select-all"
        text: i18ndc("plasma_wallpaper_org.kde.potd", "@action:inmenu", "Select All")
        onClicked: target.selectAll()
        visible: typeof target.selectAll === "function"
    }
}
