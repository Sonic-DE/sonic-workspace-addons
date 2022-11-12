/*
 * SPDX-FileCopyrightText: 2016, 2018 Friedrich W. H. Kossebau <kossebau@kde.org>
 * SPDX-FileCopyrightText: 2022 Ismael Asensio <isma.af@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15

import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami

import org.kde.plasma.private.weather 1.0


ColumnLayout {
    id: root

    property var providers

    property string currentSource
    property bool isNewSetup: true

    readonly property string source: locationListView.count ? locationListModel.valueForListIndex(locationListView.currentIndex) : ""
    readonly property bool canSearch: !!searchStringEdit.text && Object.keys(providers).length

    // The model property `isValidatingInput` doesn't account for the timer delay
    // We use a custom property to provide a more responsive feedback
    property bool isSearching: false

    signal accepted

    function searchLocation() {
        if (!canSearch) {
            locationListModel.clear();
        }
        locationListModel.searchLocations(searchStringEdit.text, Object.keys(providers));
    }

    LocationListModel {
        id: locationListModel
        onLocationSearchDone: {
            isSearching = false
            if (success) {
                // If we got any results, pre-select the top item to potentially
                // save the user a step
                locationListView.currentIndex = 0;
            }
        }
    }

    Kirigami.SearchField {
        id: searchStringEdit

        Layout.fillWidth: true
        Layout.minimumWidth: implicitWidth

        focus: true
        enabled: Object.keys(root.providers).length > 0
        placeholderText: isNewSetup ? i18nc("@info:placeholder", "Enter location") : i18nc("@info:placeholder", "Enter new location")

        Timer {
            id: searchDelayTimer
            interval: 500
            onTriggered: {
                searchLocation();
            }
        }

        onTextChanged: {
            isSearching = text.length > 0
            searchDelayTimer.restart();
        }

        Keys.onPressed: {
            if (event.key == Qt.Key_Up) {
                if (locationListView.currentIndex != 0) {
                    locationListView.currentIndex--;
                }
                event.accepted = true;
            } else if (event.key == Qt.Key_Down) {
                if (locationListView.currentIndex != locationListView.count - 1) {
                    locationListView.currentIndex++;
                }
                event.accepted = true;
            } else {
                event.accepted = false;
            }
        }
    }

    QQC2.ScrollView {
        Layout.fillWidth: true
        Layout.fillHeight: true

        enabled: Object.keys(root.providers).length > 0

        Component.onCompleted: {
            background.visible = true;
        }

        ListView {
            id: locationListView
            model: locationListModel
            clip: true
            focus: true
            activeFocusOnTab: true
            keyNavigationEnabled: true

            delegate: QQC2.ItemDelegate {
                width: locationListView.width
                text: model.display
                highlighted: ListView.isCurrentItem

                onClicked: {
                    locationListView.forceActiveFocus();
                    locationListView.currentIndex = index;
                }

                onDoubleClicked: {
                    root.accepted()
                }
            }

            Kirigami.PlaceholderMessage {
                id: listViewPlaceholder
                anchors.centerIn: parent
                width: parent.width - Kirigami.Units.gridUnit
                visible: locationListView.count === 0 && !isSearching
                text: {
                    if (canSearch) {    // There is a search text
                        return i18nc("@info", "No weather stations found for '%1'", searchStringEdit.text);
                    } else if (isNewSetup) {
                        return i18nc("@info", "Search for a weather station to set your location");
                    } else {
                        return i18nc("@info", "Search for a weather station to change your location");
                    }
                }

            }

            QQC2.BusyIndicator {
                id: busy
                anchors.centerIn: parent
                visible: locationListView.count === 0 && isSearching
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        Item {
            Layout.fillWidth: true
        }

        QQC2.Button {
            id: selectButton
            enabled: root.source.length > 0 && root.source !== root.currentSource
            icon.name: "dialog-ok"
            text: i18nc("@action:button", "Select Location")
            onClicked: root.accepted()
        }
    }

    Component.onCompleted: {
        searchStringEdit.forceActiveFocus();
    }
}
