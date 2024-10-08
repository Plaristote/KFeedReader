import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.fluxkap 1.0

ColumnLayout {
  property alias contentComponent: content.sourceComponent
  property size mediaSize: model.thumbnailSize || Qt.size(640, 360)
  property size contentSize: mediaSize
  property bool fullScreen: false
  id: layout

  states: [
    State {
      name: "fullscreen"
      when: layout.fullScreen
      ParentChange { target: content; parent: window.fullScreenContainer }
      StateChangeScript { script: window.showFullScreen() }
    },
    State {
      name: "default"
      when: !layout.fullScreen
      ParentChange { target: content; parent: contentContainer }
      StateChangeScript { script: window.showNormal() }
    }
  ]

  Kirigami.Separator {
    Layout.fillWidth: true
  }

  Kirigami.Heading {
    text: model.title
    level: 3
  }

  Component {
    id: thumbnailComponent
    Image {
      source: model.thumbnailUrl
      fillMode: Image.PreserveAspectCrop
    }
  }

  Item {
    id: contentContainer
    Layout.preferredWidth: contentSize.width
    Layout.preferredHeight: contentSize.height
    Loader {
      id: content
      sourceComponent: thumbnailComponent
      anchors.fill: parent
    }
  }

  GridLayout {
    columns: 2
    Controls.Label {
      text: i18n("Rating")
      visible: model.starRating > 0
    }
    Text {
      text: model.starRating
      visible: model.starRating > 0
    }
    Controls.Label {
      text: i18n("View count")
      visible: model.viewCount > 0
    }
    Text {
      text: model.viewCount
      visible: model.viewCount > 0
    }
  }

  Kirigami.Separator {
    Layout.fillWidth: true
  }

  TextEdit {
    Layout.fillWidth: true
    text: model.descriptionAsRichText
    readOnly: true
    wrapMode: Text.WordWrap
    selectByMouse: true
    textFormat: TextEdit.RichText
    onLinkActivated: Qt.openUrlExternally(link)
  }
}
