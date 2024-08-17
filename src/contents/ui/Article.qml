import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0
import "."

Kirigami.ScrollablePage {
  required property QtObject model
  required property QtObject feed
  Kirigami.ColumnView.pinned: true

  id: page
  title: model.title
  onModelChanged: model.read = true
  actions: [viewAction].concat(pageActions.contextualActions)

  Kirigami.Action {
    id: viewAction
    text: i18n("View")
    icon.name: "quickview"
    tooltip: i18n("Open the article link in the application")
    shortcut: "Ctrl+O"
    onTriggered: {
      pageStack.replace(Qt.resolvedUrl("BrowserView.qml"), { model: page.model, feed: page.feed });
    }
  }

  Component.onCompleted: { page.model.read = true; }

  ArticleActions {
    id: pageActions
    model: page.model
    feed: page.feed
    onRequestArticleChange: page.model = article
  }

  Item {
    implicitWidth: 0 // Workaround supposed KF6 Kirigami issue with page width
    implicitHeight: contentItem.height
    ColumnLayout {
      id: contentItem
      width: parent.width

      Flickable {
        Layout.fillWidth: true
        Layout.preferredHeight: 50
        contentWidth: breadcrumbs.width
        Breadcrumbs {
          id: breadcrumbs
          crumbs: page.model.crumbs
          anchors { top: parent.top; right: parent.right }
        }
      }

      RowLayout {
        id: authorRow
        visible: page.model.author.length > 0
        spacing: 5
        Controls.Label {
          text: i18n("Author")
          Layout.fillWidth: true
        }
        Text {
          text: page.model.author
        }
        Controls.Button {
          icon.name: "link"
          visible: page.model.authorUrl.toString() != ""
          onClicked: Qt.openUrlExternally(page.model.authorUrl)
        }
      }

      Kirigami.Separator {
        Layout.fillWidth: true
        visible: authorRow.visible && articleDescription.visible
      }

      Text {
        id: articleDescription
        visible: text.length > 0
        text: page.model.description
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
      }

      Repeater {
        model: page.model.medias
        delegate: Loader {
          property QtObject model: page.model.medias[index]
          source: model.qmlView()
          Layout.fillWidth: true
        }
      }
    }
  }
}
