import QtQuick 2.15
import QtWebEngine 1.10
import "."

ArticleMedia {
  id: mediaRoot
  property string videoTag: model.youtubeVideoTag()
  contentComponent: youtubeComponent
  contentSize: Qt.size(
    Math.min(model.contentSize.width, width),
    model.contentSize.height
  )

  Component {
    id: youtubeComponent
    WebEngineView {
      function setupHtml() {
        const iframe = `<iframe width="${contentSize.width}" height="${contentSize.height}" src="https://www.youtube.com/embed/${videoTag}" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" style="padding:0;margin:0" allowfullscreen></iframe>`;
        const html = `<html><body style="margin:0;padding:0;">${iframe}</body></html>`;
        loadHtml(html);
      }
      function updateHtml() {
        runJavaScript(`document.querySelector("iframe") != null`, function(exists) {
          if (exists) {
            runJavaScript(`document.querySelector("iframe").width="${contentSize.width}"`);
            runJavaScript(`document.querySelector("iframe").height="${contentSize.height}"`);
          }
        });
      }
      Connections {
        target: mediaRoot
        function onContentSizeChanged() { updateHtml() }
      }
      Timer {
        running: true
        interval: 500
        onTriggered: updateHtml()
      }
      Component.onCompleted: setupHtml()
    }
  }
}
