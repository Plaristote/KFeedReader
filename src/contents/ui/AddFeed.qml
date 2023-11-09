import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0

Kirigami.ScrollablePage {
  property QtObject model
  id: page

  Component.onCompleted: {
    xmlUrlField.forceActiveFocus();
  }

  Feed {
    id: newFeed
  }

  ColumnLayout {
    Kirigami.FormLayout {
      id: form

      Controls.TextField {
        id: xmlUrlField
        Kirigami.FormData.label: i18n("URL")
        onTextChanged: urlUpdateTimer.start()
      }
    }

    Controls.BusyIndicator {
      visible: newFeed.fetching
      Layout.fillWidth: true
    }

    Kirigami.FormLayout {
      id: feedDetails
      visible: !newFeed.fetching && (newFeed.name.length > 0)

      Controls.Label {
        Kirigami.FormData.label: i18n("Title")
        text: newFeed.name
      }

      Image {
        Kirigami.FormData.label: i18n("Favicon")
        source: newFeed.faviconUrl
        visible: newFeed.faviconUrl != null
        width: 16
        height: 16
      }

      Controls.Label {
        Kirigami.FormData.label: i18n("Language")
        text: newFeed.language
        visible: newFeed.language.length > 0
      }

      Controls.Label {
        Kirigami.FormData.label: i18n("Link")
        text: newFeed.link
      }

      Controls.Label {
        Kirigami.FormData.label: i18n("Description")
        text: newFeed.description
        visible: newFeed.description.length > 0
      }
    }

    Controls.Button {
      action: confirmAction
    }
  }

  Timer {
    id: urlUpdateTimer
    interval: 1500
    onTriggered: newFeed.xmlUrl = xmlUrlField.text
  }

  actions {
    main: Kirigami.Action {
      id: confirmAction
      text: i18n("Confirm")
      enabled: !newFeed.fetching && newFeed.name.length > 0
      icon.name: "dialog-ok-apply"
      tooltip: i18n("Adds this feed to the current folder")
      onTriggered: {
        page.model.addItem(newFeed);
        pageStack.pop();
      }
    }
    contextualActions: [
      Kirigami.Action {
        text: i18n("Cancel")
        icon.name: "dialog-cancel"
        tooltip: i18n("Discard changes and go back to the previous page")
        onTriggered: pageStack.pop();
      }
    ]
  }
}
