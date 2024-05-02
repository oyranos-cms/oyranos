/** @file OptionsList.qml
 *
 *  @par Copyright:
 *            2018-2023 (C) Kai-Uwe Behrmann
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
                    setHelpText( "", false )
                    var text = groupDescriptions[section]
                    if( typeof text !== "undefined" )
                        setHelpText( "oyjl-html-format-marker" + text, false )
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
            width:  parent !== null ? parent.width - 2*dens : width
            height: h
            color: run ? (bright ? Qt.lighter("steelblue") : "steelblue") : immediate ? (bright ? Qt.lighter("grey") : "grey") : "transparent"

            function setModified()
            {
                var v = appData.getOption(key);
                var set;
                set = (v === "true");
                if(!set && v === "1")
                    set = true;
                if(!set && v === 1)
                    set = true;
                if(!set && v === true)
                    set = true;
                lswitch.labelFont.bold = set;
                lslider.labelFont.bold = set;
                comboBox.labelFont.bold = set;
                linput.labelFont.bold = set;
                if(app_debug)
                    statusText = "QML::OptionsList::setModified() set bold:" + set
            }

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
                    labelFont.bold = appData.getOption(key).length && value !== "false" ? true : false
                    if(app_debug)
                    {
                        var j = JSON.parse(appData.plainJSON(text))
                        statusText = "QML::OptionsList::Combo::Component.onCompleted() " + j.key + " value:" + value + " \"" + combo.textAt(combo.currentIndex) + "\" currentValue:" + currentValue + " bold:" + labelFont.bold
                    }
                }
                combo.onCurrentIndexChanged: {
                    var role = combo.textRole
                    var old_text = combo.currentText
                    var t = combo.textAt(combo.currentIndex)
                    var i = combo.find(t)
                    var item = combo.model.get(i)
                    if(app_debug)
                        statusText = "QML::OptionsList::combo.onCurrentIndexChanged() key:\"" + key + "\" item:" + JSON.stringify(item) + " currentValue: " + currentValue + " appData.getOption(key):" + appData.getOption(key)
                    var nick = item !== undefined ? item.nick : false
                    labelFont.bold = appData.getOption(key).length && appData.getOption(key) !== "false" && appData.getOption(key) !== false ? true : false
                    if(nick === currentValue || init)
                        return;
                    if(app_debug)
                        statusText = "QML::OptionsList::combo.onCurrentIndexChanged() " + key + ":" + nick + " " + combo.textAt(combo.currentIndex) + " " + qsTr("selected") + "  " + qsTr("new/old") + ": " + nick + "/" + currentValue + " bold:" + labelFont.bold
                    currentValue = nick;
                    appData.setOption(key, nick, group.id)
                    var k = key
                    value = nick
                    changed = value
                    if(app_debug)
                        statusText = "QML::OptionsList::combo.onCurrentIndexChanged() " + key + " item:" + JSON.stringify(item) + " currentValue: " + currentValue + " appData.getOption(key):" + appData.getOption(key)
                    callback( key, value, type, group, 0 )
                }
                onLabelFontChanged: {
                    if(app_debug)
                        statusText = "QML::OptionsList:comboBox:onLabelFontChanged() " + key + ":" + currentValue + " font:" + labelFont.bold?"bold":"no mark"
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
                        var j = JSON.parse(appData.plainJSON(text))
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
                            statusText = "QML::OptionsList::LSlider::Component.onCompleted() " + key + " double " + slider.from + " - " + slider.to + " , " + slider.stepSize
                        labelFont.bold = appData.getOption(key).length ? true : false
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
                    if(app_debug)
                        statusText = "QML::OptionsList::slider.onValueChanged() " + key + ":" + currentValue + " " + slider.value + " " + qsTr("selected") + "  " + qsTr("new/old") + ": " + slider.value + "/" + currentValue
                    currentValue = slider.value;
                    value = currentValue
                    appData.setOption(key, value, group.id)
                    var k = key
                    value = JSON.stringify(slider.value)
                    changed = value
                    callback( key, value, type, group, 1 )
                    labelFont.bold = appData.getOption(k).length ? true : false
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
                        var j = JSON.parse(appData.plainJSON(text))
                        key = j.key
                        var v = parseFloat(j.current)
                        defaultValue = v
                        switcher.checked = (value === "true")
                        if(app_debug)
                            statusText = "QML::OptionsList::LSwitch::Component.onCompleted() " + key + " bool"
                        button = run
                        visible = true
                        init = false
                        changedValue = changed
                    } else
                        visible = false
                }
                switcher.onCheckedChanged: {
                    var cV = currentValue
                    var sv = switcher.checked
                    if(sv === currentValue || init)
                        return;
                    if(app_debug)
                        statusText = "QML::OptionsList::switcher.onCheckedChanged() " + key + ":" + currentValue + " " + sv + " " + qsTr("selected") + "  " + qsTr("new/old") + ": " + currentValue + "/" + sv
                    currentValue = sv;
                    appData.setOption(key, sv, group.id)
                    var k = key
                    value = JSON.stringify(sv)
                    changed = value
                    var v = value
                    var ci = list.currentItem
                    var i = currentIndex
                    var d = model
                    if(sv)
                    {
                        callback( key, value, type, group, 1 )
                        changed = "true"
                    }
                    else
                        changed = ""
                    changedValue = changed
                }
                butt.onPressed: {
                    if(init) return;
                    if(app_debug)
                        statusText = "QML::OptionsList::butt.onPressed() " + key + ":" + qsTr("selected")
                    appData.setOption(key, true, group.id)
                    callback( key, JSON.stringify(true), type, group, 1 )
                    setModified()
                }
                onChangedValueChanged: // just for fun as a Changed event; could alternatively be handled in onCheckedChanged as well
                {
                    var v = appData.getOption(key);
                    var set;
                    set = (v === "true");
                    if(!set && v === "1")
                        set = true;
                    if(!set && v === 1)
                        set = true;
                    if(!set && v === true)
                        set = true;
                    if(app_debug)
                        statusText = "QML::OptionsList::onChangedValueChanged() " + key + " set: " + set
                    labelFont.bold = set;
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
                property bool repetition: false
                Component.onCompleted: {
                    if(type === "string")
                    {
                        init = true
                        var t = text
                        var j = JSON.parse(appData.plainJSON(t))
                        key = j.key
                        if(typeof j.suggest !== "undefined")
                            defaultValue = j.suggest
                        if(app_debug)
                            statusText = "QML::OptionsList::LInput::Component.onCompleted() " + key + " string value:" + value
                        setDataText2( this, text, value )
                        repetition = j.repetition
                        visible = true
                        init = false
                    } else
                        visible = false
                    if(app_debug)
                        statusText = "QML::OptionsList::MouseArea::Component.onCompleted() appData.getOption(key).length " + appData.getOption(key).length
                    labelFont.bold = appData.getOption(key).length ? true : false
                }
                combo.onCurrentTextChanged: { value = combo.currentText; changed = value; }
                combo.onEditTextChanged: { value = combo.editText; changed = value; }
                combo.onDisplayTextChanged: { value = combo.displayText; changed = value; }
                combo.onAccepted: {
                    var i = combo
                    var t = value
                    var c = t[t.length-1]
                    if(repetition && c === ";")
                    {
                        value_old = t
                        combo.displayText = ""
                        combo.editText = ""
                        combo.currentText = ""
                        return
                    }
                    var old = value_old
                    var k = key;
                    var model = combo.model
                    if(old.length > 0)
                        c = old[old.length-1]
                    if(repetition && c === ";")
                    {
                        t = value_old + value
                        i.displayText = t
                        if( typeof model !== "undefined")
                            model.append({key: t})
                    }
                    var ind  = combo.find(t)
                    if (ind === -1)
                    {
                        if( typeof model !== "undefined")
                            model.append({key: t})
                    }
                    if(app_debug)
                        statusText = "QML::OptionsList::MouseArea::combo.onAccepted() " + k + ":" + t
                    //value = t
                    i.displayText = t;
                    i.editText = t;
                    //i.currentText = t;
                    value_old = value
                    appData.setOption(key, value, group.id)
                    callback( key, value, type, group, 0 )
                    labelFont.bold = appData.getOption(k).length ? true : false
                }
                onLabelFontChanged: {
                    if(app_debug)
                        statusText = "QML::OptionsList::linput::onLabelFontChanged() " + key + ":" + currentValue + " font:" + labelFont.bold?"bold":"no mark"
                }
            }
            MouseArea {
                width: comboBox.width - comboBox.combo.width
                height: itemRect.height
                onClicked: {
                    setHelpText( "", false )
                    setHelpText( "", false )
                    var named_option = 0
                    if(typeof key !== "undefined" && key !== "@" && key !== "#")
                        named_option = 1
                    if( named_option )
                    {
                        if(key.length === 1)
                            helpText = "[-" + key
                        else
                            helpText = "[--" + key
                    }
                    else if(typeof key !== "undefined")
                        helpText = "[" + key
                    if( value_name.length !== 0 )
                        helpText += "=" + value_name
                    helpText += "] "
                    var found = 0
                    if( typeof name !== "undefined" )
                    {
                        if(found === 1) helpText += " "; found = 0
                        var n = name
                        if( n.length !== 0 )
                        {
                            helpText += n
                            found = 1
                        }
                    }
                    if( typeof description !== "undefined" )
                    {
                        if(found === 1) helpText += "\n"; found = 0
                        var d = description
                        if( d.length !== 0 )
                        {
                            helpText += d
                            found = 1
                        }
                    }
                    if( typeof help !== "undefined" )
                    {
                        if(found === 1) helpText += "\n"; found = 0
                        var h = help
                        if( h.length !== 0 )
                        {
                            helpText += h
                            found = 1;
                        }
                    }
                    if( type === "double")
                    {
                        if(found === 1) helpText += "\n"; found = 0
                        if(typeof dbl.tick !== "undefined")
                        {
                            helpText += "(" + value_name + ":" + current + " [≥" + dbl.start + " ≤" + dbl.end + " Δ" + dbl.tick + "])"
                            found = 1;
                        }
                    }

                    if( type === "string" && repetition)
                    {
                        helpText += "\n\n";
                        helpText += qsTr("Multiple Options Hint");
                        helpText += "\n";
                        helpText += qsTr("Enter first value, append ';' and confirm. The final ';' is not executed. Then select next value. Repeat as needed. A final confirm without ending ';' can execute.");
                        found = 1;
                    }

                    itemRect.focus = true
                }
                onDoubleClicked:
                {
                    if(app_debug)
                        statusText = "QML::OptionsList::MouseArea::onDoubleClicked: " + key + ":" + value + " " + type
                    appData.setOption(key, false, group.id)
                    current = ""
                    value = ""
                    changed = ""
                    if( type === "bool" && lswitch.switcher.checked)
                        lswitch.switcher.checked = false;
                    if( type === "double" )
                        lslider.labelFont.bold = appData.getOption(key).length ? true : false;
                    if( type === "choice" && comboBox.combo.currentIndex >= 0 )
                        comboBox.combo.currentIndex = -1;
                    if( type === "string" && linput.combo.currentIndex >= 0 )
                        linput.combo.currentIndex = -1;
                    setModified()
                }
            }
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
        var j = JSON.parse(appData.plainJSON(t));
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
