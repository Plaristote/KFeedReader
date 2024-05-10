import QtQuick
import QtWebEngine

WebEngineView {
  id: webView
  property bool fullScreen: false

  onFullScreenRequested: {
    if (request.toggleOn) {
      window.showFullScreen();
    } else {
      window.showNormal();
    }
    webView.fullScreen = request.toggleOn;
    request.accept();
  }

  onNewWindowRequested: function (request) {
    if (request.userInitiated)
      Qt.openUrlExternally(request.requestedUrl);
  }

  Shortcut {
    sequence: "Esc"
    enabled: webView.fullScreen
    onActivated: {
      webView.fullScreen = false;
      webView.fullScreenCancelled();
      window.showNormal();
    }
  }
}
