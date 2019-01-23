/** @file OptionsList.qml
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  Headers and options.
 */

import QtQuick 2.7
import QtQuick.Controls 2.1
import "process.js" as P

Rectangle {
    id: top
    objectName: "top"
    color: bg

    property string highlightColor: "red"
    property alias currentIndex: list.currentIndex
    property alias count: list.count
    property alias model: list.model
    property alias l: list
    property var callback: function(key, value, setOnly) { } // do nothing

    Component {
        id: sectionHeading
        Rectangle {
            objectName: "sectionHeadingRect"
            width: list.width
            height: 2*h/3
            color: bright ? Qt.lighter("steelblue") : "steelblue"
            Text {
                //anchors.verticalCenter: parent.verticalCenter
                anchors.centerIn: parent
                x: h/4
                text: "<b>" + section + "</b>"
                color: fg
            }
            MouseArea {
                width: parent.width
                height: parent.height
                onClicked: {
                    var text = groupDescriptions[section]
                    if( typeof text !== "undefined" )
                        helpText = text
                    else
                        helpText = ""
                    focus = true
                }
            }
            Component.onCompleted: groupCount += 1
        }
    }

    ListView {
        id: list
        objectName: "list"
        anchors.fill: parent
        spacing: dens
        highlightMoveDuration: 250
        clip: true
        keyNavigationWraps: true
        focus: true

        section.property: "groupName"
        section.criteria: ViewSection.FullString
        section.delegate: sectionHeading

        delegate: Rectangle {
            id: itemRect
            objectName: "itemRect"
            x: dens
            width:  parent.width - 2*dens
            height: h
            color: "transparent"

            Combo {
                id: comboBox
                objectName: "comboBox"
                label: name
                width:  parent.width - dens
                getLabelWidth: function() { return comboBox.width / 2 };
                getLabelWidthMin: function() { return 0 }
                color: bg
                Component.onCompleted: {
                    if(type === "choice")
                    {
                        setDataText2( this, text )
                        visible = true
                    } else
                        visible = false
                }
                combo.onCurrentIndexChanged: {
                    var role = combo.textRole
                    var i = combo.find(combo.textAt(combo.currentIndex))
                    var item = combo.model.get(i)
                    var nick = item.nick
                    if(nick === currentValue)
                        return;
                    statusText = key + ":" + nick + " " + combo.textAt(combo.currentIndex) + " " + qsTr("selected") + "  " + qsTr("new/old") + ": " + nick + "/" + currentValue
                    currentValue = nick;
                    appData.setOption(key, item.nick)
                    var k = key
                    callback( k, nick, 0 )
                }
            }
            LSlider {
                id: lslider
                objectName: "lslider"
                label: name
                width:  parent.width - dens
                getLabelWidth: function() { return comboBox.width / 2 };
                getLabelWidthMin: function() { return 0 }
                //color: bg
                property bool init: true
                Component.onCompleted: {
                    if(type === "double")
                    {
                        var j = JSON.parse(text)
                        key = j.key
                        var v = parseFloat(j.current)
                        slider.from = parseFloat(j.dbl.start)
                        slider.to = parseFloat(j.dbl.end)
                        slider.stepSize = parseFloat(j.dbl.tick)
                        currentValue = v
                        slider.value = v
                        if(app_debug)
                            statusText = key + " double " + slider.from + " - " + slider.to + " , " + slider.stepSize
                        visible = true
                        init = false
                    } else
                        visible = false
                }
                slider.onValueChanged: {
                    var cV = currentValue
                    var sv = slider.value
                    if(slider.value === currentValue || init)
                        return;
                    statusText = key + ":" + currentValue + " " + slider.value + " " + qsTr("selected") + "  " + qsTr("new/old") + ": " + slider.value + "/" + currentValue
                    currentValue = slider.value;
                    appData.setOption(key, slider.value)
                    var k = key
                    callback( k, slider.value, 1 )
                }
            }
            LSwitch {
                id: lswitch
                objectName: "lswitch"
                label: name
                width:  parent.width - dens
                getLabelWidth: function() { return comboBox.width / 2 };
                getLabelWidthMin: function() { return 0 }
                //color: bg
                property bool init: true
                Component.onCompleted: {
                    if(type === "bool")
                    {
                        var j = JSON.parse(text)
                        key = j.key
                        var v = parseFloat(j.current)
                        defaultValue = v
                        //switcher.position = v
                        if(app_debug)
                            statusText = key + " bool"
                        visible = true
                        init = false
                    } else
                        visible = false
                }
                switcher.onCheckedChanged: {
                    var cV = currentValue
                    var sv = switcher.checked
                    if(sv === currentValue || init)
                        return;
                    statusText = key + ":" + currentValue + " " + sv + " " + qsTr("selected") + "  " + qsTr("new/old") + ": " + sv + "/" + currentValue
                    currentValue = sv;
                    appData.setOption(key, sv)
                    var k = key
                    callback( k, "", 1 )
                }
            }
            LInput {
                id: linput
                objectName: "linput"
                label: name
                width:  parent.width - dens
                getLabelWidth: function() { return linput.width / 2 };
                getLabelWidthMin: function() { return 0 }
                color: bg
                property bool init: true
                Component.onCompleted: {
                    if(type === "string")
                    {
                        var j = JSON.parse(text)
                        key = j.key
                        if(typeof j.suggest !== "undefined")
                            defaultValue = j.suggest
                        if(app_debug)
                            statusText = key + " string"
                        visible = true
                        init = false
                    } else
                        visible = false
                }
                input.onCurrentTextChanged: {
                    var cV = currentValue
                    var sv = value
                    if(value === currentValue || init)
                        return;
                    statusText = key + ":" + currentValue + " " + value + " " + qsTr("selected") + "  " + qsTr("new/old") + ": " + value + "/" + currentValue
                    currentValue = value;
                    appData.setOption(key, value)
                    var k = key
                    callback( k, currentValue, 1 )
                }
                input.onPressedChanged: {
                    callback( k, value, 1 )
                }
            }
            MouseArea {
                width: comboBox.width - comboBox.combo.width
                height: itemRect.height
                onClicked: {
                    statusText = description
                    if( typeof help !== "undefined" )
                        helpText = help
                    else
                        helpText = ""
                    itemRect.focus = true
                }
            }
        }
    }

    function setDataText( t )
    {
        dataText = t;
        appDataJsonString = t;
        appJsonObject = JSON.parse(t);
        if( appJsonObject === "undefined" )
            logText = "Parsing failed: " + t;
        var j = appJsonObject;
        loc = j.LOCALE_info;
        var modules = j.org.freedesktop.oyjl.modules[0];
        var groups = modules.groups;
        var group = groups[0];
        var options= group.options;
        for( var index in options )
        {
            var opt = options[index];
            var def = opt.default;
            P.setComboItems( ri, opt.choices, opt.nick, def, loc );
        }
    }

    function setDataText2( combo, t )
    {
        var j = JSON.parse(t);
        var loc = j.loc;
        var def = j.default;
        var name = j.key;
        P.setComboItems( combo, j.choices, name, def, loc );
    }
}
