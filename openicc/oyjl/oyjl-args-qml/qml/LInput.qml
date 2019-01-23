/** @file LInput.qml
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/11/05
 *
 *  Text input with label
 */

import QtQuick 2.7
import QtQuick.Controls 2.1

Rectangle {
    id: all
    objectName: "all"
    property string label
    property string key
    property string defaultValue
    property string currentValue
    property real space: textMetrics.font.pixelSize
    property real labelWidth: 20
    property var getLabelWidthMin: function() { return all.labelWidth }
    property var getLabelWidth: function() { return all.labelWidth }

    property alias input: input
    property alias value: input.currentText

    width: parent.width
    height: textMetrics.font.pixelSize * 3
    color: bg

    TextMetrics {
        id: textMetrics
        objectName: "textMetrics"
        text: label
    }

    Row {
        id: row
        objectName: "row"
        spacing: space
        leftPadding: space / 2
        property real labelWidth_: all.getLabelWidth()

        Rectangle
        {
            y: (all.height - height) / 2
            width: row.labelWidth_
            height: label_.height
            Text {
                id: label_
                objectName: "label_"
                color: fg
                text: label
            }
            MouseArea {
                width: parent.width
                height: parent.height
            }
        }
        Rectangle {
            width : all.width - row.labelWidth_ - space
            height: input.height
            y: (all.height - height) / 2
            ComboBox {
                id: input
                objectName: "input"
                width: parent.width
                editable: true;
                textRole: "text"
                ListModel { id: list }
                Component.onCompleted: {
                    if( typeof currentValue !== "undefined" )
                        list.append( { key: defaultValue, text: defaultValue } )
                }
                model: list
            }
        }
    }
}
