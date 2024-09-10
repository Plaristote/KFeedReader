import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.fluxkap 1.0

Kirigami.FormLayout {
  required property QtObject ttlSettings

  Component.onCompleted: {
    reset();
  }

  function reset() {
    useCustomTtlInput.checked = ttlSettings.useCustomTtl;
    customTtlInput.text = ttlSettings.customTtl.toString();
    customTtlTypeInput.currentIndex = customTtlTypeInput.indexOfValue(FeedUpdater.TtlInMinutes);
    ttlSettings.skipDaysChanged();
    ttlSettings.skipHoursChanged();
  }

  function save() {
    ttlSettings.useCustomTtl = useCustomTtlInput.checked;
    updateFeedTtl();
  }

  function updateFeedTtl() {
    let ttl = parseInt(customTtlInput.text);
    if (isNaN(ttl)) return ;
    ttlSettings.customTtl = ttlSettings.ttlInUnits(ttl, customTtlTypeInput.currentValue);
    ttlSettings.useCustomTtl = useCustomTtlInput.checked;
  }

  Controls.CheckBox {
    id: enableAutoUpdateInput
    text: i18n("Enable periodic updates")
    checkState: [Qt.Unchecked, Qt.PartiallyChecked, Qt.Checked][ttlSettings.autoUpdateEnabled]
    onClicked: {
      console.log("Changed 'enable periodic updates'", checkState, checkState == Qt.PartiallyChecked, checkState == Qt.Checked, checkState == Qt.Unchecked);
      if (checkState != Qt.PartiallyChecked)
        ttlSettings.enableAutoUpdate(checkState == Qt.Checked);
    }
  }

  Controls.CheckBox {
    id: useCustomTtlInput
    text: i18n("Use a custom update interval")
    checked: ttlSettings.useCustomTtl
    onCheckedChanged: updateFeedTtl()
  }

  Row {
    Kirigami.FormData.label: i18n("Custom update interval")
    visible: useCustomTtlInput.checked
    Controls.TextField {
      id: customTtlInput
      validator: IntValidator { bottom: 1 }
      Component.onCompleted: text = ttlSettings.ttlInUnits(ttlSettings.customTtl, customTtlTypeInput.currentValue).toString()
      onEditingFinished: updateFeedTtl()
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

  Row {
    Kirigami.FormData.label: i18n("Skip hours")
    visible: useCustomTtlInput.checked
    Repeater {
      model: 24
      delegate: Column {
        Controls.Label { text: `${index}` }
        Controls.CheckBox {
          id: skipHourCheckbox
          onCheckedChanged: ttlSettings.setSkipHour(index, checked)
          Connections {
            target: ttlSettings
            function onSkipHoursChanged() {
              skipHourCheckbox.checked = ttlSettings.isSkippedHour(index);
            }
          }
        }
      }
    }
  }

  Row {
    Kirigami.FormData.label: i18n("Skip days")
    visible: useCustomTtlInput.checked
    Repeater {
      model: 7
      delegate: Column {
        Controls.Label { text: `${index}` }
        Controls.CheckBox {
          id: skipDayCheckbox
          onCheckedChanged: ttlSettings.setSkipDay(index, checked)
          Connections {
            target: ttlSettings
            function onSkipDaysChanged() {
              skipDayCheckbox.checked = ttlSettings.isSkippedDay(index)
            }
          }
        }
      }
    }
  }
}
