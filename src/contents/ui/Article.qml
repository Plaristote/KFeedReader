import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0
import QtWebEngine 1.10

Kirigami.ScrollablePage {
  property QtObject model
  id: page
  title: model.title

  Component.onCompleted: {
    page.model.read = true;
  }

  ColumnLayout {
    anchors.left: parent.left
    anchors.right: parent.right

    Item {
      implicitHeight: authorRow.height
      implicitWidth: authorRow.width
      RowLayout {
        id: authorRow
        visible: page.model.author.length > 0
        spacing: 5
        Controls.Label {
          text: i18n("Author")
        }
        Text {
          text: page.model.author
        }
      }
      MouseArea {
        anchors.fill: parent
        enabled: page.model.authorUrl != null
        cursorShape: Qt.PointingHandCursor
        onClicked: Qt.openUrlExternally(page.model.authorUrl)
      }
    }

    Kirigami.Separator {
      Layout.fillWidth: true
    }

    Text {
      text: page.model.description
      wrapMode: Text.WordWrap
      Layout.fillWidth: true
    }

    Repeater {
      model: page.model.medias
      delegate: Loader {
        property QtObject model: page.model.medias[index]
        source: model.qmlView()
        Layout.fillWidth: true
      }
    }
  }

  actions.contextualActions: [
    Kirigami.Action {
      text: i18n("Open")
      icon.name: "internet-web-browser"
      tooltip: i18n("Open the current article in a web browser")
      onTriggered: Qt.openUrlExternally(page.model.link)
    },
    Kirigami.Action {
      text: i18n("Mark as unread")
      onTriggered: page.model.read = false
    }
  ]
  actions.main: Kirigami.Action {
    text: i18n("View")
    icon.name: "quickview"
    tooltip: i18n("Open the article link in the application")
    onTriggered: {
      pageStack.push(Qt.resolvedUrl("BrowserView.qml"), { url: page.model.link });
    }
  }
}
