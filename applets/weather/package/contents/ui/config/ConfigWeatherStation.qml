/*
 * SPDX-FileCopyrightText: 2016, 2018 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.9
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.5 as Kirigami
import org.kde.plasma.private.weather 1.0


ColumnLayout {
    id: weatherStationConfigPage

    property string cfg_source
    property alias cfg_updateInterval: updateIntervalSpin.value

    property var providers: plasmoid.nativeInterface.providers

    readonly property var sourceDetails: cfg_source.split('|')
    readonly property bool hasSource: sourceDetails.length > 2

    Kirigami.FormLayout {
        id: formLayout

        QQC2.SpinBox {
            id: updateIntervalSpin

            Kirigami.FormData.label: i18nc("@label:spinbox", "Update every:")

            textFromValue: function(value) {
                return (i18np("%1 minute", "%1 minutes", value));
            }
            valueFromText: function(text) {
                return parseInt(text);
            }

            from: 30
            to: 3600
            editable: true
        }

        QQC2.Label {
            Kirigami.FormData.label: i18nc("@label", "Location:")

            Layout.fillWidth: true
            elide: Text.ElideRight
            opacity: hasSource ? 1 : 0.7

            text: {
                if (hasSource) {
                    return i18nc("A weather station location and the weather service it comes from",
                                    "%1 (%2)", sourceDetails[2], plasmoid.nativeInterface.providers[sourceDetails[0]]);
                }
                return i18nc("No location is currently selected", "None selected")
            }
        }

        Item {
            Kirigami.FormData.isSection: true
        }
    }

    WeatherStationPicker {
        id: stationPicker
        providers: plasmoid.nativeInterface.providers
        isNewSetup: !hasSource

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.maximumHeight: weatherStationConfigPage.height - selectButtonRow.height - formLayout.height - 4 * Kirigami.Units.smallSpacing

        onAccepted: {
            weatherStationConfigPage.cfg_source = stationPicker.source
        }
    }

    RowLayout {
        id: selectButtonRow

        Layout.fillWidth: true
        Item {
            Layout.fillWidth: true
        }

        QQC2.Button {
            enabled: stationPicker.source.length > 0 && stationPicker.source !== weatherStationConfigPage.source
            icon.name: "dialog-ok"
            text: i18nc("@action:button", "Select Location")
            onClicked: stationPicker.accepted()
        }
    }
}
