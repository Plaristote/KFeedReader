import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import "."

BrowserView {
  required property QtObject model
  required property QtObject feed
  //Kirigami.ColumnView.pinned: true

  id: page
  url: model.link
  actions: pageActions.contextualActions
  onModelChanged: model.read = true

  topBar: ArticleInfobar {
    model: page.model
  }

  ArticleActions {
    id: pageActions
    model: page.model
    feed: page.feed
    onRequestArticleChange: page.model = article
    onRequestArticleOpen: Qt.openUrlExternally(page.url)
  }
}
