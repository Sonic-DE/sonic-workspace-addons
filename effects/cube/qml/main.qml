/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.15
import QtQuick3D 1.15
import org.kde.kwin 3.0 as KWinComponents

Item {
    id: root
    focus: true

    required property QtObject effect
    required property QtObject targetScreen

    property bool animationEnabled: false

    function start() {
        cameraController.rotateTo(KWinComponents.Workspace.currentVirtualDesktop);
        root.animationEnabled = true;
        cameraController.state = "distant";
    }

    function stop() {
        cameraController.rotateTo(KWinComponents.Workspace.currentVirtualDesktop);
        cameraController.state = "close";
    }

    function switchToSelected() {
        const eulerRotation = effect.quaternionToEulerAngles(cameraController.rotation);
        const desktop = cube.desktopAt(eulerRotation.y);
        KWinComponents.Workspace.currentVirtualDesktop = desktop;
        effect.deactivate();
    }

    View3D {
        id: view
        anchors.fill: parent

        environment: SceneEnvironment {
            clearColor: "black"
            backgroundMode: SceneEnvironment.Color
        }

        PerspectiveCamera { id: camera }

        Cube {
            id: cube
            faceDisplacement: effect.cubeFaceDisplacement
            faceSize: Qt.size(root.width, root.height)
        }

        CubeCameraController {
            id: cameraController

            property real distanceScale: 1

            anchors.fill: parent
            state: "close"
            origin: cube
            camera: camera
            xInvert: effect.mouseInvertedX
            yInvert: effect.mouseInvertedY
            radius: cube.faceDistance * distanceScale + 0.5 * cube.faceSize.height * Math.tan(camera.fieldOfView * Math.PI / 180)

            states: [
                State {
                    name: "close"
                    PropertyChanges {
                        target: cameraController
                        distanceScale: 1
                    }
                },
                State {
                    name: "distant"
                    PropertyChanges {
                        target: cameraController
                        distanceScale: effect.distanceFactor
                    }
                }
            ]

            Behavior on rotation {
                enabled: !cameraController.busy && root.animationEnabled
                QuaternionAnimation {
                    duration: effect.animationDuration
                    easing.type: Easing.OutCubic
                }
            }
            Behavior on distanceScale {
                NumberAnimation {
                    duration: effect.animationDuration
                    easing.type: Easing.OutCubic
                }
            }

            function rotateToLeft() {
                rotation = effect.quaternionDotProduct(rotation, Quaternion.fromEulerAngles(0, -90, 0));
            }

            function rotateToRight() {
                rotation = effect.quaternionDotProduct(rotation, Quaternion.fromEulerAngles(0, 90, 0));
            }

            function rotateTo(desktop) {
                rotation = Quaternion.fromEulerAngles(0, cube.desktopAzimuth(desktop), 0);
            }
        }
    }

    MouseArea {
        anchors.fill: view
        onClicked: root.switchToSelected();
    }

    Keys.onEscapePressed: effect.deactivate();
    Keys.onLeftPressed: cameraController.rotateToLeft();
    Keys.onRightPressed: cameraController.rotateToRight();
    Keys.onEnterPressed: root.switchToSelected();
    Keys.onReturnPressed: root.switchToSelected();
    Keys.onSpacePressed: root.switchToSelected();

    Component.onCompleted: start();
}
