/** @file AppWindow.qml
 *
 *  @par Copyright:
 *            2014-2018 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2015/10/09
 *
 *  QML ApplicationWindow
 *
 *  needs:
 *  - AppData::log, quitMsg
 *  - AppData::batteryDischarging, readBattery, battery_timer
 *  - AppWindow { VisualItemModel { id: pagesModel; ...
 */

// developed with Qt 5.7-5.10

import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2
import Qt.labs.settings 1.0

import AppData 1.0
import "qrc:/qml"

ApplicationWindow {
    id: mainWindow
    objectName: "mainWindow"
    color: bg
    visible: true
    readonly property bool fullscreen: (Qt.platform.os === "android" ||
                                        Qt.platform.os === "blackberry" ||
                                        Qt.platform.os === "ios" ||
                                        Qt.platform.os === "tvos" ||
                                        Qt.platform.os === "windowsphone")
    width: if( !fullscreen )
               200*dens
    height: if( !fullscreen )
                150*dens
    minimumWidth: 100*dens
    minimumHeight: 75*dens

    readonly property int dens: Math.round(Screen.logicalPixelDensity)
    readonly property string dpiName: {
//        if(density >= 21.26) // 560
//            "xxxxhdpi"
//        else if(density >= 14.173)  // 360
//            "xxxhdpi"
//        else
        if ( Screen.pixelDensity >= 10.630 ) // 270
            "xxhdpi"
        else if ( Screen.pixelDensity >= 7.0866 ) // 180
            "xhdpi"
        else if ( Screen.pixelDensity >= 5.3150 ) // 135
            "hdpi"
        else
            "mdpi"
    }


    property int theme: Material.System
    property int accent: Material.Orange
    property color bg: Material.background
    property color fg: Material.foreground
    property color highlighted: Material.highlightedButtonColor
    Material.theme: theme
    Material.accent: accent
    property bool bright: (bg.r + bg.g) > 1

    property alias myPalette: myPalette
    SystemPalette {
        id: myPalette
        objectName: "myPalette"
        colorGroup: SystemPalette.Active
    }

    Settings {
        objectName: "Settings"
        property alias app_debug: mainWindow.app_debug
        property alias theme: mainWindow.theme
        property alias accent: mainWindow.accent
    }

    property string logo;
    property string icon;
    onIconChanged: {
        appData.setWindowIcon( icon ) // set window icon
    }

    // DEBUG
    property int app_debug: 0
    onApp_debugChanged: appData.setDebug( app_debug )

    Screen.onPrimaryOrientationChanged: {
        if(app_debug)
            statusText = "oPrimOrientCh(): old index: " + pages.currentIndex + "/" + pagesContentIndex + " landscape: " + landscape
    }

    signal fullLogMessage(var logText)
    onFullLogMessage: logHistory = logText + logHistory

    signal batteryInfo(var battery)
    onBatteryInfo: { var s = false
        if(battery === "discharging") s = true
        else s = false
        var update = true
        if(s === batteryDischarging) update = false
        batteryDischarging = s;

        var str = qsTr("discharging"); str = qsTr("charging"); str = qsTr("full")
        statusText = qsTr("Battery") + ": " + qsTr(battery)
    }


    property alias appData: appData
    AppData {
        id: appData;
        objectName: "appData"

        onQuitMessage: statusText = appData.quitMsg
        onLogMessage: logText = appData.log
        onBatteryDischarging: {
            var update = true
            var state = (s === 1) ? true : false
            if(state === mainWindow.batteryDischarging) update = false
            mainWindow.batteryDischarging = state
            if(update)
                statusText = qsTr("Battery") + " " + (s === 1 ? qsTr("discharging") : qsTr("not used"))
        }
    }

    property bool batteryDischarging: false
    property int drawPause: batteryDischarging ? 1000/2 : 1000/20


    // Take busy out of non UI function
    property string timer_status_text_
    property alias setBusyTimer: setBusyTimer
    Timer {
        id: setBusyTimer
        objectName: "setBusyTimer"
        triggeredOnStart: false
        interval: 0
        onTriggered: {
            if(isbusy === false)
            {
                timer_status_text_ = statusText
                statusText = qsTr("Busy")
                isbusy = true
            }
        }
    }
    property alias unsetBusyTimer: unsetBusyTimer
    Timer {
        id: unsetBusyTimer
        objectName: "unsetBusyTimer"
        triggeredOnStart: false
        interval: 200
        onTriggered: {
            if(isbusy === true)
            {
                isbusy = false
                if(timer_status_text_.length)
                    statusText = timer_status_text_
                timer_status_text_ = ""
            }
        }
    }

    property var closeFunction: function() { /* do nothing */ }
    Shortcut {
        objectName: "ShortcutQuit"
        sequence: StandardKey.Quit
        onActivated: {
            closeFunction()
            close()
        }
    }
    Shortcut { // leave help page or iconise window
        objectName: "ShortcutCancel"
        sequence: StandardKey.Cancel
        onActivated: {  // assume help page is on page 3
            if(pages.currentIndex !== 3)
            {
                if(Qt.platform.os === "android")
                {
                    closeFunction()
                    close()
                } else
                    showMinimized()
            } else {
                pages.currentIndex = 3
                setPage(-1)
            }
        }
    }


    property int landscape: 0
    function checkLandscape ()
    {
        if(app_debug)
            statusText = "checkLand(): old index: " + pages.currentIndex + "/" + pagesContentIndex + " landscape: " + landscape + " indexAt(): " + pages.indexAt(1,1)
        if(width > height)
            landscape = 1
        else
            landscape = 0
    }
    onLandscapeChanged: {
        if(app_debug)
            statusText = "onLandCh(): old index: " + pages.currentIndex + "/" + pagesContentIndex + " landscape: " + landscape + " indexAt(): " + pages.indexAt(1,1)
        if(pages.currentIndex < 2)
            setPage( 0 )
        //pages.interactive = !landscape
    }

    property bool init: true
    Component.onCompleted: {
        checkLandscape()
        if(app_debug) {
            appData.log = "dens: " + dens
            statusText = "bg: " + bg.r.toFixed(2) + " " + bg.g.toFixed(2) + " " + bg.b.toFixed(2) + " bright: " + bright
        }
        init = 0
    }
    onWidthChanged: checkLandscape()
    onHeightChanged: checkLandscape()


    property alias textMetrics: textMetrics
    TextMetrics {
        id: textMetrics
        objectName: "textMetrics"
    }

    property string statusText
    property string logText
    property string logHistory
    onStatusTextChanged: {
        appData.log = "statusText: " + statusText
        logText = statusText
    }

    Shortcut { // move
        objectName: "ShortcutRight"
        sequence: StandardKey.MoveToNextChar
        onActivated: {
            var pCI = pagesContentIndex
            var i = pCI + 1
            setPage(i)
        }
    }
    Shortcut { // move
        objectName: "ShortcutLeft"
        sequence: StandardKey.MoveToPreviousChar
        onActivated: setPage(pagesContentIndex - 1)
    }
    property int pagesContentIndex: 0
    onPagesContentIndexChanged: if(app_debug) statusText = "pagesContentIndex: " + pagesContentIndex
    function setPage(page)
    {
        if(app_debug)
            statusText = "setPage("+page+") old index: " + pages.currentIndex + "/" + pagesContentIndex + " landscape: " + landscape

        if(page === -1)
            page = 0

        // assume te first two pages being each a HalfPage in landscape mode
        if(landscape && page < 2 && page > pagesContentIndex)
        {
            page = 2
        } else
        if(landscape && page < 2)
        {
            page = 0
        } else
        if(landscape === 0 && page < 0)
        {
            page = 0
        }

        if(page <= 4) // remember any page except the help page
            pagesContentIndex = page

        pages.currentIndex = page = pagesContentIndex
        var i = 0
        var count = pagesModel.count
        for (i = 0; i < count; ++i)
        {
            var p = pagesModel.get(i)
            // is not needed as visibility and focus are handled in swipe aware pages.onContentXChanged
            /*if(i === page)
                p.visible = true
            else if(landscape)
            {
                if(p.width < mainWindow.width && i >= page && page <= i + 1 ) // half page
                    p.visible = true
                else
                    p.visible = false
            }
            else
                p.visible = false*/
            if(app_debug)
                statusText = p.objectName + " " + i + " " + (p.visible?"+":"-") + " " + p.width  + "/" + mainWindow.width
        }
    }

    property string appDataJsonString: "someJSON"
    property bool showJson: true // used by the JSON/log history page; the log would be pretty slow


    property color high_light_color: Material.accent
    property real button_height: (pages.height - 2 * font.pixelSize * 3 - 2*dens)
    property real h: font.pixelSize * 3

    property alias pages: pages
    ListView {
        id: pages
        objectName: "pages"

        onXChanged: if(app_debug) statusText = "onXChan x: " + x
        width: parent.width
        height: mainWindow.height - mainStatusBar.height

        model: pagesModel
        currentIndex: 0
        orientation: ListView.Horizontal
        snapMode: ListView.SnapToItem
        flickDeceleration: 2000

        highlightFollowsCurrentItem: true
        highlightMoveDuration: 500
        focus: true

        onContentXChanged: {
            if(app_debug)
                statusText = "onContentXChanged " + contentX
            var count = pagesModel.count
            var start = 0
            var end = 0
            var i
            // set page focus
            for (i = 0; i < count; ++i)
            {
                var p = pagesModel.get(i)
                var w = p.width
                end += w
                var x = Math.round(contentX)
                if(start <= x && x < end)
                {
                    p.visible = true
                    p.focus = true
                } else if(start - pages.width < x && x < end)
                {
                    p.visible = true
                    p.focus = false
                } else {
                    p.visible = false
                    p.focus = false
                }
                if(app_debug)
                    statusText = p.objectName + " " + i + " " + (p.visible?"+":"-") + " " + p.width  + "/" + mainWindow.width + " " + start + " - " + end
                start += w
            }
        }
        onCurrentIndexChanged: {
            if(app_debug)
                statusText = "onCurrIndxCh(): currentIndex: " + currentIndex + "/" + pagesContentIndex + "/" + pagesModel.children[0].x + " landscape: " + landscape + " indexAt(): " + indexAt(1,1)
        }
        Shortcut {
            sequence: StandardKey.HelpContents
            onActivated: setPage(3)
        }
    }


    footer: Rectangle {
        id: mainStatusBar
        objectName: "mainStatusBar"
        height: font.pixelSize*2
        color: bg
        Text {
            height: parent.height
            //anchors.verticalCenter: parent
            text: logText
            color: fg
        }
    }

    property bool isbusy: false
    BusyIndicator {
       id: busy
       objectName: "busy"
       anchors.centerIn: parent
       running: isbusy
       opacity: 0.85
       //Layout.alignment: Qt.AlignLeft
    }
}
