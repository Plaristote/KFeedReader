import QtQuick 2.15
import QtWebEngine 1.10

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
