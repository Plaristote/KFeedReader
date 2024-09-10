import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegate
import org.kde.fluxkap 1.0
import "."

Kirigami.ScrollablePage {
  property QtObject model
  property var nextPage: window.getNextPageInStack(this, window.pageStack.lastItem)
  id: page
  title: model.name

  actions: folderActions.actions

  function toggleSearch() { searchField.visible = !searchField.visible; }

  FolderActions {
    id: folderActions
    model: page.model
    focused: window.pageStack.currentItem === page
    onToggleSearch: page.toggleSearch();
  }

  Item {
    implicitWidth: 0 // Workaround supposed KF6 Kirigami issue with page width
    implicitHeight: contentItem.height
    ColumnLayout {
      id: contentItem
      spacing: 0
      width: parent.width

      AllFeedListItem {
        label: i18n("All")
        iconSource: "mail-message-new-list"
        model: page.model
        nextPage: page.nextPage
        Layout.fillWidth: true
      }

      AllFeedListItem {
        unreadOnly: true
        label: i18n("Unread")
        iconSource: "mail-mark-unread-new"
        model: page.model
        nextPage: page.nextPage
        Layout.fillWidth: true
      }

      Repeater {
        focus: true
        model: page.model.items.length
        delegate: Controls.ItemDelegate {
          property QtObject item: page.model.items[index]
          Layout.fillWidth: true
          visible: !searchField.visible || searchField.matches(item, searchField.text)
          highlighted: action.checked
          contentItem: ListItemDelegate {
            title: item.name
            subtitle: item.description.replace(/\n/g, '')
            iconSource: item.faviconUrl
            trailing: UnreadCountBox {
              model: page.model.items[index]
            }
          }
          action: Controls.Action {
            checkable: true
            checked: nextPage != null && nextPage.model == item
            onTriggered: pageStack.push(item.view, { model: item })
          }
        } // END delegate
      } // END Repeater
    } // END ColumnLayout
  } // END KF6 workaround

  footer: ColumnLayout {
    Controls.ProgressBar {
      visible: model.fetching
      from: 0
      to: 1
      value: model.progress
      Layout.fillWidth: true
      Layout.preferredHeight: 30
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
}
