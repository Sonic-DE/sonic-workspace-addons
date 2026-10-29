import QtQuick

import org.kde.plasma.configuration

ConfigModel {
    ConfigCategory {
         name: i18n("General")
         icon: "preferences-desktop-plasma"
         source: "ConfigGeneral.qml"
    }
    ConfigCategory {
         name: i18n("Edit Menu")
         icon: "application-menu"
         source: "ConfigMenuJson.qml"
    }
    ConfigCategory {
         name: i18n("Edit JSON")
         icon: "code-context"
         source: "ConfigEditJson.qml"
    }
}

