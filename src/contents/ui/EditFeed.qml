import QtQuick 2.15
import org.kde.fluxkap 1.0 as FluxKap
import "."

FeedForm {
  property QtObject source

  Component.onCompleted: {
    feedModel.copy(source);
    reset();
  }

  feed: FluxKap.Feed {
    id: feedModel
  }
  onConfirmed: {
    save();
    source.copy(feedModel);
  }
}
