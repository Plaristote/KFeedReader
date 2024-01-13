import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.treeview 1.0 as TreeView
import org.kde.kfeedreader 1.0
import "."
import QtQuick.Dialogs 1.3

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
    }

    FolderTreeView {
      Layout.fillWidth: true
      Layout.fillHeight: true
      model: page.model 
      clip: true
    }

    Kirigami.FormLayout {
      Controls.Button {
        Kirigami.FormData.label: i18n("Synchronize")
        text: "YouTube"
        onClicked: importYouTubeDialog.open()
      }
    }
  }

  FileDialog {
    id: importYouTubeDialog
    title: i18n("Pick a YouTube subcription file from a Google Takeout archive")
    nameFilters: ["(*.csv)"]
    folder: shortcuts.home
    onAccepted: model.synchronizeWithYouTube(fileUrl)
  }
}
