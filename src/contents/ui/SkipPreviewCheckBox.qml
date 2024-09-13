import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls

Controls.CheckBox {
  required property QtObject model
  tristate: true
  checkState: {
    switch (model.skipPreviewSetting) {
      case 0: return Qt.PartiallyChecked;
      case 1: return Qt.Unchecked;
      case 2: return Qt.Checked;
    }
  }
  nextCheckState: function() {
    return checkState === Qt.Checked ? Qt.Unchecked : Qt.Checked;
  }
  onCheckStateChanged: {
    model.skipPreviewSetting = checkState === Qt.Checked ? 2 : 1;
  }
}
