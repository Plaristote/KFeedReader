import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0
import "."

Kirigami.ScrollablePage {
  property QtObject model
  id: page
  title: model.name
  actions.main: folderActions.mainAction
  actions.contextualActions: folderActions.contextualActions

  FolderActions {
    id: folderActions
    model: page.model
  }

  ColumnLayout {
    spacing: 0

    Controls.ProgressBar {
      visible: model.fetching
      from: 0
      to: 1
      value: model.progress
      Layout.fillWidth: true
    }

    Kirigami.BasicListItem {
      bold: true
      label: i18n("All")
      action: Controls.Action {
        checkable: true
        checked: pageStack.lastItem.folder == page.model
        onTriggered: pageStack.push(Qt.resolvedUrl("./AggregatedFeed.qml"), { folder: page.model })
      }
      UnreadCountBox {
        model: page.model
        Layout.alignment: Qt.VerticalCenter
      }
    }

    Repeater {
      focus: true
      model: page.model.items
      delegate: Kirigami.BasicListItem {
        property QtObject item: page.model.items[index]
        bold: true
        label: item.name
        subtitle: item.description.replace(/\n/g, '')
        subtitleItem {
          clip: true
          textFormat: Text.PlainText
          wrapMode: Text.NoWrap
        }
        leadingFillVertically: false
        leading: Item {
          implicitWidth: 16
          Image {
            id: faviconImage
            source: item.faviconUrl
            height: parent.height > 16 ? 16 : parent.height
            width: 16
            anchors.centerIn: parent
          }
        }
        UnreadCountBox {
          model: page.model.items[index]
        }
        action: Controls.Action {
          checkable: true
          checked: pageStack.lastItem.model == item
          onTriggered: pageStack.push(item.view, { model: item })
        }
      } // END delegate
    }
  }
}
