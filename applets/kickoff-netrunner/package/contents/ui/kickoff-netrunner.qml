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

    property int minimumWidth: 200
    property int minimumHeight: 300

    PlasmaCore.DataSource {
        id: appsSource
        engine: "apps"
        onSourceAdded: connectSource(source)
        onSourceRemoved: disconnectSource(source)

        Component.onCompleted: {
            connectedSources = sources
        }
    }

    PlasmaCore.DataModel {
        id: appsModel
        dataSource: appsSource
    }

    Component.onCompleted: {
        plasmoid.popupIcon = "start-here-kde";
        plasmoid.aspectRatioMode = IgnoreAspectRatio;
    }

    PlasmaComponents.TextField {
        id: searchField

        anchors {
            left: contextArea.right
            top: parent.top
            right: parent.right
        }

        clearButtonShown: true
    }

    ContextArea {
        id: contextArea
        width: 64

        anchors {
            left: parent.left
            top: parent.top
            bottom: leaveArea.top
        }
    }

    ContentPanes {
        id: contentPanes

        //FIXME HACK: why in the *hell* do i do this and it works
        //but if i do parent.width /3 , where width is 249, it fucking doesn't..argh
        paneWidth: 249

        anchors {
            left: contextArea.right
            top: searchField.bottom
            bottom: leaveArea.top
            right: parent.right
        }

        onAppToRunChanged: {
            plasmoid.runApplication(appToRun);
        }

        state: contextArea.selectedItem
    }

    LeaveArea {
        id: leaveArea

        //really HACK, because Row doesn't seem to want to be horiz centered
        //interesting..means there's got to be a layout bug inside LeaveArea i think
        x: (searchField.width / 2) - (width / 2) + 35
        anchors {
        // doesn't work -_-
        //    left: searchField.left;
        //    right: searchField.right;
            bottom: parent.bottom

            //FIXME hardcoded, plasma comp bug?
            bottomMargin: 25
        }
    }



//    Text {
//        id: textMetric
//        visible: false
//        // translated but not used, we just need length/height
//        text: i18n("Arbitrary String Which Says The Dictionary Type")
//

}
