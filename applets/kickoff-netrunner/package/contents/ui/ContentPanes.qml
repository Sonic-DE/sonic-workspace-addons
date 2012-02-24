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

    property string pane1CurrentSelection;
    property string pane2CurrentSelection;
    property string pane3CurrentSelection;
    property string appToRun: "";

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
            startupTimer.running = true
        }
    }

    Timer {
        id: startupTimer
        running: false
        repeat: false
        interval: 500
        onTriggered: {
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

    ListModel {
        id: pane2AppsMenuModel
    }

    ListModel {
        id: pane3AppsMenuModel
    }

    /**
     * Populates the menu model when needed
     */
    function populateMenu(path, model) {
        print("SREICH POPULATE, looking up path: " + path);
        var sourcesFiltered = appsSource.data[path]["entries"]
        print("SREICH SOURCESFILTERED: " + sourcesFiltered);

        for (var i = 0; i < sourcesFiltered.length; ++i) {
            if (sourcesFiltered[i] != "---") {
                if (sourcesFiltered[i].name != ".hidden") {
                    print( appsSource.data[sourcesFiltered[i]].name);

                    model.append({ "mainModel": appsSource.data[sourcesFiltered[i]], "canonicalName": sourcesFiltered[i] });
                }
            }
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
                subSource: ""
            }
        },

        State {
            name: "Settings"
            PropertyChanges {
                target: pane1
                model: settingsAppsModel
                iconSource: "iconName"
                textSource: "name"
                subSource: ""
            }
        },

        State {
            name: "Files"
            PropertyChanges {
                target: pane1
                model: placesModel
                iconSource: "icon"
                textSource: "name"
                subSource: ""
            }

            PropertyChanges {
                target: pane3
                model: devicesModel
                iconSource: "icon"
                textSource: "text"
                subSource: ""
            }
        },

        State {
            name: "Apps"
            PropertyChanges {
                target: pane1
                model: pane1AppsMenuModel
                iconSource: "iconName"
                textSource: "name"
                subSource: "mainModel"
                subSubSource: "canonicalName"
            }

            PropertyChanges {
                target: pane2
                model: pane2AppsMenuModel
                iconSource: "iconName"
                textSource: "name"
                subSource: "mainModel"
                subSubSource: "canonicalName"
            }

            PropertyChanges {
                target: pane3
                model: pane3AppsMenuModel
                iconSource: "iconName"
                textSource: "name"
                subSource: "mainModel"
                subSubSource: "canonicalName"
            }
        },

        State {
            name: "Home"
            PropertyChanges {
                target: pane1
//                model: placesModel
                iconSource: "icon"
                textSource: "name"
                subSource: ""
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

        onSelectedTextChanged: {
            if (selectedText.toString().substring(selectedText.length - 1, selectedText.length) == "/") {
                pane2AppsMenuModel.clear();
                pane3AppsMenuModel.clear();
                populateMenu(selectedText, pane2AppsMenuModel);
            } else {
                appToRun = selectedText;
            }
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

        onSelectedTextChanged: {
            if (selectedText.toString().substring(selectedText.length - 1, selectedText.length) == "/") {
                pane3AppsMenuModel.clear();
                populateMenu(selectedText, pane3AppsMenuModel);
            } else {
                appToRun = selectedText;
            }
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

        onSelectedTextChanged: {
            if (selectedText.toString().substring(selectedText.length - 1, selectedText.length) == "/") {
                //FIXME: undefined handling of 3rd submenu pane
            } else {
                appToRun = selectedText;
            }
        }

        width: paneWidth
        height: parent.height
    }
}
