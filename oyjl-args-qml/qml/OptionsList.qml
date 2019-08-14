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
import QtQuick.Controls 2.4
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
    property var callback: function(model, group, setOnly) { } // do nothing

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
            color: run ? (bright ? Qt.lighter("steelblue") : "steelblue") : "transparent"

            Combo {
                id: comboBox
                objectName: "comboBox"
                label: name
                width:  parent.width - dens
                getLabelWidth: function() { return comboBox.width / 2 };
                getLabelWidthMin: function() { return 0 }
                color: "transparent"
                property bool init: true
                Component.onCompleted: {
                    if(type === "choice")
                    {
                        init = true
                        setDataText2( this, text, value )
                        visible = true
                        init = false
                    } else
                        visible = false
                }
                combo.onCurrentIndexChanged: {
                    var role = combo.textRole
                    var t = combo.textAt(combo.currentIndex)
                    var i = combo.find(t)
                    var item = combo.model.get(i)
                    var nick = item.nick
                    if(nick === currentValue || init)
                        return;
                    statusText = key + ":" + nick + " " + combo.textAt(combo.currentIndex) + " " + qsTr("selected") + "  " + qsTr("new/old") + ": " + nick + "/" + currentValue
                    currentValue = nick;
                    appData.setOption(key, item.nick)
                    var k = key
                    value = nick
                    callback( key, value, type, group, 0 )
                }
            }
            LSlider {
                id: lslider
                objectName: "lslider"
                label: name
                width:  parent.width - dens
                getLabelWidth: function() { return comboBox.width / 2 };
                getLabelWidthMin: function() { return 0 }
                color: "transparent"
                property bool init: true
                Component.onCompleted: {
                    if(type === "double")
                    {
                        init = true
                        var j = JSON.parse(text)
                        key = j.key
                        var v = parseFloat(j.current)
                        slider.from = parseFloat(j.dbl.start)
                        slider.to = parseFloat(j.dbl.end)
                        slider.stepSize = parseFloat(j.dbl.tick)
                        currentValue = v
                        if(value !== "")
                            slider.value = parseFloat(value)
                        else
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
                    value = JSON.stringify(slider.value)
                    callback( key, value, type, group, 1 )
                }
            }
            LSwitch {
                id: lswitch
                objectName: "lswitch"
                label: name
                width:  parent.width - dens
                getLabelWidth: function() { return comboBox.width / 2 }
                getLabelWidthMin: function() { return 0 }
                color: "transparent"
                property bool init: true
                Component.onCompleted: {
                    if(type === "bool")
                    {
                        init = true
                        var j = JSON.parse(text)
                        key = j.key
                        var v = parseFloat(j.current)
                        defaultValue = v
                        switcher.checked = (value === "true")
                        if(app_debug)
                            statusText = key + " bool"
                        button = run
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
                    value = JSON.stringify(sv)
                    var v = value
                    var ci = list.currentItem
                    var i = currentIndex
                    var d = model
                    if(sv)
                        callback( key, value, type, group, 1 )
                }
                butt.onPressed: {
                    if(init) return;
                    statusText = key + ":" + qsTr("selected")
                    appData.setOption(key, true)
                    callback( key, JSON.stringify(true), type, group, 1 )
                }
            }
            LInput {
                id: linput
                objectName: "linput"
                label: name
                width:  parent.width - dens
                getLabelWidth: function() { return linput.width / 2 };
                getLabelWidthMin: function() { return 0 }
                color: "transparent"
                property bool init: true
                Component.onCompleted: {
                    if(type === "string")
                    {
                        init = true
                        var t = text
                        var j = JSON.parse(t)
                        key = j.key
                        if(typeof j.suggest !== "undefined")
                            defaultValue = j.suggest
                        if(app_debug)
                            statusText = key + " string"
                        setDataText2( this, text, value )
                        visible = true
                        init = false
                    } else
                        visible = false
                }
                combo.onCurrentTextChanged: {
                    var cV = currentValue
                    var sv = combo.currentText
                    var k = key;
                    if(sv === cV || sv.length === 0 || init)
                        return;
                    statusText = k + ":" + cV + " " + sv + " " + qsTr("selected")
                    cV = sv;
                    appData.setOption(k, cV);
                    value = cV
                    callback( key, value, type, group, 1 )
                }
                combo.onAccepted: {
                    var i = combo
                    var t = i.editText
                    var k = key;
                    var ind  = combo.find(t)
                    var model = combo.model
                    if (ind === -1)
                    {
                        if( typeof model !== "undefined")
                            model.append({key: t})
                    }
                    statusText = k + ":" + t
                    value = t
                    callback( key, value, type, group, 1 )
                }
                //combo.onAcceptableInputChanged:  statusText = "AI: "+combo.currentText
                //combo.onEditTextChanged: statusText = "ET: "+combo.editText
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

    function findListModel( model, type, value )
    {
        var n = model.count
        var i
        for(i=0; i<n; ++i)
        {
            var obj = model.get(i)
            if(obj[type] !== "undefined")
            {
                var v = obj[type]
                if(v === value)
                    return i
            }
        }
        return -1
    }

    function setDataText2( combo, t, value )
    {
        var j = JSON.parse(t);
        var loc = j.loc;
        var def = j.default;
        var name = j.key;
        var choices = j.choices
        P.setComboItems( combo, choices, name, def, loc );
        if(value !== "")
        {
            var i = findListModel(combo.combo.model, "nick", value)
            if(i >= 0)
                combo.combo.currentIndex = i
        }
    }
}
