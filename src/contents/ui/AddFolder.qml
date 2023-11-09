import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0

Kirigami.ScrollablePage {
  property QtObject model
  id: page

  Component.onCompleted: {
    nameField.forceActiveFocus();
  }

  FeedFolder {
    id: newFolder
  }

  Kirigami.FormLayout {
    id: form
    Controls.TextField {
      id: nameField
      Kirigami.FormData.label: i18n("Name")
      text: newFolder.name
    }

    Controls.TextField {
      id: descriptionField
      Kirigami.FormData.label: i18n("Description")
      text: newFolder.description
    }

    Controls.Button {
      action: confirmAction
    }
  }

  actions {
    main: Kirigami.Action {
      id: confirmAction
      text: i18n("Confirm")
      enabled: nameField.text.length > 0
      icon.name: "dialog-ok-apply"
      tooltip: i18n("Create a new folder")
      onTriggered: {
        newFolder.name = nameField.text;
        newFolder.description = descriptionField.text;
        page.model.addItem(newFolder);
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
