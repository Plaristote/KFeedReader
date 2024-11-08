import QtQuick
import QtWebView
import QtQuick.Controls as Controls
import org.kde.fluxkap 1.0

WebView {
  id: webView
  property bool fullScreen: false
  readonly property bool fullScreenEnabled: false
  readonly property bool searchEnabled: false
  readonly property size frameMargin: Qt.size(1, 1)

  function closeFullScreen() {
    window.showNormal();
  }

  onLoadingChanged: function (loadRequest) {
    if (loadRequest.status == WebView.LoadSucceededStatus) {
      for (let plugin of App.javaScriptPlugins.plugins) {
        if (plugin.matchesDomain(loadRequest.url)) {
          webView.runJavaScript(plugin.source);
        }
      }
    }
  }
}
