import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtWebEngine 1.10

Kirigami.Page {
  property QtObject model

  title: webview.title
  actions.contextualActions: pageActions.contextualActions

  ArticleActions {
    id: pageActions
    model: parent.model
  }

  WebEngineView {
    id: webview
    url: model.link
    anchors.fill: parent
  }
}
