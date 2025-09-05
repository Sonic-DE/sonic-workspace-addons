/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
    SPDX-FileCopyrightText: 2025 Hocine Hachemi <salahhachmi06@gmail.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Window
import QtQuick3D
import Qt5Compat.GraphicalEffects
import org.kde.kwin as KWinComponents

Item {
    id: root
    focus: true

    readonly property QtObject targetScreen: KWinComponents.SceneView.screen

    function switchTo(desktop) {
        KWinComponents.Workspace.currentDesktop = desktop;
        effect.deactivate();
    }

    function switchToSelected() {
        const eulerRotation = cameraController.rotation.toEulerAngles();
        switchTo(cube.desktopAt(eulerRotation.y));
    }

    View3D {
        id: view
        anchors.fill: parent
        renderMode: View3D.Overlay
        
        SceneEnvironment {
            id: sceneEnvironment
            backgroundMode: SceneEnvironment.Transparent
            antialiasingMode: SceneEnvironment.MSAA
            antialiasingQuality: SceneEnvironment.High
        }

        environment: sceneEnvironment

        PerspectiveCamera {
            id: camera
            clipNear: 10.0
            clipFar: 100000.0
        }

        Cube {
            id: cube
            faceDisplacement: effect.configuration.CubeFaceDisplacement
            faceSize: Qt.size(root.width, root.height)
        }

        CubeCameraController {
            id: cameraController
            anchors.fill: parent
            state: effect.activated ? "distant" : "close"
            camera: camera
            xInvert: effect.configuration.MouseInvertedX
            yInvert: effect.configuration.MouseInvertedY

            states: [
                State {
                    name: "close"
                    PropertyChanges {
                        target: cameraController
                        radius: cube.faceDistance + 0.5 * cube.faceSize.height / Math.tan(0.5 * camera.fieldOfView * Math.PI / 180)
                        rotation: Quaternion.fromEulerAngles(0, cube.desktopAzimuth(KWinComponents.Workspace.currentDesktop), 0)
                    }
                },
                State {
                    name: "distant"
                    PropertyChanges {
                        target: cameraController
                        radius: cube.faceDistance * effect.configuration.DistanceFactor + 0.5 * cube.faceSize.height / Math.tan(0.5 * camera.fieldOfView * Math.PI / 180)
                        rotation: Quaternion.fromEulerAngles(effect.configuration.ElevationAngle, cube.desktopAzimuth(KWinComponents.Workspace.currentDesktop), 0)
                    }
                }
            ]

            Behavior on rotation {
                enabled: !cameraController.busy
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
                const eulerAngles = rotation.toEulerAngles();
                let next = Math.floor(eulerAngles.y / cube.angleTick) * cube.angleTick;
                if (Math.abs(next - eulerAngles.y) < 0.05 * cube.angleTick) {
                    next -= cube.angleTick;
                }
                rotation = Quaternion.fromEulerAngles(0, next - eulerAngles.y, 0).times(rotation);
            }

            function rotateToRight() {
                if (rotationAnimation.running) {
                    return;
                }
                const eulerAngles = rotation.toEulerAngles();
                let next = Math.ceil(eulerAngles.y / cube.angleTick) * cube.angleTick;
                if (Math.abs(next - eulerAngles.y) < 0.05 * cube.angleTick) {
                    next += cube.angleTick;
                }
                rotation = Quaternion.fromEulerAngles(0, next - eulerAngles.y, 0).times(rotation);
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        z: -1  // Behind the View3D
        color: "transparent"
        
        // Get the desktop background
        KWinComponents.DesktopBackground {
            id: desktopBackground
            activity: KWinComponents.Workspace.currentActivity
            desktop: KWinComponents.Workspace.currentDesktop
            outputName: targetScreen.name
            anchors.fill: parent
            visible: false  // Don't show directly, only used as source for blur
        }
        
        // Apply blur to the background
        FastBlur {
            anchors.fill: parent
            source: desktopBackground
            radius: 64
            transparentBorder: true
        }
        
        // Dim overlay for better contrast
        Rectangle {
            anchors.fill: parent
            color: Qt.rgba(0, 0, 0, 0.3)
        }
    }

    MouseArea {
        anchors.fill: view
        onClicked: mouse => {
            const hitResult = view.pick(mouse.x, mouse.y);
            if (hitResult.objectHit) {
                // Get the desktop from the hit object
                const hitFace = hitResult.objectHit;
                if (hitFace && hitFace.desktop) {
                    root.switchTo(hitFace.desktop);
                }
            } else {
                root.switchToSelected();
            }
        }
    }

    Keys.onEscapePressed: effect.deactivate();
    Keys.onLeftPressed: cameraController.rotateToLeft();
    Keys.onRightPressed: cameraController.rotateToRight();
    Keys.onEnterPressed: root.switchToSelected();
    Keys.onReturnPressed: root.switchToSelected();
    Keys.onSpacePressed: root.switchToSelected();
}
