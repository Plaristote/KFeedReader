import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0

Kirigami.ScrollablePage {
  property QtObject model
  id: page
  title: model.name
  Kirigami.ColumnView.pinned: true

  ColumnLayout {
    spacing: 0

    Controls.ProgressBar {
      visible: model.fetching
      from: 0
      to: 1
      value: model.progress
      Layout.fillWidth: true
    }

    Text {
      text: model.description
      wrapMode: Text.WordWrap
      Layout.fillWidth: true
      Layout.bottomMargin: Kirigami.Units.smallSpacing
    }

    Kirigami.Separator {
      Layout.fillWidth: true
      visible: model.description.length > 0
    }

    RssTextInput {
      model: page.model
    }

    Repeater {
      focus: true
      model: page.model.articles
      delegate: Kirigami.BasicListItem {
        property QtObject item: page.model.articles[index]
        label: item.title
        bold: !item.read
        trailing: Text {
          text: item.publicationDate.toLocaleDateString()
        }
        action: Controls.Action {
          checkable: true
          checked: pageStack.lastItem.model == item
          onTriggered: pageStack.push(Qt.resolvedUrl("./Article.qml"), { model: item })
        }
      } // END delegate
    }
  }

  footer: ColumnLayout {
    Kirigami.Separator {
      Layout.fillWidth: true
      visible: model.copyright.length > 0
    }

    Text {
      Layout.fillWidth: true
      text: model.copyright
      wrapMode: Text.WordWrap
    }
  }

  actions.main: Kirigami.Action {
    text: i18n("Update")
    icon.name: "cloud-download"
    tooltip: i18n("Download new articles from the remote server")
    onTriggered: page.model.fetch()
  }

  actions.contextualActions: [
    Kirigami.Action {
      text: i18n("Edit")
      icon.name: "edit-entry"
      tooltip: i18n("Change the settings of a feed")
      onTriggered: pageStack.push(Qt.resolvedUrl("./EditFeed.qml"), { source: page.model })
    },
    Kirigami.Action {
      text: i18n("Remove")
      icon.name: "edit-delete-remove"
      tooltip: i18n("Permanently remove the feed from the application")
      onTriggered: destroyConfirmDialog.open()
    }
  ]

  Kirigami.PromptDialog {
    id: destroyConfirmDialog
    title: i18n("Removing feed")
    subtitle: i18n("Are you sure you want to permanently remove this feed ?")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    onAccepted: {
      page.model.remove();
      pageStack.pop();
    }
  }
}
