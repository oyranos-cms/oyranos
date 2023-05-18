/** @file About.qml
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2023 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  About info page
 */

import QtQuick 2.7
import QtQuick.Controls 2.4

Rectangle {
    id: aboutRectangle
    width: parent.width
    height: parent.height
    anchors.centerIn: parent

    color: "lightgray"
    implicitHeight: parent.height
    implicitWidth: parent.width

    property alias image: logoImage.source

    Flickable {
        width: parent.width
        height: aboutRectangle.height - textArea.font.pixelSize * 3 // keep some space for the button
        contentWidth: parent.width; contentHeight: logoImage.height + parent.height

        Image {
            id: logoImage
            width: parent.width
            y: 10
            horizontalAlignment: Image.AlignHCenter
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/logo-sw.svg"
            sourceSize.width: 350
            sourceSize.height: 350
            height: 175
        }

        TextArea { // our content
            id: textArea
            y: logoImage.height + logoImage.y + 10
            width: parent.width
            height: parent.height - logoImage.height - font.pixelSize * 3 // keep some space for the button

            Accessible.name: "about text"
            //backgroundVisible: false // keep the area visually simple
            //frameVisible: false      // keep the area visually simple
            color: fg
            background: Rectangle { color: bg }

            textFormat: Qt.RichText // Html
            textMargin: font.pixelSize
            readOnly: true // obviously no edits
            text: "<html><head></head><body> <p align=\"center\">" +
                          "Version " + ApplicationVersion + "<br \>" +
                          qsTr("Oyjl Options Renderer") +
                          "<br \>Copyright © 2018-2023 Kai-Uwe Behrmann<br \>" +
                          qsTr("All Rights reserved.") +
                          "<br \><a href=\"http://www.oyranos.org\">www.oyranos.org</a></p>" +
                          "<hr /><p align=\"center\">" +
                          "<table border=\"0\" style=\"border-spacing:10px\">" +
                          "<tr><td align=\"right\" style=\"padding-right:10;\">" + qsTr("Platform") + "</td>" +
                              "<td>" + Qt.platform.os + "</td></tr>" +
                          "<tr><td align=\"right\" style=\"padding-right:10;\">" + qsTr("System") + "</td>" +
                              "<td>" + SysProductInfo + "</td></tr>" +
                          "<tr><td align=\"right\" style=\"padding-right:10;\">" + qsTr("Qt Version") + "</td>" +
                              "<td>" + QtRuntimeVersion + " (" + QtCompileVersion + ")</td></tr>" +
                          "<tr><td align=\"right\" style=\"padding-right:10;\">" + appData.getLibDescription(1) + "</td>" +
                              "<td>" + appData.getLibDescription(0) + "</td></tr>" +
                          "<tr><td align=\"right\" style=\"padding-right:10;\">" + appData.getLibDescription(1) + "</td>" +
                              "<td>" + appData.getLibDescription(3) + "</td></tr>" +
                          "<tr><td align=\"right\" style=\"padding-right:10;\">" + appData.getLibDescription(2) + " git</td>" +
                              "<td>" + appData.getLibDescription(4) + "</td></tr>" +
                          "</table></p>" +
                          "</body></html>"
            onLinkActivated: {
                setBusyTimer.start()
                if(Qt.openUrlExternally(link))
                    statusText = qsTr("Launched app for ") + link
                else
                    statusText = "Launching external app failed"
                unsetBusyTimer.start()
            }
            onLinkHovered: (Qt.platform.os === "android") ? Qt.openUrlExternally(link) : statusText = link
        }
        ScrollBar.vertical: ScrollBar { }
    }

    Button { // finish button
        text: qsTr("OK")
        width: parent.width - textArea.font.pixelSize * 2 // make this button big
        x: aboutRectangle.width/2 - width/2 // place in the middle
        y: aboutRectangle.height - textArea.font.pixelSize * 3 // place below aboutTextArea
        onClicked:  {
            pages.currentIndex = 3
            setPage(-1)
        }
    }
}

