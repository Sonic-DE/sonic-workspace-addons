/*
    SPDX-FileCopyrightText: 2024

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.kcmutils as KCM
import org.kde.kirigami as Kirigami

KCM.SimpleKCM {
    id: configMenuJson

    property string cfg_menuJson
    // Filled in by the config dialog from main.xml
    property string cfg_menuJsonDefault

    MenuBuilder {
        id: menuBuilder
        anchors.fill: parent
        defaultJson: configMenuJson.cfg_menuJsonDefault

        Connections {
            target: menuBuilder
            function onJsonOutputChanged() {
                // Update configuration property when JSON changes
                // This ensures the Apply button becomes enabled
                cfg_menuJson = menuBuilder.jsonOutput;
            }
        }
        
        Component.onCompleted: {
            // Load JSON when MenuBuilder is ready
            Qt.callLater(() => {
                const jsonData = plasmoid.configuration.menuJson;
                menuBuilder.loadFromJson(jsonData || "[]");
                // Set initial value to match loaded data
                cfg_menuJson = menuBuilder.jsonOutput;
            });
        }
    }
}

