/*
 *  SPDX-FileCopyrightText: 2017 David Faure <faure@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.5 as Kirigami
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.private.dict 1.0

ColumnLayout {
    id: root
    property string cfg_dictionary: ""

    DictionariesModel {
        id: dictionariesModel
    }

    Kirigami.Heading {
        Layout.fillWidth: true
        level: 2
        text: i18nc("@label:listbox", "Available dictionaries:")
    }

    ScrollView {
        Layout.fillWidth: true
        Layout.fillHeight: true
        contentWidth: listView.implicitWidth
        contentHeight: listView.implicitHeight
        Component.onCompleted: background.visible = true;

        ListView {
            id: listView
            model: dictionariesModel

            delegate: PlasmaComponents3.ItemDelegate {
                text: model.description
                highlighted: model.id == root.cfg_dictionary
                implicitWidth: root.width - Kirigami.Units.largeSpacing
                onClicked: root.cfg_dictionary = model.id
            }
        }
    }
}
