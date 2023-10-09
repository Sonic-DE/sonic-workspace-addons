/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick3D
import org.kde.kwin as KWinComponents

Item {
    id: root
    focus: true

    readonly property QtObject targetScreen: KWinComponents.SceneView.screen

    property bool animationEnabled: false

    function start() {
        cameraController.rotateTo(KWinComponents.Workspace.currentDesktop);
        root.animationEnabled = true;
        cameraController.state = "distant";
    }

    function stop() {
        cameraController.rotateTo(KWinComponents.Workspace.currentDesktop);
        cameraController.state = "close";
    }

    function switchToSelected() {
        const eulerRotation = cameraController.rotation.toEulerAngles();
        const desktop = cube.desktopAt(eulerRotation.y);
        KWinComponents.Workspace.currentDesktop = desktop;
        effect.deactivate();
    }

    View3D {
        id: view
        anchors.fill: parent

        Loader {
            id: colorSceneEnvironment
            active: effect.configuration.Background == 0 // CubeEffect.BackgroundMode.Color
            sourceComponent: SceneEnvironment {
                clearColor: effect.configuration.BackgroundColor
                backgroundMode: SceneEnvironment.Color
            }
        }

        Loader {
            id: skyboxSceneEnvironment
            active: effect.configuration.Background == 1 // CubeEffect.BackgroundMode.Skybox
            sourceComponent: SceneEnvironment {
                backgroundMode: SceneEnvironment.SkyBox
                lightProbe: Texture {
                    source: effect.configuration.Skybox
                }
            }
        }

        environment: {
            console.log(effect.configuration.BackgroundColor)
            switch (effect.configuration.Background) {
            case 1: // CubeEffect.BackgroundMode.Skybox:
                return skyboxSceneEnvironment.item;
            case 0: // CubeEffect.BackgroundMode.Color:
                return colorSceneEnvironment.item;
            }
        }

        PerspectiveCamera { id: camera }

        Cube {
            id: cube
            faceDisplacement: effect.configuration.CubeFaceDisplacement
            faceSize: Qt.size(root.width, root.height)
        }

        CubeCameraController {
            id: cameraController
            anchors.fill: parent
            state: "close"
            camera: camera
            xInvert: effect.configuration.MouseInvertedX
            yInvert: effect.configuration.MouseInvertedY

            states: [
                State {
                    name: "close"
                    PropertyChanges {
                        target: cameraController
                        radius: cube.faceDistance + 0.5 * cube.faceSize.height / Math.tan(0.5 * camera.fieldOfView * Math.PI / 180)
                    }
                },
                State {
                    name: "distant"
                    PropertyChanges {
                        target: cameraController
                        radius: cube.faceDistance * effect.configuration.DistanceFactor + 0.5 * cube.faceSize.height / Math.tan(0.5 * camera.fieldOfView * Math.PI / 180)
                    }
                }
            ]

            Behavior on rotation {
                enabled: !cameraController.busy && root.animationEnabled
                QuaternionAnimation {
                    id: rotationAnimation
                    duration: effect.animationDuration
                    easing.type: Easing.OutCubic
                }
            }
            Behavior on radius {
                NumberAnimation {
                    duration: effect.animationDuration
                    easing.type: Easing.OutCubic
                }
            }

            function rotateToLeft() {
                if (rotationAnimation.running) {
                    return;
                }
                rotation = Quaternion.fromEulerAngles(0, -cube.angleTick, 0).times(rotation);
            }

            function rotateToRight() {
                if (rotationAnimation.running) {
                    return;
                }
                rotation = Quaternion.fromEulerAngles(0, cube.angleTick, 0).times(rotation);
            }

            function rotateTo(desktop) {
                if (rotationAnimation.running) {
                    return;
                }
                rotation = Quaternion.fromEulerAngles(0, cube.desktopAzimuth(desktop), 0);
            }
        }
    }

    MouseArea {
        anchors.fill: view
        onClicked: root.switchToSelected();
    }

    Connections {
        target: effect
        onActivatedChanged: {
            if (!effect.activated) {
                stop();
            }
        }
    }

    Keys.onEscapePressed: effect.deactivate();
    Keys.onLeftPressed: cameraController.rotateToLeft();
    Keys.onRightPressed: cameraController.rotateToRight();
    Keys.onEnterPressed: root.switchToSelected();
    Keys.onReturnPressed: root.switchToSelected();
    Keys.onSpacePressed: root.switchToSelected();

    Component.onCompleted: start();
}
