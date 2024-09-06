import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import QtWebEngine 1.10
import "."

Kirigami.Page {
  required property QtObject model
  required property QtObject feed
  Kirigami.ColumnView.pinned: true

  id: page
  title: {
    if (webview.title.length > window.titleMaxLength) {
      return webview.title.slice(0, window.titleMaxLength) + '…';
    }
    return webview.title;
  }
  onModelChanged: model.read = true
  actions: pageActions.contextualActions
  states: [
    State {
      name: "fullScreen"
      when: webview.fullScreen
      ParentChange { target: webview; parent: window.fullScreenContainer }
    },
    State {
      name: "default"
      when: !webview.fullScreen
      ParentChange { target: webview; parent: webviewContainer }
    }
  ]

  ArticleActions {
    id: pageActions
    model: page.model
    feed: page.feed
    onRequestArticleChange: page.model = article
  }

  ColumnLayout {
    anchors.fill: parent

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

    Item {
      id: webviewContainer
      Layout.fillWidth: true
      Layout.fillHeight: true
      AppWebView {
        id: webview
        url: model.link
        anchors.fill: parent
      }
    }
  }
}
