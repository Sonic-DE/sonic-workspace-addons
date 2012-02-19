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

   ListModel {
       id: contentPaneModel

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
       id: contentPaneView
       anchors.fill: parent
       spacing: 5
       interactive: false
       clip: true

       model: contentPaneModel

       delegate: contentPaneDelegate

       highlight: PlasmaComponents.Highlight {
           hover: true
       }

       highlightMoveDuration: 250
       highlightMoveSpeed: 1
   }

   Component {
       id: contentPaneDelegate

       Item {
           height: contentPaneItem.height
           width: contentPaneView.width

           MouseArea {
               height: parent.height + 15
               anchors { left: parent.left; right: parent.right;}
               hoverEnabled: true

               onClicked: {
               }

               onEntered: {
                   contentPaneView.currentIndex = index
                   contentPaneView.highlightItem.opacity = 1
               }

               onExited: {
                   contentPaneView.highlightItem.opacity = 0
               }
           }

           PlasmaCore.FrameSvgItem {
               id: contentPaneSvg

               width: contentPaneView.width
               height: contentPaneItem.height
               visible: false

               imagePath: "widgets/viewitem"
               prefix: "hover"
           }

           ContextAreaItem {
               id: contentPaneItem

               anchors {
                   left: contentPaneSvg.left
                   horizontalCenter: contentPaneSvg.horizontalCenter
               }

               icon: model.icon
               text: model.text
           }
       }
   }
}
