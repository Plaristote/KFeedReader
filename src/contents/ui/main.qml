// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kfeedreader 1.0
import "."
import QtQuick.Dialogs 1.3

Kirigami.ApplicationWindow {
    property alias fullScreenContainer: fullScreenContainer
    id: window

    title: i18n("KFeedReader")

    minimumWidth: Kirigami.Units.gridUnit * 20
    minimumHeight: Kirigami.Units.gridUnit * 20

    onClosing: App.saveWindowGeometry(window)

    onWidthChanged: saveWindowGeometryTimer.restart()
    onHeightChanged: saveWindowGeometryTimer.restart()
    onXChanged: saveWindowGeometryTimer.restart()
    onYChanged: saveWindowGeometryTimer.restart()

    Component.onCompleted: App.restoreWindowGeometry(window)

    // This timer allows to batch update the window size change to reduce
    // the io load and also work around the fact that x/y/width/height are
    // changed when loading the page and overwrite the saved geometry from
    // the previous session.
    Timer {
        id: saveWindowGeometryTimer
        interval: 1000
        onTriggered: App.saveWindowGeometry(window)
    }

    property int counter: 0

    globalDrawer: Kirigami.GlobalDrawer {
        title: i18n("KFeedReader")
        titleIcon: "applications-graphics"
        isMenu: !window.isMobile
        actions: [
            Kirigami.Action {
                text: i18n("Import feeds...")
                icon.name: "document-import"
                onTriggered: importFeedsDialog.open()
            },
            Kirigami.Action {
                text: i18n("Export feeds...")
                icon.name: "document-export"
                onTriggered: exportFeedsDialog.open()
            },
            Kirigami.Action {
                text: i18n("About KFeedReader")
                icon.name: "help-about"
                //onTriggered: pageStack.layers.pushDialogLayer('qrc:About.qml')
                onTriggered: pageStack.layers.push('qrc:About.qml')
            },
            Kirigami.Action {
                text: i18n("Quit")
                icon.name: "application-exit"
                onTriggered: Qt.quit()
            }
        ]
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    pageStack.initialPage: {
      switch (App.rootFolder.displayType) {
      case FeedFolder.ListDisplay: return listRootPage;
      case FeedFolder.TreeDisplay: return treeRootPage;
      }
    }

    Component {
      id: treeRootPage
      FolderTreePage {
        model: App.rootFolder
      }
    }

    Component {
      id: listRootPage
      FolderListPage {
        model: App.rootFolder
      }
    }

    Rectangle {
        id: fullScreenContainer
        color: children.length ? "black" : "transparent"
        anchors.fill: parent
    }

    FileDialog {
        id: importFeedsDialog
        title: i18n("Pick a feed source to import")
        selectExisting: true
        nameFilters: [i18n("OPML structures") + " (*.opml)"]
        folder: shortcuts.home
        onAccepted: App.importOpml(fileUrl)
    }

    FileDialog {
        id: exportFeedsDialog
        title: i18n("Pick a file to export your feeds to")
        selectExisting: false
        nameFilters: [i18n("OPML structures") + " (*.opml)"]
        folder: shortcuts.home
        onAccepted: App.exportOpml(fileUrl)
    }
}
