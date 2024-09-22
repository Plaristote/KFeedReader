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
      App.cloudProvider.setSettings(urlField.text, authTokenField.text)
    }
  }

  ColumnLayout {
    Kirigami.FormLayout {
      id: form

      Controls.TextField {
        id: urlField
        Kirigami.FormData.label: i18n("URL")
        text: App.cloudProvider.url
        onTextChanged: settingsUpdateTimer.start()
      }

      Controls.TextField {
        id: authTokenField
        Kirigami.FormData.label: i18n("Application token")
        text: App.cloudProvider.authToken
        onTextChanged: settingsUpdateTimer.start()
      }

      Kirigami.Separator {
        Layout.fillWidth: true
      }

      Text {
        Kirigami.FormData.label: i18n("Sharing status")
        text: App.sharingService.enabled ? i18n("Ready") : i18n("Not ready")
      }

      Controls.BusyIndicator {
        Kirigami.FormData.label: i18n("Loading sharing service")
        visible: App.sharingService.inProgress
      }
    }
  }
}
