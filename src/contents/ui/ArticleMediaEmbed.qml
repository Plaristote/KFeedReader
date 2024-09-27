import QtQuick 2.15
import "."

ArticleMedia {
  property url embedUrl: model.embedUrl
  id: mediaRoot
  contentComponent: embedComponent
  onWidthChanged: updateContentSize()
  Component.onCompleted: updateContentSize()

  function updateContentSize() {
    const maxHeight = window.height * 0.67;
    const idealRatio = mediaSize.height / mediaSize.width;
    const desiredHeight = mediaRoot.width * idealRatio;
    const finalHeight = desiredHeight > maxHeight ? maxHeight : desiredHeight;
    const finalWidth = finalHeight * (1 / idealRatio);
    contentSize = Qt.size(finalWidth, finalHeight);
  }

  Component {
    id: embedComponent
    AppWebView {
      id: webView
      onFullScreenChanged: mediaRoot.fullScreen = webView.fullScreen
      function setupHtml() {
        const fullscreenOption = webView.fullScreenEnabled ? "allowfullscreen" : "donotallowfullscreen";
        const iframe = `<iframe width="${contentSize.width}" height="${contentSize.height}" src="${mediaRoot.embedUrl}" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" style="padding:0;margin:0" ${fullscreenOption}></iframe>`;
        const html = `<html><body style="margin:0;padding:0;">${iframe}</body></html>`;
        loadHtml(html);
      }
      function updateHtml() {
        runJavaScript(`document.querySelector("iframe") != null`, function(exists) {
          if (exists) {
            runJavaScript(`document.querySelector("iframe").width="${contentSize.width - frameMargin.width}"`);
            runJavaScript(`document.querySelector("iframe").height="${contentSize.height - frameMargin.height}"`);
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
