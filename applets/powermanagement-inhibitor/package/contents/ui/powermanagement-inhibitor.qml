/*****************************************************************************
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
   id: konsoleProfiles

//    property int minimumWidth: 200
 //   property int minimumHeight: 300

    PlasmaCore.DataSource {
        id: profilesSource
        engine: "powermanagement"
    }

    Component.onCompleted: {
  //      plasmoid.popupIcon = "utilities-terminal";
   //     plasmoid.aspectRatioMode = IgnoreAspectRatio;
    }

    QIconItem {
        id: actionIcon
        icon: QIcon ("preferences-system-power-management")

        anchors.fill: parent

//        height: actionsList.actionIconHeight
 //       width: actionsList.actionIconHeight
    }


//
//    PlasmaComponents.Label {
//        id: header
//        text: i18n("Konsole Profiles")
//        anchors { top: parent.top; left: parent.left; right: parent.right }
//        horizontalAlignment: Text.AlignHCenter
//    }
//
//   PlasmaCore.Svg {
//       id: lineSvg
//       imagePath: "widgets/line"
//    }
//
//    PlasmaCore.SvgItem {
//        id: separator
//
//        anchors { top: header.bottom; left: parent.left; right: parent.right }
//        anchors { topMargin: 3 }
//
//        svg: lineSvg
//        elementId: "horizontal-line"
//        height: lineSvg.elementSize("horizontal-line").height
//    }
//
//    //we use this to compute a fixed height for the items, and also to implement
//    //the said constant below (itemHeight)
//    Text {
//        id: heightMetric
//        visible: false
//        text: "Arbitrary String"
//    }
}
