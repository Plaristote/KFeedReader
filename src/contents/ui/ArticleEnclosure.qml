import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import QtMultimedia 5.15
import org.kde.kirigami 2.19 as Kirigami

ColumnLayout {
  property bool fullScreen: false
  property bool withVideo: model.type.startsWith("video/")
  id: layout

  states: [
    State {
      name: "fullscreen"
      when: layout.fullScreen
      ParentChange { target: content; parent: window.fullScreenContainer }
      StateChangeScript { script: window.showFullScreen() }
    },
    State {
      name: "default"
      when: !layout.fullScreen
      ParentChange { target: content; parent: contentContainer }
      StateChangeScript { script: window.showNormal() }
    }
  ]

  Kirigami.Action {
    id: togglePlayAction
    text: i18n("Play")
    shortcut: "space"
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

  Kirigami.Action {
    id: toggleFullScreenAction
    text: i18n("Full screen")
    shortcut: "F11"
    icon.name: "view-fullscreen"
    enabled: layout.withVideo
    onTriggered: layout.fullScreen = !layout.fullScreen
  }

  Kirigami.Action {
    id: exitFullScreen
    text: i18n("Exit full screen")
    shortcut: "ESC"
    enabled: layout.fullScreen
    onTriggered: toggleFullScreenAction.trigger()
  }

  Kirigami.Separator {
    Layout.fillWidth: true
  }

  Kirigami.Heading {
    text: model.title
    level: 3
    visible: model.title.length > 0
  }

  MediaPlayer {
    id: mediaPlayer
    source: model.url
  }

  Item {
    id: contentContainer
    Layout.fillWidth: true
    Layout.preferredHeight: width * 0.6

    ColumnLayout {
      id: content
      anchors.fill: parent

      Rectangle {
        color: "black"
        border.width: 1
        border.color: "lightgray"
        visible: model.type.startsWith("video/")
        Layout.fillWidth: true
        Layout.fillHeight: true

        VideoOutput {
          id: videoOutput
          source: mediaPlayer
          fillMode: VideoOutput.PreserveAspectFit
          anchors.fill: parent
        }
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

        Controls.Button {
          action: toggleFullScreenAction
          visible: withVideo
        }
      }
    }
  }

  Kirigami.InlineMessage {
    Layout.fillWidth: true
    text: mediaPlayer.errorString
    visible: mediaPlayer.errorString.length > 0
  }
}
