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
    onRequestArticleOpen: Qt.openUrlExternally(page.model.link)
  }

  Item {
    implicitWidth: 0 // Workaround supposed KF6 Kirigami issue with page width
    implicitHeight: contentItem.height
    ColumnLayout {
      id: contentItem
      width: parent.width

      ArticleInfobar {
        Layout.fillWidth: true
        model: page.model
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
