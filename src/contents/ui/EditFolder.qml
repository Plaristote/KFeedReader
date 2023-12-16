import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.treeview 1.0 as TreeView
import org.kde.kfeedreader 1.0
import "."

Kirigami.Page {
  required property QtObject model
  id: page
  title: model.name

  ColumnLayout {
    anchors.fill: parent

    Kirigami.FormLayout {
      visible: model != App.rootFolder

      Controls.TextField {
        Kirigami.FormData.label: i18n("Name")
        text: model.name
        onEditingFinished: model.name = text
      }

      Controls.TextField {
        Kirigami.FormData.label: i18n("Description")
        text: model.description
        onEditingFinished: model.description = text
      }
    }

    FolderTreeView {
      Layout.fillWidth: true
      Layout.fillHeight: true
      model: page.model 
    }
  }
}
