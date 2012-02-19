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
   id: contextArea

   ListModel {
       id: contextAreaModel

       ListElement {
           icon: "applications-internet"
           text: "Net"
       }

       ListElement {
           icon: "preferences-system"
           text: "Settings"
       }

        ListElement {
           icon: "folder-documents"
           text: "Files"
        }

        ListElement {
           icon: "applications-other"
           text: "Apps"
       }

        ListElement {
           icon: "favorites"
           text: "Favorites"
       }
   }

   ListView {
        id: contextAreaView
        anchors.fill: parent
        spacing: 5
        interactive: false
        clip: true

        model: contextAreaModel

        delegate: contextAreaDelegate

        highlight: PlasmaComponents.Highlight {
            hover: true
        }

        highlightMoveDuration: 250
        highlightMoveSpeed: 1

        focus: true

        Keys.onDownPressed: {
            contextAreaView.incrementCurrentIndex();
        }

        Keys.onUpPressed: {
            contextAreaView.decrementCurrentIndex();
        }
    }

    Component {
        id: contextAreaDelegate

        Item {
                height: contextAreaItem.height
                width: contextAreaView.width

            MouseArea {
                height: parent.height + 15
                anchors { left: parent.left; right: parent.right;}
                hoverEnabled: true

                onClicked: {
                }

                onEntered: {
                    contextAreaView.currentIndex = index
                    contextAreaView.highlightItem.opacity = 1
                }

                onExited: {
                    contextAreaView.highlightItem.opacity = 0
                }
            }

            ContextAreaItem {
                id: contextAreaItem

                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                icon: model.icon
                text: model.text
            }
        }
    }
}
