/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
    SPDX-FileCopyrightText: 2025 Hocine Hachemi <salahhachmi06@gmail.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick3D
import org.kde.kwin as KWinComponents

Model {
    id: face
    required property QtObject desktop
    required property int index
    required property size faceSize
    
    pickable: true
    source: "#Rectangle"
    
    materials: [
        DefaultMaterial {
            cullMode: Material.NoCulling
            lighting: DefaultMaterial.NoLighting
            diffuseMap: Texture {
                sourceItem: DesktopView {
                    desktop: face.desktop
                    width: faceSize.width
                    height: faceSize.height
                }
            }
        }
    ]
}
