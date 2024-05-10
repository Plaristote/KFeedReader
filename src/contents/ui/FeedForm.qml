import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0
import "."

Kirigami.ScrollablePage {
  required property QtObject feed
  id: page

  signal confirmed()

  Component.onCompleted: {
    xmlUrlField.forceActiveFocus();
  }

  function reset() {
    xmlUrlField.text = page.feed.xmlUrl;
    ttlSettingsForm.reset();
  }

  function save() {
    page.feed.xmlUrl = xmlUrlField.text;
    ttlSettingsForm.save();
  }

  function updateXmlUrl() {
    if (page.feed.xmlUrl.toString() != xmlUrlField.text) {
      page.feed.xmlUrl = xmlUrlField.text
      page.feed.fetch()
    }
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
      visible: page.feed.fetching
      Layout.fillWidth: true
    }

    Kirigami.FormLayout {
      id: feedDetails
      visible: !page.feed.fetching && (page.feed.name.length > 0)

      Controls.Label {
        Kirigami.FormData.label: i18n("Title")
        text: page.feed.name
      }

      Item {
        Kirigami.FormData.label: i18n("Favicon")
        implicitWidth: 32
        implicitHeight: 32
        visible: page.feed.faviconUrl != null
        Image {
          source: page.feed.faviconUrl
          anchors.fill: parent
        }
      }

      Controls.Label {
        Kirigami.FormData.label: i18n("Language")
        text: page.feed.language
        visible: page.feed.language.length > 0
      }

      Text {
        Kirigami.FormData.label: i18n("Link")
        text: `<a href="${page.feed.link}">${page.feed.link}</a>`
        onLinkActivated: Qt.openUrlExternally(link)
        visible: page.feed.link.toString().length > 0
      }

      Text {
        Kirigami.FormData.label: i18n("Description")
        text: page.feed.description
        visible: page.feed.description.length > 0
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
      }

      Kirigami.Separator {
        Layout.fillWidth: true
      }

      Text {
        Kirigami.FormData.label: i18n("Suggested update interval")
        text: {
          if (page.feed.ttl)
            return `${page.feed.ttl} ${i18n("minutes")}`;
          return i18n("No suggested update interval");
        }
      }
    }

    TtlSettingsForm {
      id: ttlSettingsForm
      ttlSettings: page.feed
    }

    Controls.Button {
      action: confirmAction
    }
  }

  Timer {
    id: urlUpdateTimer
    interval: 1500
    onTriggered: updateXmlUrl()
  }

  actions: [
    Kirigami.Action {
      id: confirmAction
      text: i18n("Confirm")
      enabled: !page.feed.fetching && page.feed.name.length > 0
      icon.name: "dialog-ok-apply"
      tooltip: i18n("Adds this feed to the current folder")
      onTriggered: page.confirmed()
    },
    Kirigami.Action {
      text: i18n("Cancel")
      icon.name: "dialog-cancel"
      tooltip: i18n("Discard changes and go back to the previous page")
      onTriggered: pageStack.pop();
    }
  ]
}
