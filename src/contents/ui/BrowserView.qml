import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls
import org.kde.kirigami 2.19 as Kirigami
import "."

Kirigami.Page {
  property alias url: webview.url
  property alias topBar: topBarLoader.sourceComponent
  property alias fullTitle: webview.title

  function toggleSearch() {
    if (webview.searchEnabled) {
      searchField.visible = !searchField.visible;
      if (searchField.visible)
        searchTextField.forceActiveFocus()
    }
  }

  Shortcut {
    sequence: "Ctrl+F"
    enabled: webview.searchEnabled
    onActivatedAmbiguously: toggleSearch()
    onActivated: toggleSearch()
  }

  id: page
  title: {
    if (webview.title.length > window.titleMaxLength) {
      return webview.title.slice(0, window.titleMaxLength) + '…';
    }
    return webview.title;
  }
  states: [
    State {
      name: "fullScreen"
      when: webview.fullScreen
      ParentChange { target: webview; parent: window.fullScreenContainer }
    },
    State {
      name: "default"
      when: !webview.fullScreen
      ParentChange { target: webview; parent: webviewContainer }
    }
  ]

  ColumnLayout {
    anchors.fill: parent

    Loader {
      id: topBarLoader
      Layout.fillWidth: true
    }

    Item {
      id: webviewContainer
      Layout.fillWidth: true
      Layout.fillHeight: true
      AppWebView {
        id: webview
        url: page.url
        anchors.fill: parent
      }
    }

    RowLayout {
      id: searchField
      visible: false
      Controls.Label { text: i18n("Search") }
      Controls.TextField {
        id: searchTextField
        Layout.fillWidth: true
        onEditingFinished: webview.findText(text)
      }
      Controls.Button {
        action: Controls.Action {
          icon.name: "search"
          onTriggered: webview.findText(searchTextField.text)
        }
      }
      Controls.Button {
        action: Controls.Action {
          icon.name: "dialog-close"
          shortcut: Shortcut {
            sequence: "Esc"
            enabled: searchField.visible
            onActivated: searchField.visible = false
            onActivatedAmbiguously: searchField.visible = false
          }
          onTriggered: searchField.visible = false
        }
      }
    }
  }
}
