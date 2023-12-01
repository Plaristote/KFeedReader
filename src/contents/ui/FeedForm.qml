import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0

Kirigami.ScrollablePage {
  required property QtObject feed
  id: page

  signal confirmed()

  Component.onCompleted: {
    xmlUrlField.forceActiveFocus();
  }

  function reset() {
    xmlUrlField.text = page.feed.xmlUrl;
    useCustomTtlInput.checked = page.feed.useCustomTtl;
    customTtlInput.text = page.feed.customTtl.toString();
    customTtlTypeInput.currentIndex = customTtlTypeInput.indexOfValue(FeedUpdater.TtlInMinutes);
  }

  function save() {
    page.feed.xmlUrl = xmlUrlField.text;
    console.log("Saving custom ttl", parseInt(customTtlInput.text), customTtlTypeInput.currentValue, page.feed.ttlInUnits(parseInt(customTtlInput.text), customTtlTypeInput.currentValue));
    page.feed.customTtl = page.feed.ttlInUnits(parseInt(customTtlInput.text), customTtlTypeInput.currentValue);
    page.feed.useCustomTtl = useCustomTtlInput.checked;
  }

  function updateXmlUrl() {
    if (page.feed.xmlUrl.toString() != xmlUrlField.text) {
      page.feed.xmlUrl = xmlUrlField.text
      page.feed.fetch()
    }
  }

  function updateFeedTtl() {
    let ttl = parseInt(customTtlInput.text);
    switch (customTtlTypeInput.currentValue) {
      case FeedUpdater.TtlInHours:
        ttl = ttl * 60;
        break ;
    }
    page.feed.useCustomTtl = useCustomTtlInput.checked;
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

      Controls.CheckBox {
        id: useCustomTtlInput
        text: i18n("Use a custom update interval")
        checked: page.feed.useCustomTtl
        onCheckedChanged: updateFeedTtl()
      }

      Row {
        Kirigami.FormData.label: i18n("Suggested update interval")
        visible: useCustomTtlInput.checked
        Controls.TextField {
          id: customTtlInput
          validator: IntValidator { bottom: 0 }
          text: page.feed.ttlInUnits(page.feed.customTtl, customTtlTypeInput.currentValue).toString()
        }
        Controls.ComboBox {
          id: customTtlTypeInput
          textRole: "text"
          valueRole: "value"
          model: [
            { value: FeedUpdater.TtlInMinutes, text: i18n("minutes") },
            { value: FeedUpdater.TtlInHours, text: i18n("hours") }
          ]
          onCurrentValueChanged: updateFeedTtl()
        }
      }
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

  actions {
    main: Kirigami.Action {
      id: confirmAction
      text: i18n("Confirm")
      enabled: !page.feed.fetching && page.feed.name.length > 0
      icon.name: "dialog-ok-apply"
      tooltip: i18n("Adds this feed to the current folder")
      onTriggered: page.confirmed()
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
