import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtWebEngine 1.10
import "."

Kirigami.Page {
  property QtObject model

  title: webview.title
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
    model: parent.model
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
