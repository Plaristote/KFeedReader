import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

Kirigami.AbstractCard {
  required property QtObject model

  visible: model.hasTextInput

  Kirigami.Action {
    id: searchAction
    text: model.textInputTitle
    onTriggered: {
      const url = `${model.textInputLink}${model.textInputName}=${encodeURIComponent(textInput.text)}`;
      Qt.openUrlExternally(url);
    }
  }

  RowLayout {
    Controls.TextField {
      id: textInput
      placeholderText: model.textInputDescription
    }
    Controls.Button {
      action: searchAction
    }
  }
}
