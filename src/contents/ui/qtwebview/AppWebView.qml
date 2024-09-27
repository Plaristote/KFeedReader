import QtQuick
import QtWebView
import QtQuick.Controls as Controls

WebView {
  id: webView
  property bool fullScreen: false
  readonly property bool fullScreenEnabled: false
  readonly property bool searchEnabled: false
  readonly property size frameMargin: Qt.size(1, 1)

  function closeFullScreen() {
    window.showNormal();
  }
}
