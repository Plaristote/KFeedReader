import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.fluxkap 1.0
import "."

Kirigami.ScrollablePage {
  required property QtObject model
  required property QtObject feed
  //Kirigami.ColumnView.pinned: true

  id: page
  title: {
    if (model.title.length > window.titleMaxLength) {
      return model.title.substring(0, window.titleMaxLength) + '…';
    }
    return model.title;
  }
  onModelChanged: model.read = true
  actions: [viewAction].concat(pageActions.contextualActions)

  Kirigami.Action {
    id: viewAction
    text: i18n("View")
    icon.name: "quickview"
    tooltip: i18n("Open the article link in the application")
    shortcut: "Ctrl+O"
    onTriggered: {
      pageStack.replace(Qt.resolvedUrl("BrowserArticleView.qml"), { model: page.model, feed: page.feed });
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
        clip: true
        Breadcrumbs {
          id: breadcrumbs
          crumbs: page.model.crumbs
          anchors { top: parent.top; right: parent.right }
        }
      }

      RowLayout {
        id: metaDataActions

        Item { Layout.fillWidth: true }

        Controls.Button {
          id: dateLabel
          icon.name: "view-calendar-day"
          text: page.model.publicationDate.toLocaleDateString()
          flat: true
        }

        Controls.Button {
          id: categoryLabel
          visible: page.model.category.length > 0
          icon.name: "category"
          text: page.model.category
          flat: true
        }

        Controls.Button {
          id: authorButton
          visible: page.model.author.length > 0
          icon.name: "user-identity"
          text: page.model.author
          flat: !page.model.authorUrl.toString().length
          onClicked: Qt.openUrlExternally(page.model.authorUrl)
        }
      }

      Repeater {
        model: page.model.medias
        delegate: Loader {
          property QtObject model: page.model.medias[index]
          source: model.qmlView()
          Layout.fillWidth: true
        }
      }

      Text {
        id: articleDescription
        visible: text.length > 0
        text: page.model.description
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
      }
    }
  }
}
