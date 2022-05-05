/*
 *  SPDX-FileCopyrightText: 2017 David Faure <faure@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.5
import org.kde.kirigami 2.5 as Kirigami
import org.kde.plasma.private.dict 1.0

Page {
    id: root
    property string cfg_dictionary: ""

    DictionariesModel {
        id: dictionariesModel
    }

    header: Kirigami.Heading {
        level: 2
        text: i18nc("@label:listbox", "Available dictionaries:")
    }

    ScrollView {
        anchors {
            fill: parent
            topMargin: Kirigami.Units.largeSpacing
        }
        Component.onCompleted: background.visible = true;

        ListView {
            id: listView
            model: dictionariesModel

            delegate: Control {

                background: Rectangle {
                    id: highlight
                    implicitWidth: root.width - Kirigami.Units.largeSpacing
                    visible: model.id == root.cfg_dictionary
                    color: Kirigami.Theme.highlightColor
                }

                contentItem: Label {
                    id: pathText
                    width: listView.availableWidth
                    text: model.description
                    color: model.id == root.cfg_dictionary ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.cfg_dictionary = model.id
                }
            }
        }
    }
}
