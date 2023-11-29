import QtQuick 2.15
import QtWebEngine 1.10
import "."

ArticleMediaEmbed {
  id: mediaRoot
  property string videoTag: model.youtubeVideoTag()
  embedUrl: `https://www.youtube.com/embed/${videoTag}`
  contentSize: Qt.size(
    Math.min(model.contentSize.width, width),
    model.contentSize.height
  )
}
