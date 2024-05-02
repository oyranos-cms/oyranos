/** @file Combo.qml
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  Combo Box with label and translated items
 */

import QtQuick 2.7
import QtQuick.Controls 2.4

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
    property var labelFont: label_.font
    property ListModel model: items

    property alias combo: combo

    width: parent.width
    height: textMetrics.font.pixelSize * 3
    color: "transparent"

    ListModel { id: items; objectName: "items" }

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
            y: (combo.height - height) / 2
            width: row.labelWidth_
            height: label_.height
            color: "transparent"
            Text {
                id: label_
                objectName: "label_"
                color: fg
                text: label
            }
            MouseArea {
                width: parent.width
                height: parent.height
                onClicked: combo.focus = false
            }
        }
        Rectangle {
            width : all.width - row.labelWidth_ - space
            height: combo.height
            ComboBox {
                id: combo
                objectName: "combo"
                width: parent.width
                height: all.height
                textRole: "key"
                model: items
                property real s: space
                onActiveFocusChanged: {
                    // make the combo box nearly as wide as the all item
                    // it works not relyable, as it is based on focus handling
                    if(all.getLabelWidth() === all.getLabelWidthMin())
                         return;
                    if(activeFocus)
                    {
                        row.labelWidth_ = Qt.binding( function() { return all.getLabelWidthMin() } )
                        combo.width = Qt.binding( function() { return all.width - row.labelWidth_ } )
                    } else
                    {
                        row.labelWidth_ = Qt.binding( function() { return all.getLabelWidth() } )
                        combo.width = Qt.binding( function() { return all.width - row.labelWidth_ - space } )
                    }
                    //statusText = activeFocus + " " + row.labelWidth_ + "/" + labelWidth
                }
                //onCurrentIndexChanged: setResult(label, currentText)
            }
        }
    }
}
