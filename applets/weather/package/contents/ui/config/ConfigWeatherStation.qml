/*
 * SPDX-FileCopyrightText: 2016, 2018 Friedrich W. H. Kossebau <kossebau@kde.org>
 * SPDX-FileCopyrightText: 2022 Ismael Asensio <isma.af@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.9
import QtQuick.Controls 2.5 as QQC2
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.5 as Kirigami
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.private.weather 1.0


ColumnLayout {
    id: weatherStationConfigPage

    property string cfg_source
    property alias cfg_updateInterval: updateIntervalSpin.value

    property var providers: Plasmoid.nativeInterface.providers

    readonly property var sourceDetails: cfg_source ? cfg_source.split('|') : ""
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

            text: hasSource ? sourceDetails[2] : i18nc("No location is currently selected", "None selected")
        }

        QQC2.Label {
            Kirigami.FormData.label: hasSource ? i18nc("@label", "Provider:") : ""

            Layout.fillWidth: true
            elide: Text.ElideRight
            // Keep it visible to avoid height changes which can confuse AppletConfigurationPage
            opacity: hasSource ? 1 : 0

            text: hasSource ? Plasmoid.nativeInterface.providers[sourceDetails[0]] : ""
        }

        Item {
            Kirigami.FormData.isSection: true
        }
    }

    WeatherStationPicker {
        id: stationPicker
        providers: Plasmoid.nativeInterface.providers
        isNewSetup: !hasSource

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.maximumHeight: weatherStationConfigPage.height - formLayout.height - 2 * Kirigami.Units.smallSpacing

        onAccepted: {
            weatherStationConfigPage.cfg_source = stationPicker.source
        }
    }
}
