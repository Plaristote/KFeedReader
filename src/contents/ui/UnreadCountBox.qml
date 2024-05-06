import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0

Rectangle {
  property QtObject model

  visible: model.unreadCount > 0
  color: Kirigami.Theme.highlightColor
  implicitHeight: 16
  implicitWidth: unreadText.width + 10
  Text {
    id: unreadText
    anchors.centerIn: parent
    text: model.unreadCount
    color: Kirigami.Theme.highlightedTextColor
  }
}
