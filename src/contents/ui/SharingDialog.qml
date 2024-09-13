import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.fluxkap 1.0

Kirigami.Dialog {
  id: dialog
  title: i18n("Select the feeds in which you want to share this article")
  property QtObject model
  property var selectedFeeds: []

  Connections {
    target: App.sharingService
    function onShared() {
      dialog.close();
    }
    function onFailedToShare() {
      errorDisplay.visible = true;
    }
  }

  ColumnLayout {
    Text {
      id: errorDisplay
      text: i18n("Failed to share the article !")
      visible: false
    }

    ListView {
      id: listView
      // hints for the dialog dimensions
      implicitWidth: Kirigami.Units.gridUnit * 16
      implicitHeight: Kirigami.Units.gridUnit * 16
      visible: !App.sharingService.inProgress

      model: App.sharingService.feedNames
      delegate: Controls.CheckDelegate {
        topPadding: Kirigami.Units.smallSpacing * 2
        bottomPadding: Kirigami.Units.smallSpacing * 2
        implicitWidth: listView.width
        text: modelData
        onCheckStateChanged: {
          if (checkState === Qt.Checked)
            selectedFeeds.push(modelData);
          else
            selectedFeeds = selectedFeeds.filter(name => name != modelData);
        }
      }
    }

    Controls.BusyIndicator {
      id: busyIndicator
      visible: App.sharingService.inProgress
    }
  }

  customFooterActions: Kirigami.Action {
    text: i18n("Share")
    icon.name: "document-share"
    enabled: !App.sharingService.inProgress
    onTriggered: App.sharingService.share(selectedFeeds, model)
  }
}
