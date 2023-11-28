import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import org.kde.kirigami 2.19 as Kirigami

ColumnLayout {
  Kirigami.Action {
    id: togglePlayAction
    text: i18n("Play")
    icon.name: mediaPlayer.playbackState != MediaPlayer.PlayingState
      ? "media-playback-start"
      : "media-playback-pause"
    onTriggered: {
      if (mediaPlayer.playbackState == MediaPlayer.PlayingState)
        mediaPlayer.pause();
      else
        mediaPlayer.play();
    }
  }

  Kirigami.Separator {
    Layout.fillWidth: true
  }

  MediaPlayer {
    id: mediaPlayer
    source: model.url
  }

  Rectangle {
    color: "black"
    border.width: 1
    border.color: "lightgray"
    visible: model.type.startsWith("video/")
    Layout.fillWidth: true
    Layout.preferredHeight: width * 0.6

    VideoOutput {
      id: videoOutput
      source: mediaPlayer
      fillMode: VideoOutput.PreserveAspectFit
      anchors.fill: parent
    }
  }

  Kirigami.InlineMessage {
    Layout.fillWidth: true
    text: mediaPlayer.errorString
    visible: mediaPlayer.errorString.length > 0
  }

  RowLayout {
    Layout.fillWidth: true

    Controls.Button {
      action: togglePlayAction
    }

    Controls.Slider {
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
