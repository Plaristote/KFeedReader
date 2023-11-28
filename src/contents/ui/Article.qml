import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0
import "."

Kirigami.ScrollablePage {
  property QtObject model

  id: page
  title: model.title
  actions.contextualActions: pageActions.contextualActions
  actions.main: Kirigami.Action {
    text: i18n("View")
    icon.name: "quickview"
    tooltip: i18n("Open the article link in the application")
    onTriggered: {
      pageStack.replace(Qt.resolvedUrl("BrowserView.qml"), { model: page.model });
    }
  }

  Component.onCompleted: { page.model.read = true; }

  ArticleActions {
    id: pageActions
    model: page.model
  }

  ColumnLayout {
    anchors.left: parent.left
    anchors.right: parent.right

    RowLayout {
      id: authorRow
      visible: page.model.author.length > 0
      spacing: 5
      Controls.Label {
        text: i18n("Author")
        Layout.fillWidth: true
      }
      Text {
        text: page.model.author
      }
      Controls.Button {
        icon.name: "link"
        visible: page.model.authorUrl.toString() != ""
        onClicked: Qt.openUrlExternally(page.model.authorUrl)
      }
    }

    Kirigami.Separator {
      Layout.fillWidth: true
      visible: authorRow.visible
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
}
