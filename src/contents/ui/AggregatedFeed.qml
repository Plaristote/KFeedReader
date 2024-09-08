import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0 as KFeedReader
import "."

Feed {
  id: page
  readOnly: true
  leadingFeedIcon: true
  fetching: model.parent.fetching
  progress: model.parent.progress
  Component.onCompleted: model.enabled = true
  Component.onDestruction: model.enabled = false
}
