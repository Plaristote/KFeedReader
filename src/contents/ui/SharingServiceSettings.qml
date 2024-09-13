import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.fluxkap 1.0

Kirigami.ScrollablePage {
  id: page

  Timer {
    id: settingsUpdateTimer
    interval: 750
    onTriggered: {
      App.sharingService.setSettings(urlField.text, authTokenField.text)
    }
  }

  ColumnLayout {
    Kirigami.FormLayout {
      id: form

      Controls.TextField {
        id: urlField
        Kirigami.FormData.label: i18n("URL")
        text: App.sharingService.url
        onTextChanged: settingsUpdateTimer.start()
      }

      Controls.TextField {
        id: authTokenField
        Kirigami.FormData.label: i18n("Application token")
        text: App.sharingService.authToken
        onTextChanged: settingsUpdateTimer.start()
      }

      Kirigami.Separator {
        Layout.fillWidth: true
      }

      Text {
        Kirigami.FormData.label: i18n("Status")
        text: App.sharingService.enabled ? i18n("Ready") : i18n("Not ready")
      }
    }

    Controls.BusyIndicator {
      visible: App.sharingService.inProgress
    }
  }
}
