import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import "."

Controls.MenuBar {
  required property var globalActions
  required property QtObject pageStack
  id: menuBarRoot

  function getArticleItem() {
    const item = window.pageStack.lastItem;
    const objectId = item.model.toString();
    return objectId.indexOf("FeedArticle(") == 0
      ? item : null;
  }

  function getFeedItem() {
    const articleItem = getArticleItem();
    const item = articleItem != null
      ? window.pageStack.items[window.pageStack.depth - 2]
      : window.pageStack.lastItem;
    const objectId = item.model.toString();
    return objectId.indexOf("Feed(") >= 0
      ? item : null;
  }

  Controls.Menu {
    id: fileMenu
    title: i18n("&File")

    Instantiator {
      model: globalActions.length
      delegate: Controls.MenuItem {
        action: globalActions[index]
      }
      onObjectAdded: (index, object) => fileMenu.insertItem(index, object)
      onObjectRemoved: (index, object) => fileMenu.removeItem(object)
    }
  }

  Component {
    id: feedMenuComponent
    PageMenu {
      title: i18n("Feed")
      pageItem: getFeedItem(window.pageStack.lastItem)
    }
  }

  Component {
    id: articleMenuComponent
    PageMenu {
      title: i18n("Article")
      pageItem: getArticleItem(window.pageStack.lastItem)
    }
  }

  Loader {
    property var previouslyAddedItem
    sourceComponent: getFeedItem(menuBarRoot.pageStack.lastItem) != null ? feedMenuComponent : null
    onItemChanged: {
      if (previouslyAddedItem) {
        menuBarRoot.removeMenu(previouslyAddedItem);
      }
      if (item) {
        menuBarRoot.addMenu(item)
        previouslyAddedItem = item;
      }
    }
  }

  Loader {
    property var previouslyAddedItem
    sourceComponent: getArticleItem(menuBarRoot.pageStack.lastItem) != null ? articleMenuComponent : null
    onItemChanged: {
      if (previouslyAddedItem) {
        menuBarRoot.removeMenu(previouslyAddedItem);
      }
      if (item) {
        menuBarRoot.addMenu(item)
        previouslyAddedItem = item;
      }
    }
  }
}
