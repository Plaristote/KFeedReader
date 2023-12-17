import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0

Kirigami.ScrollablePage {
  property QtObject model
  property bool readOnly: false
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
        visible: !searchField.visible || searchField.matches(item.title)
        action: Controls.Action {
          checkable: true
          checked: pageStack.lastItem.model == item
          onTriggered: pageStack.push(Qt.resolvedUrl("./Article.qml"), { model: item, feed: page.model })
        }
      } // END delegate
    }
  }

  footer: ColumnLayout {
    Kirigami.Separator {
      Layout.fillWidth: true
      visible: (model.copyright || "").length > 0
    }

    Text {
      Layout.fillWidth: true
      text: model.copyright || ""
      wrapMode: Text.WordWrap
      visible: text.length > 0
    }

    Kirigami.Separator {
      Layout.fillWidth: true
      visible: searchField.visible
    }

    FeedSearchField {
      Layout.fillWidth: true
      id: searchField
      visible: false
    }
  }

  FeedActions {
    id: feedActions
    model: page.model
    readOnly: page.readOnly
    onRequireDestroy: destroyConfirmDialog.open()
    onToggleSearch: searchField.visible = !searchField.visible
  }

  actions.main: feedActions.mainAction
  actions.contextualActions: feedActions.contextualActions

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
