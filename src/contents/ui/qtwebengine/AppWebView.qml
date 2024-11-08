import QtQuick
import QtWebEngine
import QtQuick.Controls as Controls
import org.kde.fluxkap 1.0

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

  onLoadingChanged: function (loadRequest) {
    if (loadRequest.status == WebEngineView.LoadSucceededStatus) {
      for (let plugin of App.javaScriptPlugins.plugins) {
        if (plugin.matchesDomain(loadRequest.url)) {
          webView.runJavaScript(plugin.source);
        }
      }
    }
  }

  Shortcut {
    sequence: "Esc"
    enabled: webView.fullScreen
    onActivatedAmbiguously: closeFullScreen();
    onActivated: closeFullScreen();
  }
}
