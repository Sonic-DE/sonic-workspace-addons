import QtQuick
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12 as QQC2
import org.kde.plasma.components 3.0 as PlasmaComponents3

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kcmutils as KCM

KCM.SimpleKCM {
    property alias cfg_useMinViewWidth: useMinViewWidth.checked
    property alias cfg_minViewWidth: minViewWidth.value
    property alias cfg_constantZoomFactor: constantZoomFactor.value
    property alias cfg_useDefaultUrl: useDefaultUrl.checked
    property alias cfg_defaultUrl: defaultUrl.text

    Kirigami.FormLayout {
        RowLayout {
            Kirigami.FormData.label: i18nc("@title:group", "Default Url:")

            QQC2.CheckBox {
                id: useDefaultUrl
                text: i18nc("@option:radio", "Open default url on load")
                checked: cfg_useDefaultUrl.checked

                onCheckedChanged: {
                    cfg_useDefaultUrl = useDefaultUrl.checked
                    defaultUrl.forceActiveFocus();
                }
            }
        }

        PlasmaComponents3.TextField {
            id: defaultUrl
            onAccepted: {
                var url = text;
                if (url.indexOf(":/") < 0) {
                    url = "http://" + url;
                }
            }
            onActiveFocusChanged: {
                if (activeFocus) {
                    selectAll();
                }
            }

            text: "https://ddg.gg"
            enabled: useDefaultUrl.checked
            Accessible.description: text.length > 0 ? text : i18nc("@info", "Type a URL")
        }


        QQC2.ButtonGroup { id: zoomGroup }

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
                    regularExpression: /[0-9]?[0-9]{3}[ ]?px/
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
    }
}
