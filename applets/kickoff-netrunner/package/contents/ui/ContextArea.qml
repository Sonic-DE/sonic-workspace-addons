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


    Row {
        anchors.fill: parent

        PlasmaComponents.Button {
            text: i18n("Lock Session")
            iconSource: "system-lock-screen"
        }

        PlasmaComponents.Button {
            text: i18n("Leave...")
            iconSource: "system-shutdown"
//            textHorizontalAlignment: Text.AlignHCenter
        }

        PlasmaComponents.Button {
            text: i18n("Switch User")
            iconSource: "system-switch-user"
        }
    }
}
