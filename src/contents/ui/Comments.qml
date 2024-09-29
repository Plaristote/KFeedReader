import QtQuick 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import "."

BrowserView {
  required property QtObject model

  url: model.comments
}
