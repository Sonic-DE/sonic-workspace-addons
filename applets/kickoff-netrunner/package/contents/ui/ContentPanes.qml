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

//FIXME: i can't get it to find libqzeitgeist, even though it's installed :(
//import org.gnome.zeitgeist 0.1

Item {
    id: contentPanes

    PlasmaCore.DataSource {
        id: placesSource
        engine: "places"
        onSourceAdded: connectSource(source)
        onSourceRemoved: disconnectSource(source)

        Component.onCompleted: {
            connectedSources = sources
        }
    }

    PlasmaCore.DataModel {
        id: placesModel
        dataSource: placesSource

        Component.onCompleted: {
        }
    }

    PlasmaCore.DataSource {
        id: devicesSource
        engine: "hotplug"
        onSourceAdded: connectSource(source)
        onSourceRemoved: disconnectSource(source)

        Component.onCompleted: {
            connectedSources = sources
        }
    }

    PlasmaCore.DataModel {
        id: devicesModel
        dataSource: devicesSource

        Component.onCompleted: {
        }
    }

    PlasmaCore.DataSource {
        id: appsSource
        engine: "apps"
        onSourceAdded: connectSource(source)
        onSourceRemoved: disconnectSource(source)

        Component.onCompleted: {
            connectedSources = sources
            populateMenu("/", pane1AppsMenuModel);
        }
    }

//    PlasmaCore.DataModel {
//        id: settingsAppsModel
//        dataSource: appsSource
//
//        Component.onCompleted: {
//        }
//    }

    ListModel {
        id: pane1AppsMenuModel
    }

    /**
     * Populates the menu model when needed
     */
    function populateMenu(path, model) {
        print("SREICH POPULATE");
        var sourcesFiltered = appsSource.data[path]["entries"]

        for (var i = 0; i < sourcesFiltered.length; ++i) {
            print ("TOPLEVELMENU: " + sourcesFiltered[i]);
            print( appsSource.data[sourcesFiltered[i]].name);

            model.append(appsSource.data[sourcesFiltered[i]]);

            print ("MODEL NAME THING:" + model.get(i).name);
            print ("MODEL ICONNAME THING:" + model.get(i).iconName);
        }

        print("SREICH COMPONENT" + sourcesFiltered);
    }

    Timer {
        id: timer
        interval: 5000
        repeat: false
        running: false

        onTriggered: {
            populateMenu();
        }
    }

    states: [
        State {
            name: "Net"
            PropertyChanges {
                target: pane1
  //              model: placesModel
                iconSource: "icon"
                textSource: "name"
            }
        },

        State {
            name: "Settings"
            PropertyChanges {
                target: pane1
                model: settingsAppsModel
                iconSource: "iconName"
                textSource: "name"
            }
        },

        State {
            name: "Files"
            PropertyChanges {
                target: pane1
                model: placesModel
                iconSource: "icon"
                textSource: "name"
            }
            PropertyChanges {
                target: pane3
                model: devicesModel
                iconSource: "icon"
                textSource: "text"
            }
        },

        State {
            name: "Apps"
            PropertyChanges {
                target: pane1
                model: pane1AppsMenuModel
                iconSource: "iconName"
                textSource: "name"
            }
        },

        State {
            name: "Home"
            PropertyChanges {
                target: pane1
//                model: placesModel
                iconSource: "icon"
                textSource: "name"
            }
        }
    ]

    property int paneWidth;
    ContentPane {
        id: pane1

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        width: paneWidth
        height: parent.height
    }

    ContentPane {
        id: pane2

        anchors {
            left: pane1.right
            top: parent.top
            bottom: parent.bottom
        }

        width: paneWidth
        height: parent.height
    }

    ContentPane {
        id: pane3

        anchors {
            left: pane2.right
            top: parent.top
            bottom: parent.bottom
        }

        width: paneWidth
        height: parent.height
    }
}
