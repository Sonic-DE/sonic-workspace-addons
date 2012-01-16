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
   id: main

   property bool inhibited: false
//    property int minimumWidth: 200
 //   property int minimumHeight: 300

    PlasmaCore.DataSource {
        id: pmSource
        engine: "powermanagement"
    }

    Component.onCompleted: {
  //      plasmoid.popupIcon = "utilities-terminal";
        plasmoid.aspectRatioMode = Square;
    }

    QIconItem {
        id: pmIcon
        icon: QIcon("preferences-system-power-management")

        anchors { top: parent.top; bottom: inhibitedLabel.top; left: parent.left; right: parent.right }

        smooth: true

        MouseArea {
            anchors.fill: parent

            onClicked: {
                //call service and either do or undo it
                service = pmSource.serviceForSource("PowerDevil")

                if (!inhibited) {
                    operation = service.operationDescription("inhibitScreensaver")
                    operation.reason = "Powermanagement-inhibitor Plasma Applet toggled"

                    //FIXME: track result (success/fail). forgot the JS binding for that...
                    service.startOperationCall(operation)

                    inhibited = true
                } else {
                    operation = service.operationDescription("uninhibitScreensaver")
                    operation.reason = "powermanagement-inhibitor Plasma Applet toggled via user"

                    service.startOperationCall(operation)

                    inhibited = false
                }
            }
        }
    }

    QIconItem {
        id: disabledIcon

        function sizeFactor() {
            var size = pmIcon.width

            if (size > 48) {
                // 48x48 is the max size for this icon.
                // so that's the closest we can get..
                size = 48
            }

            return size
        }

        width: sizeFactor()
        height: sizeFactor()

        anchors {
            bottom: pmIcon.bottom
            right: pmIcon.right
        }

        visible: inhibited
        icon: QIcon("dialog-cancel")
        smooth: true
    }

   PlasmaComponents.Label {
        id: inhibitedLabel
        text: inhibited ? i18n("Powersaving Disabled") : i18n("Powersaving Enabled")
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
        horizontalAlignment: Text.AlignHCenter
    }
}
