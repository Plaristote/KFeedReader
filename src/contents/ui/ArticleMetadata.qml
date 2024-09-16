import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15

RowLayout {
  property QtObject model

  Controls.Button {
    id: dateLabel
    icon.name: "view-calendar-day"
    text: model.publicationDate.toLocaleDateString()
    flat: true
  }

  Controls.Button {
    id: categoryLabel
    visible: model.category.length > 0
    icon.name: "category"
    text: model.category
    flat: true
  }

  Controls.Button {
    id: authorButton
    visible: model.author.length > 0
    icon.name: "user-identity"
    text: model.author
    flat: !model.authorUrl.toString().length
    onClicked: Qt.openUrlExternally(model.authorUrl)
  }
}
