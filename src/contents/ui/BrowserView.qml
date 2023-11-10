import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtWebEngine 1.10

Kirigami.Page {
  property alias url: webview.url
  title: webview.title

  WebEngineView {
    id: webview
    anchors.fill: parent
  }
}
