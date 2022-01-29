/*
    SPDX-FileCopyrightText: 2022 ivan tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2

import org.kde.plasma.core 2.0 as PlasmaCore

QtObject {
    id: manager

    property Flickable flickable

    property real x: 0.0
    property real y: 0.0

    /*
        In onSave handler implementors should persist values x and y provided
        by this manager object.
    */
    signal save()

    /*
        In onRestore handler implementors should populate values x and y of
        this manager object with whatever was previously persisted during
        onSave() signal handling.
    */
    signal restore()

    function forceSave() {
        if (flickable !== null) {
            x = flickable.contentX;
            y = flickable.contentY;
            save();
        }
    }

    function forceRestore() {
        if (flickable !== null) {
            restore();
            flickable.contentX = x;
            flickable.contentY = y;
        }
    }

    // Save scrolling position when it changes, but throttle to avoid
    // killing a storage disk.
    property list<QtObject> data: [
        Timer {
            id: throttle
            interval: PlasmaCore.Units.humanMoment
            repeat: false
            running: false
            onTriggered: manager.forceSave()
        },
        Connections {
            target: manager.flickable

            function onContentXChanged() {
                throttle.restart();
            }
            function onContentYChanged() {
                throttle.restart();
            }
        }
    ]

    // Give it some time to lay out the text, because at this
    // point in time content size is not reliable yet.
    Component.onCompleted: Qt.callLater(forceRestore)
    Component.onDestruction: forceSave()
}
