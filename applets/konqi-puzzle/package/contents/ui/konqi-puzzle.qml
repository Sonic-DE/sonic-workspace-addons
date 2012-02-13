/******************************************************************************
 *   Copyright (C) 2012 by Shaun Reich <shaun.reich@kdemail.net>              *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

import QtQuick 1.1
import org.kde.qtextracomponents 0.1

import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: konqiPuzzle

    ListModel {
        id: puzzleModel

        ListElement {
            text: "1"
        }
        ListElement {
            text: "2"
        }
        ListElement {
            text: "3"
        }
        ListElement {
            text: "4"
        }
        ListElement {
            text: "5"
        }
        ListElement {
            text: "6"
        }
        ListElement {
            text: "7"
        }
        ListElement {
            text: "8"
        }
        ListElement {
            text: "9"
        }
        ListElement {
            text: "10"
        }
        ListElement {
            text: "11"
        }
        ListElement {
            text: "12"
        }
        ListElement {
            text: "13"
        }
        ListElement {
            text: "14"
        }
        ListElement {
            text: "15"
        }
    }

    GridView {
        width: 300
        height: 300

        model: puzzleModel

        interactive: false

        delegate: puzzleDelegate
    }

    Component {
        id: puzzleDelegate

        Item {
            width: 80
            height: 80

            Rectangle {
                anchors.fill: parent
                color: "lightgreen"

                Text {
                    anchors.fill: parent
                    text: model.text
                }
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    console.log("CLICKED!")
                    puzzleModel.move(model.index, model.index-1, 1);
                }
            }
        }
    }
}

//    Rectangle {
//        width: 400; height: 400; color: "black"
//
//        Grid {
//            x: 5
//            y: 5
//
//            rows: 5
//            columns: 5
//
//            spacing: 10
//
//            Repeater {
//                id: repeater
//                model: 25
//
//                Rectangle {
//                    width: 70
//                    height: 70
//                    color: "lightgreen"
//
//                    Text {
//                        // so we don't get 0
//                        text: index + 1
//                        font.pointSize: 30
//                        anchors.centerIn: parent
//                    }
//
//                    MouseArea {
//                        anchors.fill: parent
//
//                        onClicked: {
//                            console.log("CLICKED!")
//                            puzzleModel.move(0, 2, 2)
//                        }
//                    }
//                }
//            }
//        }
//    }
