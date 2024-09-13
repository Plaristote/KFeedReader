import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.fluxkap 1.0
import "."

Kirigami.ScrollablePage {
  property QtObject model
  property bool readOnly: false
  property bool leadingFeedIcon: false
  property bool fetching: model.fetching
  property bool progress: model.progress
  property url feedViewUrl: Qt.resolvedUrl(model.skipPreviewSetting !== 2 ? "./Article.qml" : "./BrowserArticleView.qml")
  id: page
  title: model.name
  actions: feedActions.actions

  Item {
    implicitWidth: 0 // Workaround supposed KF6 Kirigami issue with page width
    implicitHeight: contentItem.height
    ColumnLayout {
      id: contentItem
      spacing: 0
      width: parent.width

      GridLayout {
        Layout.fillWidth: true
        Layout.bottomMargin: Kirigami.Units.smallSpacing
        columns: width > 500 ? 2 : 1
        visible: description.text.length || model.logoUrl != null

        Item {
          Layout.fillHeight: true
          Layout.alignment: Qt.AlignHCenter
          implicitWidth: logo.width
          implicitHeight: logo.height
          visible: model.logoUrl != null
          Image {
            id: logo
            source: model.logoUrl
            height: Math.min(sourceSize.height, description.text.length > 150 ? 200 : 50)
            width: Math.min(sourceSize.width, height)
            fillMode: Image.PreserveAspectFit
          }
        }

        Text {
          id: description
          Layout.fillWidth: true
          text: model.description
          wrapMode: Text.WordWrap
        }
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
        delegate: Controls.ItemDelegate {
          property QtObject item: page.model.articles[index]
          Layout.fillWidth: true
          visible: !searchField.visible || searchField.matches(item, searchField.text)
          highlighted: action.checked
          contentItem: ListItemDelegate {
            title: item.title
            bold: !item.read
            iconSource: leadingFeedIcon ? item.faviconUrl : ""
            trailing: Text {
              text: page.model.articles[index].publicationDate.toLocaleDateString({})
            }
          }
          action: Controls.Action {
            checkable: true
            checked: pageStack.lastItem.model == item
            onTriggered: pageStack.push(page.feedViewUrl, { model: item, feed: page.model })
          }
        } // END delegate
      } // END Repeater
    } // END ColumnLayout
  } // END KF6 workaround

  footer: ColumnLayout {
    Controls.ProgressBar {
      visible: page.fetching
      from: 0
      to: 1
      value: page.progress
      Layout.fillWidth: true
      Layout.preferredHeight: 10
    }

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

  function toggleSearch() {
    searchField.visible = !searchField.visible;
  }

  FeedActions {
    id: feedActions
    model: page.model
    readOnly: page.readOnly
    focused: window.pageStack.currentItem === page
    onRequireDestroy: destroyConfirmDialog.open()
    onToggleSearch: page.toggleSearch()
    onMarkAsRead: page.model.markAsRead()
  }

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
