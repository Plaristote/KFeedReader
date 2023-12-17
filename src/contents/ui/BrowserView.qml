import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtWebEngine 1.10
import "."

Kirigami.Page {
  required property QtObject model
  required property QtObject feed

  id: page
  title: webview.title
  onModelChanged: model.read = true
  actions.contextualActions: pageActions.contextualActions
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

  Item {
    id: webviewContainer
    anchors.fill: parent
    AppWebView {
      id: webview
      url: model.link
      anchors.fill: parent
    }
  }
}
