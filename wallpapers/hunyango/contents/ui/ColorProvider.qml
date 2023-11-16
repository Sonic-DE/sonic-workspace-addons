/*
 *   SPDX-FileCopyrightText: 2023 Oliver Beard <olib141@outlook.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

pragma Singleton

import QtQuick

Item {

    property color color: _randomColor()

    readonly property int fastDuration: 2000
    readonly property int slowDuration: 30000

    Behavior on color {
        SequentialAnimation {
            id: animation

            ColorAnimation {
                id: colorAnimation
                duration: slowDuration
                easing.type: Easing.InOutQuad
            }

            ScriptAction {
                script: {
                    colorAnimation.duration = slowDuration;
                    updateColor();
                }
            }
        }
    }

    function _randomColor() {
        return Qt.hsla(Math.random(), 1, 0.5, 1)
    }

    function updateColor(fast = false) {
        if (fast) {
            colorAnimation.duration = fastDuration;
        }
        color = _randomColor()
    }

    Component.onCompleted: updateColor()
}
