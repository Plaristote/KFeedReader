import QtQuick 2.15
import org.kde.fluxkap 1.0 as FluxKap
import "."

FeedForm {
  required property QtObject parentFolder

  feed: FluxKap.Feed {
    id: newFeed
  }
  onConfirmed: {
    save();
    parentFolder.addItem(newFeed);
    pageStack.pop();
  }
}
