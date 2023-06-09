/*
 *  SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Layouts 1.3
import QtQuick.Controls as QQC2

import org.kde.iconthemes as KIconThemes
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore

Kirigami.FormLayout {
    anchors.right: parent.right
    anchors.left: parent.left

    property alias cfg_useMinViewWidth: useMinViewWidth.checked
    property alias cfg_minViewWidth: minViewWidth.value
    property alias cfg_constantZoomFactor: constantZoomFactor.value

    // Icon
    property string cfg_icon: plasmoid.configuration.icon
    property alias cfg_useFavIcon: useFavIcon.checked

    QQC2.ButtonGroup { id: zoomGroup }
    QQC2.ButtonGroup {
        id: iconGroup
    }

    RowLayout {
        Kirigami.FormData.label: i18nc("@title:group", "Content scaling:")

        QQC2.RadioButton {
            id: useConstantZoom
            text: i18nc("@option:radio", "Fixed scale:")

            QQC2.ButtonGroup.group: zoomGroup

            onClicked: {
                constantZoomFactor.forceActiveFocus();
            }
        }

        QQC2.SpinBox {
            id: constantZoomFactor
            editable: true
            enabled: useConstantZoom.checked

            validator: RegularExpressionValidator {
                regularExpression: /[0-9]?[0-9]{2}[ ]?%/
            }

            textFromValue: function(value) {
                return value+"%";
            }

            valueFromText: function(text) {
                return text.split(" ")[0].split("%")[0];
            }

            from: 25
            to: 500
        }
    }

    RowLayout {
        QQC2.RadioButton {
            id: useMinViewWidth
            text: i18nc("@option:radio", "Automatic scaling if width is below")

            QQC2.ButtonGroup.group: zoomGroup

            onClicked: {
                minViewWidth.forceActiveFocus();
            }
        }

        QQC2.SpinBox {
            id: minViewWidth
            editable: true
            enabled: useMinViewWidth.checked

            validator: RegularExpressionValidator {
                regularExpression: /RegExp("[0-9]?[0-9]{3}[ ]?px")/
            }

            textFromValue: function(value) {
                return value+"px";
            }

            valueFromText: function(text) {
                return text.split(" ")[0].split("px")[0];
            }

            from: 320
            to: 3840
        }
    }

    RowLayout {
        Kirigami.FormData.label: i18nc("@title:group", "Icon:")

        QQC2.RadioButton {
            id: useSystemIcon
            checked: !cfg_useFavIcon

            QQC2.ButtonGroup.group: iconGroup
        }

        QQC2.Button {
            id: iconButton

            implicitWidth: previewFrame.width + PlasmaCore.Units.smallSpacing * 2
            implicitHeight: previewFrame.height + PlasmaCore.Units.smallSpacing * 2
            enabled: useSystemIcon.checked
            hoverEnabled: true

            Accessible.name: i18nc("@action:button", "Change Web Browser's icon")
            Accessible.description: i18nc("@info:whatsthis", "Current icon is %1. Click to open menu to change the current icon or reset to the default icon.", cfg_icon)
            Accessible.role: Accessible.ButtonMenu

            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            QQC2.ToolTip.text: i18nc("@info:tooltip", "Icon name is \"%1\"", cfg_icon)
            QQC2.ToolTip.visible: iconButton.hovered && cfg_icon.length > 0

            onPressed: iconMenu.opened ? iconMenu.close() : iconMenu.open()

            KIconThemes.IconDialog {
                id: iconDialog
                onIconNameChanged: {
                    cfg_icon = iconName || "";
                }
            }

            PlasmaCore.FrameSvgItem {
                id: previewFrame
                anchors.centerIn: parent
                imagePath: plasmoid.formFactor === PlasmaCore.Types.Vertical || plasmoid.formFactor === PlasmaCore.Types.Horizontal
                        ? "widgets/panel-background" : "widgets/background"
                width: PlasmaCore.Units.iconSizes.large + fixedMargins.left + fixedMargins.right
                height: PlasmaCore.Units.iconSizes.large + fixedMargins.top + fixedMargins.bottom

                PlasmaCore.IconItem {
                    anchors.centerIn: parent
                    width: PlasmaCore.Units.iconSizes.large
                    height: width
                    source: cfg_icon || "applications-internet"
                }
            }

            QQC2.Menu {
                id: iconMenu

                // Appear below the button
                y: +parent.height

                QQC2.MenuItem {
                    text: i18nc("@item:inmenu Open icon chooser dialog", "Choose…")
                    icon.name: "document-open-folder"
                    Accessible.description: i18nc("@info:whatsthis", "Choose an icon for Web Browser")
                    onClicked: iconDialog.open()
                }
                QQC2.MenuItem {
                    text: i18nc("@item:inmenu Reset icon to default", "Reset to default icon")
                    icon.name: "edit-clear"
                    enabled: cfg_icon !== ""
                    onClicked: cfg_icon = ""
                }
            }
        }
    }

    QQC2.RadioButton {
        id: useFavIcon

        text: i18nc("@option:radio", "From website (Favicon)")

        QQC2.ButtonGroup.group: iconGroup
    }
}
