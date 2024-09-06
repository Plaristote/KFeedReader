import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.kirigami.delegates as KirigamiDelegate

Item {
  id: root

  required property string title
  property alias subtitle: titleSubtitle.subtitle
  property alias color: titleSubtitle.color
  property alias subtitleColor: titleSubtitle.color
  property alias font: titleSubtitle.font
  property alias subtitleFont: titleSubtitle.subtitleFont
  property alias reserveSpaceForSubtitle: titleSubtitle.reserveSpaceForSubtitle
  property alias selected: titleSubtitle.selected
  //property alias showTooltip: titleSubtitle.showTooltip
  property alias elide: titleSubtitle.elide
  property alias iconSource: iconImage.source
  property real iconSize: 18
  property bool bold: false
  property alias leading: leadingItem.sourceComponent
  property alias trailing: trailingItem.sourceComponent

  implicitWidth: iconItem.implicitWidth + titleSubtitle.anchors.leftMargin + titleSubtitle.implicitWidth
  implicitHeight: titleSubtitle.implicitHeight

  Loader {
    id: leadingItem
    anchors.left: parent.left
    anchors.verticalCenter: parent.verticalCenter
  }

  Item {
    id: iconItem
    implicitWidth: root.iconSize
    implicitHeight: root.iconSize
    visible: root.iconSize > 0
    anchors {
      left: leadingItem.right
      leftMargin: leadingItem.width > 0 ? Kirigami.Units.mediumSpacing : 0
    }
    Kirigami.Icon {
      id: iconImage
      height: parent.height > root.iconSize ? root.iconSize : parent.height
      width: root.iconSize
      anchors.centerIn: parent
    }
  }

  KirigamiDelegate.TitleSubtitle {
    id: titleSubtitle
    title: root.bold ? `<b>${root.title}</b>` : root.title
    selected: root.parent.action && root.parent.action.checked

    anchors {
      left: iconItem.right
      leftMargin: root.iconSize > 0 ? Kirigami.Units.mediumSpacing : 0
      top: parent.top
      //bottom: parent.bottom
      right: trailingItem.left
      rightMargin: trailingItem.width > 0 ? Kirigami.Units.mediumSpacing : 0
    }
  }

  Loader {
    id: trailingItem
    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter
  }
}
