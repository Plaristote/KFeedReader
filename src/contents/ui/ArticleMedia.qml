import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0

ColumnLayout {
  id: layout

  Kirigami.Separator {
    Layout.fillWidth: true
  }

  Kirigami.Heading {
    text: model.title
    level: 3
  }

  Text {
    text: model.description
  }

  Image {
    source: model.thumbnailUrl
    fillMode: Image.PreserveAspectCrop
    Layout.preferredWidth: model.thumbnailSize.width > layout.width ? layout.width : model.thumbnailSize.width
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
}
