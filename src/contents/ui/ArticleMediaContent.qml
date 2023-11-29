import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import org.kde.kirigami 2.19 as Kirigami
import "."

ArticleMedia {
  id: mediaRoot
  property url mediaUrl: model.contentUrl
  contentComponent: mediaComponent

  Component {
    id: mediaComponent
    ColumnLayout {
      MediaPlayer {
        id: mediaPlayer
        source: model.mediaUrl
      }

      VideoOutput {
        source: mediaPlayer
        fillMode: VideoOutput.PreserveAspectFit
        implicitHeight: contentSize.height
        implicitWidth: contentSize.width
      }

      Kirigami.InlineMessage {
        Layout.fillWidth: true
        text: mediaPlayer.errorString
        visible: mediaPlayer.errorString.length > 0
      }

      RowLayout {
        Layout.fillWidth: true

        Controls.Button {
          Layout.fillWidth: true
          from: 0
          to: mediaPlayer.duration
          value: mediaPlayer.position
          onValueChanged: {
            if (value != mediaPlayer.position)
              mediaPlayer.seek(value);
          }
        }
      }
    }
  }
}
