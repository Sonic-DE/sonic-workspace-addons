/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.15

import org.kde.plasma.components 3.0 as PlasmaComponents3

PlasmaComponents3.BusyIndicator {
    id: indicator
    anchors.fill: parent

    required property DropArea dropArea
    property int jobRemaining: 0

    signal jobDone

    onJobDone: {
        jobRemaining -= 1;
        if (!jobRemaining) {
            animator.start();
        }
    }

    OpacityAnimator {
        id: animator
        easing.type: Easing.InQuad
        from: 1
        to: 0
        target: indicator
        duration: PlasmaCore.Units.longDuration

        onStopped: {
            dropArea.indicator = null;
            indicator.destroy();
        }
    }
}
