/*
 *  SPDX-FileCopyrightText: 2017 David Faure <faure@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import QtWebEngine 1.1

import org.kde.plasma.private.dict 1.0

ColumnLayout {

    DictObject {
        id: dict
        selectedDictionary: plasmoid.configuration.dictionary
        // Activate the busy indicator, and deactivate it when page is loaded.
        onSearchInProgress: loadingPlaceholder.active = true;
        onDefinitionFound: web.loadHtml(html);
    }

    RowLayout {
        Layout.alignment: Qt.AlignTop
        Layout.fillWidth: true
        PlasmaExtras.SearchField {
            id: input
            placeholderText: i18nc("@info:placeholder", "Enter word to define here…")
            Layout.fillWidth: true
            Layout.minimumWidth: PlasmaCore.Units.gridUnit * 12
            onAccepted: {
                if (input.text === "") {
                    web.visible = false;
                } else {
                    web.visible = true;
                    dict.lookup(input.text);
                }
            }
        }
        PlasmaComponents3.Button {
            icon.name: "configure"
            onClicked: plasmoid.action("configure").trigger();
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.minimumHeight: input.Layout.minimumWidth

        WebEngineView {
            id: web
            anchors.fill: parent
            visible: false

            zoomFactor: PlasmaCore.Units.devicePixelRatio
            profile: dict.webProfile

            onLoadingChanged: {
                if (!loading) {
                    // Page is loaded
                    loadingPlaceholder.active = false;
                }
            }
        }

        Loader {
            id: loadingPlaceholder
            active: false
            anchors.fill: parent
            asynchronous: true
            visible: active
            sourceComponent: Rectangle {
                anchors.fill: parent
                color: web.backgroundColor

                PlasmaComponents3.BusyIndicator {
                    anchors.centerIn: parent
                }
            }
        }
    }

}
