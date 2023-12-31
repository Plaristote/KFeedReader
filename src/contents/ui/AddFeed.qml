import QtQuick 2.15
import org.kde.kfeedreader 1.0 as KFeedReader
import "."

FeedForm {
  required property QtObject parentFolder

  feed: KFeedReader.Feed {
    id: newFeed
  }
  onConfirmed: {
    save();
    parentFolder.addItem(newFeed);
    pageStack.pop();
  }
}
