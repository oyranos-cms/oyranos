/** @file main.qml
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  QML ApplicationWindow mainWindow
 */

// developed with Qt 5.7-5.10

import QtQuick 2.7
//import QtQml.Models 2.2
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4

import AppData 1.0
import Process 1.0
import "qrc:/qml"
import "process.js" as P
import "linkify.js" as Link

AppWindow {
    id: mainWindow
    objectName: "mainWindow"

    width: if( !fullscreen )
               100*dens

    signal fileChanged(var url) // Input
    onFileChanged: { setDataText( appData.getJSON( url ) ); statusText = qsTr("Loaded") + " " + url
        if(processSetCommand.length)
            processSet.start( processSetCommand, processSetArgs );
    }

    signal outputChanged( var url ) // Output
    property string outputJSON: ""
    onOutputChanged: { outputJSON = url; statusText = "output = " + outputJSON }

    signal commandsChanged( var url ) // Commands
    property string commandsJSON: ""
    onCommandsChanged: {
        if(url === "+")
          commandsJSON = appDataJsonString
        else
          commandsJSON = appData.readFile( url );
        textArea2.text = commandsJSON
        if( commandsJSON.length )
        {
            var c = JSON.parse(commandsJSON)

            if( typeof c.command_get !== "undefined" )
                processGetCommand = c.command_get
            else
                processGetCommand = ""
            if( typeof c.command_get_args !== "undefined" )
                processGetArgs = c.command_get_args
            else
                processGetArgs = []
            if( typeof c.command_set !== "undefined" )
                processSetCommand = c.command_set
            else
                processSetCommand = ""
            if( typeof c.command_set_args !== "undefined" )
                processSetArgs = c.command_set_args
            else
                processSetArgs = []
            if( typeof c.command_set_delimiter !== "undefined" )
                command_set_delimiter = c.command_set_delimiter
            else
                command_set_delimiter = "="
            if( typeof c.command_set_option !== "undefined" )
                command_set_option = c.command_set_option
            else
                command_set_option = ""
        }

        statusText = qsTr("commands enabled") + " = " + url
    }

    Process { id: processGet; onReadChannelFinished: { textArea2.text = readAll(); setDataText(textArea2.text);} }
    property string processGetCommand: ""
    property var processGetArgs: [ ]

    Process { id: processSet; onReadChannelFinished: helpText = readAll(); }
    property string processSetCommand: ""
    property var processSetArgs: [ ]
    property string command_set_delimiter: "="
    property string command_set_option: ""


    function interactiveCallback( key, value, setOnly )
    {
        if(processSetCommand.length && setOnly >= 0)
        {
            var arg = key
            if(command_set_option.length === 0)
            {
                if(key.length > 1)
                    arg = "--" + key
                else
                    arg = "-" + key
            }
            var v = JSON.stringify(value);
            if(v.length)
                if(typeof command_set_delimiter !== "undefined")
                {
                    if(typeof value === "string")
                    {
                        if(value.length)
                            arg += command_set_delimiter + value
                    }
                    else
                        arg += command_set_delimiter + v
                }
            var args = []
            args = processSetArgs.slice()
            var count = args.length
            if(command_set_option.length === 0)
                args[args.length] = arg
            else
            {
                args[args.length] = command_set_option
                args[args.length] = arg
            }
            if(app_debug)
                statusText = "command_set: " + processSetCommand + " " + args
            processSet.start( processSetCommand, args )
            processSet.waitForFinished()
        }
        if(processGetCommand.length && setOnly <= 0)
        {
            if(app_debug)
                statusText = "command_get: " + processGetCommand + " " + processGetArgs
            processGet.start( processGetCommand, processGetArgs )
        }
    }

    Component.onCompleted: {
        icon = ":/images/logo-sw.svg"
        logo = "qrc:/images/logo-sw.svg"
    }


    property string appDataJsonString: "someJSON"
    property bool showJson: false
    property string introText: "Intro"
    property string cmmText: "Description"
    property string cmmHelp: ""
    property string helpText: ""
    property bool helpTextChanging: false
    onHelpTextChanged: {
        if(helpTextChanging)
            return
        helpTextChanging = true
        var text = helpText.replace(/\n/g,"<br />")
        helpText = text
        helpTextChanging = false
    }

    property var groupDescriptions: []
    property int groupCount: 0
    property url uiLogo: ""

    ListModel {  id: optionsModel; objectName: "optionsModel" }

    property real button_height: (pages.height - 2 * font.pixelSize * 3 - 2*dens)
    VisualItemModel {
        id: pagesModel

        HalfPage {
            id: firstPage

            Column {
                height: firstPage.height - 2*h
                padding: dens

                TextArea {
                    id: introTextField
                    objectName: "introTextField"
                    width: firstPage.width

                    textFormat: Qt.RichText // Html
                    textMargin: font.pixelSize
                    readOnly: true // obviously no edits
                    wrapMode: TextEdit.Wrap
                    text: "<html><head></head><body> <h3><p align=\"center\">" + introText + "</p></h3></body></html>"

                    color: fg
                    background: Rectangle { color: bg }
                }
                OptionsList {
                    id: optList
                    objectName: "optList"
                    anchors.bottomMargin: 0
                    width: firstPage.width - 2*dens
                    height: Math.min( firstPage.height - introTextField.height - dens - firstPage.height/4,
                                      optionsModel.count * h + groupCount * h )
                    model: optionsModel
                    callback: interactiveCallback
                    color: bg
                }
                Rectangle {
                    color: fg
                    width: firstPage.width - 2*dens
                    height: 1
                }
                Flickable {
                    id: helpFlickable
                    width: firstPage.width - dens
                    height: firstPage.height - introTextField.height - dens - optList.height - dens

                    flickableDirection: Flickable.VerticalFlick

                TextArea.flickable: TextArea {
                    id: helpTextArea
                    objectName: "helpTextArea"
                    width: helpFlickable.width
                    height: helpFlickable.height

                    Accessible.name: "Text Area Help"

                    textFormat: Qt.RichText // Html
                    wrapMode: TextEdit.Wrap
                    readOnly: true
                    text: helpText

                    color: fg
                    background: Rectangle { color: bg }
                }
                }
            }
        }
        HalfPage {
            id: twoPage

            Image {
                id: logoImage
                width: parent.width
                y: 10
                horizontalAlignment: Image.AlignHCenter
                fillMode: Image.PreserveAspectFit
                source: uiLogo
            }
            Flickable {
                y: logoImage.height
                width: twoPage.width
                height: twoPage.height - logoImage.height
                flickableDirection: Flickable.VerticalFlick
                clip: true
            TextArea.flickable: TextArea { // our content
                id: textArea

                Accessible.name: "about text"
                //backgroundVisible: false // keep the area visually simple
                //frameVisible: false      // keep the area visually simple

                textFormat: Qt.RichText // Html
                textMargin: font.pixelSize
                readOnly: true // obviously no edits
                wrapMode: TextEdit.Wrap
                text: cmmText
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
            }
        }
        Rectangle {
            id: threePage
            width: pages.width
            height: pages.height
            color: "transparent"

            Flickable {
                width: threePage.width
                height: threePage.height
                flickableDirection: Flickable.VerticalFlick
            TextArea.flickable: TextArea {
                id: textArea2

                Accessible.name: "Text Area 2"
                anchors.fill: parent
                textFormat: text[0] !== '<' ? Qt.PlainText : Qt.RichText
                wrapMode: TextEdit.Wrap
                readOnly: true
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
            }
        }
        Rectangle {
            width: pages.width
            height: pages.height
            id: aboutPage
            About {
                objectName: "About";
                image: logo
            }
        }
    }

    property string dataText;
    property var appJsonObject;
    property real deviceLabelWidth: 20;
    property string loc: "";

    function setOptions( options, groupName, groupDescription )
    {
        if(typeof groupDescriptions[groupName] === "undefined")
            groupDescriptions[groupName] = groupDescription

        for( var index in options )
        {
            var opt = options[index];
            var def = opt.default;
            var current = -1;
            var suggest;
            var choices = opt.choices
            var type = opt.type
            var dbl = {"start":0,"end":1}
            if( type === "double" )
                // try slider
            {
                var start = opt.start
                var end = opt.end
                var tick = opt.tick
                current = opt.default
                if(typeof tick !== "undefined")
                {
                    dbl["start"] = start
                    dbl["end"] = end
                    dbl["tick"] = tick
                }
            }
            if( type === "bool")
            {
                current = opt.default;
            }
            if( type === "string")
            {
                var opt_ = opt
                if(typeof opt_.suggest !== "undefined")
                    suggest = opt_.suggest;
            }

            var name
            if(type === "choice")
            for( var i in choices )
            {
                if( typeof choices[i].name != "string" )
                    continue;
                var item = choices[i];
                var nick = item.nick;
                if(nick === def)
                    current = name;
            }

            name = P.getTranslatedItem( opt, "name", loc );
            var desc = P.getTranslatedItem( opt, "description", loc );
            var help = P.getTranslatedItem( opt, "help", loc );
            //if(typeof help === "undefined")
            //    help = ""
            var o = {
                key: opt.key,
                name: name,
                type: type,
                choices: opt.choices,
                dbl: dbl,
                current: current,
                suggest: suggest,
                nick: opt.nick,
                loc: loc,
                groupName: groupName,
                description: desc,
                help: help,
                default: opt.default
            }
            var text = JSON.stringify(o);
            o.text = text;
            optionsModel.append(o)
            var count = optionsModel.count
            var c = count
        }
    }

    Process { id: logoProcess; onReadyRead: {uiLogo = readAll();
            var name = uiLogo
            statusText = "found: " + name;
            icon = name } }
    // get the whole bunch of CMM keys and filter the groups and other
    // major stuff
    function setDataText( t )
    {
        optionsModel.clear()
        dataText = t;
        appDataJsonString = t;
        appJsonObject = JSON.parse(t);
        if( appJsonObject === "undefined" )
            logText = "Parsing failed: " + t;
        var j = appJsonObject;
        if(typeof j.LOCALE_info !== "undefined")
            loc = j.LOCALE_info;
        if(typeof j.org === "undefined" ||
           typeof j.org.freedesktop === "undefined" ||
           typeof j.org.freedesktop.oyjl === "undefined" ||
           typeof j.org.freedesktop.oyjl.modules === "undefined")
            introText = qsTr("Load failed") + "<br />" + t
        var cmm = j.org.freedesktop.oyjl.modules[0];

        uiLogo = appData.findLogo( cmm.logo )
        var name = uiLogo
        icon = name
        // CMM head line - long
        introText = P.getTranslatedItem( cmm, "name", loc );
        appName = introText
        // extract CMM infos
        var html
        cmmHelp = ""
        html = "<html><body><p align=\"center\"><table border=\"0\" style=\"border-spacing:10px\">" +
                "<tr><td align=\"right\" style=\"padding-right:10;\">" + P.getTranslatedItem( cmm, "label", loc ) + ":</td><td style=\"font-weight:bold;\">" + P.getTranslatedItem( cmm, "description", loc ) + "</td></tr>"
        html += "<tr><td align=\"right\" style=\"padding-right:10;\">ID:</td><td style=\"font-weight:bold;\">" + cmm.nick + "</td>"
        for( var index in cmm.information )
        {
            var item = cmm.information[index]
            if(item.type === "date")
                continue
            var label = P.getTranslatedItem( item, "label", loc );
            name = P.getTranslatedItem( item, "name", loc );
            var desc = P.getTranslatedItem( item, "description", loc );
            html += "<tr><td align=\"right\" style=\"padding-right:10;word-wrap:break-word;\">" + label + ":</td><td style=\"font-weight:bold;\">" + name
            if( typeof desc !== "undefined" && item.type !== "documentation" )
                html += "<br /> " + desc
            if( typeof desc !== "undefined" && item.type === "documentation" )
                cmmHelp = desc
            html += "</td></tr>"
        }
        html += "</table></p></body></html>";

        if(cmmHelp.length > 0)
            html += "<br /><p>" + cmmHelp + "</p>"

        cmmText = Link.linkify( html );

        var groups = cmm.groups;
        // test for various groups
        for( var g1 in groups )
        {
            var group = groups[g1];
            var options= group.options;
            var groupName = P.getTranslatedItem( group, "name", loc );
            var help = P.getTranslatedItem( group, "help", loc );
            if( typeof options === "undefined" )
                for( var g2 in group.groups )
                {
                    var g = group.groups[g2]
                    options = g.options
                    var groupName2 = groupName + " : " + P.getTranslatedItem( g, "name", loc );
                    desc = P.getTranslatedItem( g, "description", loc )
                    if(typeof desc !== "undefined")
                        groupName2 += " " + desc
                    var help2 = help
                    if(typeof groupName2 !== "undefined")
                        help2 += " <i>" + P.getTranslatedItem( g, "help", loc ) + "</i>";
                    setOptions( options, groupName2, help2 )
                }
            else
            {
                desc = P.getTranslatedItem( group, "description", loc )
                if(typeof desc !== "undefined")
                    groupName = desc
                setOptions( options, groupName, help )
            }
        }
    }

    closeFunction: function() { appData.writeJSON( outputJSON ) }
}
