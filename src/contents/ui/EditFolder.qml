import QtCore
import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.treeview 1.0 as TreeView
import org.kde.fluxkap 1.0
import "."
import QtQuick.Dialogs

Kirigami.Page {
  required property QtObject model
  id: page
  title: model.name

  ColumnLayout {
    anchors.fill: parent

    Kirigami.FormLayout {
      Controls.TextField {
        Kirigami.FormData.label: i18n("Name")
        visible: model != App.rootFolder
        text: model.name
        onEditingFinished: model.name = text
      }

      Controls.TextField {
        Kirigami.FormData.label: i18n("Description")
        visible: model != App.rootFolder
        text: model.description
        onEditingFinished: model.description = text
      }

      Controls.ComboBox {
        Kirigami.FormData.label: i18n("Display")
        model: [
          { name: i18n("Tree"), value: FeedFolder.TreeDisplay },
          { name: i18n("List"), value: FeedFolder.ListDisplay }
        ]
        textRole: "name"
        valueRole: "value"
        onActivated: page.model.displayType = currentValue
        Component.onCompleted: currentIndex = indexOfValue(page.model.displayType)
      }

      SkipPreviewCheckBox {
        Kirigami.FormData.label: i18n("Skip preview")
        model: page.model
      }
    }

    TtlSettingsForm {
      ttlSettings: page.model
      Component.onCompleted: reset()
    }

    SortableFolderTree {
      Layout.fillWidth: true
      Layout.fillHeight: true
      model: page.model
      clip: true
    }

    Kirigami.FormLayout {
      Controls.Button {
        Kirigami.FormData.label: i18n("Synchronize")
        text: i18n("YouTube Google Takeout subscriptions")
        onClicked: importYouTubeDialog.open()
      }
    }
  }

  FileDialog {
    id: importYouTubeDialog
    title: i18n("Pick a YouTube subcription file from a Google Takeout archive")
    fileMode: FileDialog.OpenFile
    nameFilters: ["(*.csv)"]
    currentFolder: StandardPaths.standardLocations(StandardPaths.DownloadLocation)[0]
    onAccepted: model.synchronizeWithYouTube(selectedFile)
  }
}
