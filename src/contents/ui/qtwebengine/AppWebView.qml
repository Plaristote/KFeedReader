import QtQuick
import QtWebEngine
import QtQuick.Controls as Controls

WebEngineView {
  id: webView
  property bool fullScreen: false
  readonly property bool fullScreenEnabled: true
  readonly property bool searchEnabled: true
  readonly property size frameMargin: Qt.size(0, 0)

  profile.persistentCookiesPolicy: WebEngineProfile.ForcePersistentCookies
  profile.offTheRecord: false

  onFullScreenRequested: {
    if (request.toggleOn) {
      window.fullScreenWidget = webView;
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

  function closeFullScreen() {
    webView.fullScreen = false;
    webView.fullScreenCancelled();
    window.showNormal();
  }

  Shortcut {
    sequence: "Esc"
    enabled: webView.fullScreen
    onActivatedAmbiguously: closeFullScreen();
    onActivated: closeFullScreen();
  }
}
