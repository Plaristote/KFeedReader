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

  function reset() {
    xmlUrlField.text = newFeed.xmlUrl;
    useCustomTtlInput.checked = newFeed.useCustomTtl;
    customTtlInput.text = newFeed.customTtl;
    customTtlTypeInput.currentIndex = customTtlTypeInput.indexOfValue(Feed.TtlInMinutes);
  }

  function updateFeedTtl() {
    let ttl = parseInt(customTtlInput.text);
    switch (customTtlTypeInput.currentValue) {
      case Feed.TtlInHours:
        ttl = ttl * 60;
        break ;
    }
    newFeed.useCustomTtl = useCustomTtlInput.checked;
    newFeed.customTtl = ttl;
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

      Text {
        Kirigami.FormData.label: i18n("Description")
        text: newFeed.description
        visible: newFeed.description.length > 0
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
      }

      Kirigami.Separator {
        Layout.fillWidth: true
      }

      Text {
        Kirigami.FormData.label: i18n("Suggested update interval")
        text: {
          if (newFeed.ttl)
            return `${newFeed.ttl} ${i18n("minutes")}`;
          return i18n("No suggested update interval");
        }
      }

      Controls.CheckBox {
        id: useCustomTtlInput
        text: i18n("Use a custom update interval")
        checked: newFeed.useCustomTtl
        onCheckedChanged: updateFeedTtl()
      }

      Row {
        Kirigami.FormData.label: i18n("Suggested update interval")
        visible: useCustomTtlInput.checked
        Controls.TextField {
          id: customTtlInput
          validator: IntValidator { bottom: 0 }
          text: newFeed.customTtl.toString()
          onTextChanged: updateFeedTtl()
        }
        Controls.ComboBox {
          id: customTtlTypeInput
          textRole: "text"
          valueRole: "value"
          model: [
            { value: Feed.TtlInMinutes, text: i18n("minutes") },
            { value: Feed.TtlInHours, text: i18n("hours") }
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
    onTriggered: {
      newFeed.xmlUrl = xmlUrlField.text
      newFeed.fetch()
    }
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
