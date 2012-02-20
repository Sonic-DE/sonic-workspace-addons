/*****************************************************************************
*   Copyright (C) Shaun Reich <shaun.reich@kdemail.net>                      *
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
    id: contentPane

    property alias model: contentPaneView.model
    property string subSource: ""
    property string iconSource: ""
    property string textSource: ""
    property string selectedText: ""

    ListView {
        id: contentPaneView

        anchors {
            left: parent.left
            right: scrollBar.left
            top: parent.top
            bottom: parent.bottom
        }

        spacing: 5
        interactive: true
        clip: true

        delegate: contentPaneDelegate

        highlight: PlasmaComponents.Highlight {
            hover: true
        }

        highlightMoveDuration: 250
        highlightMoveSpeed: 1
    }

    PlasmaComponents.ScrollBar {
        id: scrollBar

        anchors { top: contentPaneView.top; bottom: contentPaneView.bottom; right: parent.right }

        orientation: Qt.Vertical
        stepSize: contentPaneView.count / 4
        scrollButtonInterval: contentPaneView.count / 4

        flickableItem: contentPaneView
    }

    Component {
       id: contentPaneDelegate

        Item {
            height: 40
            width: parent.width

            MouseArea {
//                height: parent.height + 15
 //               anchors { left: parent.left; right: parent.right; }
                anchors { left: parent.left; right: parent.right; bottom: parent.bottom; top: parent.top }
 //width: 100
// height: 100
                hoverEnabled: true

                onClicked: {
                   selectedText = model[textSource];
                   print("SELECTEDITEM: " + selectedText);
                }

                onEntered: {
                    contentPaneView.currentIndex = index
                    contentPaneView.highlightItem.opacity = 1
                }

                onExited: {
                    contentPaneView.highlightItem.opacity = 1
                }
            }

            QIconItem {
                id: contentPaneIcon

                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }

                width: 22
                height: 22

                icon: subSource == "" ? model[iconSource] : model[subSource][iconSource]
            }

            PlasmaComponents.Label {
                id: contentPaneText

                anchors {
                    left: contentPaneIcon.right
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }

                text: subSource == "" ? model[textSource] : model[subSource][textSource]
            }
        }
    }
}
