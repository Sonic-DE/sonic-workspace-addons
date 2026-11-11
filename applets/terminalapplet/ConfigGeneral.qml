/*
    SPDX-FileCopyrightText: 2026 Joseph Crowell <joseph.w.crowell@gmail.com>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kcmutils as KCM
import org.kde.kirigami as Kirigami

KCM.SimpleKCM {
    property alias cfg_profile: profile.text

    Kirigami.FormLayout {
        QQC2.TextField {
            id: profile
            Kirigami.FormData.label: i18n("Profile:")
            placeholderText: i18n("Default profile")
        }
    }
}
