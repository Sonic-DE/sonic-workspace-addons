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
   id: leaveArea

    PlasmaCore.DataSource {
       id: powerSource
       engine: "powermanagement"

    }

        width: lock.width + leave.width + switchuser.width
    Row {
        id: row
        anchors.fill: parent

        PlasmaComponents.Button {
            id: lock

            text: i18n("Lock Session")
            iconSource: "system-lock-screen"

            onClicked: {
                var service = powerSource.serviceForSource("PowerDevil");
                var operation = service.operationDescription("lockScreen");
                var job = service.startOperationCall(operation);
            }
        }

        PlasmaComponents.Button {
            id: leave

            text: i18n("Leave...")
            iconSource: "system-shutdown"

            onClicked: {
                var service = powerSource.serviceForSource("PowerDevil");
                var operation = service.operationDescription("lockScreen");
                var job = service.startOperationCall(operation);
            }
        }

        PlasmaComponents.Button {
            id: switchuser

            text: i18n("Switch User")
            iconSource: "system-switch-user"

            onClicked: {
                var service = powerSource.serviceForSource("PowerDevil");
                var operation = service.operationDescription("lockScreen");
                var job = service.startOperationCall(operation);
            }
        }
    }
}
