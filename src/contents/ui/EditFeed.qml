import QtQuick 2.15
import org.kde.kfeedreader 1.0 as KFeedReader
import "."

FeedForm {
  property QtObject source

  Component.onCompleted: {
    feedModel.copy(source);
    reset();
  }

  feed: KFeedReader.Feed {
    id: feedModel
  }
  onConfirmed: {
    save();
    source.copy(feedModel);
  }
}
