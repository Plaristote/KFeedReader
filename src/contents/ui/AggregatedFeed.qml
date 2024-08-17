import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0 as KFeedReader
import "."

Feed {
  property QtObject folder
  property alias showUnreadOnly: aggregatedFeed.onlyUnread
  id: page
  readOnly: true
  leadingFeedIcon: true

  model: KFeedReader.AggregatedFeed {
    id: aggregatedFeed
    Component.onCompleted: {
      addFolder(page.folder);
    }
  }
}
