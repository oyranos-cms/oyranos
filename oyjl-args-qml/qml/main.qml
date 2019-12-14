/** @file main.qml
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2019 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  QML ApplicationWindow mainWindow
 */

// developed with Qt 5.7-5.12

import QtQuick 2.7
import QtQml.Models 2.4
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4

import AppData 1.0
import Process 1.0
import "qrc:/qml"
import "process.js" as P
import "linkify.js" as Link

AppWindow {
    id: mainWindow
    objectName: "mainWindow"

    width: if( fullscreen === false )
               100*dens

    signal fileChanged(var variable) // Input
    onFileChanged: { setDataText( appData.getJSON( variable ) ); var fn = "---"; if(variable[0] !== '{') fn = variable;
        statusText = qsTr("Loaded") + " " + fn
        if(processSetCommand.length)
            processSet.start( processSetCommand, processSetArgs );
    }

    signal outputChanged( var variable ) // Output
    property string outputJSON: ""
    onOutputChanged: { outputJSON = variable; statusText = "output = " + outputJSON }

    signal commandsChanged( var variable ) // Commands
    property string commandsJSON: ""
    onCommandsChanged: {
        if(variable === "+")
          commandsJSON = appDataJsonString
        else
        {
          if(variable[0] === '{')
            commandsJSON = variable;
          else
            commandsJSON = appData.readFile( variable );
        }
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

        statusText = qsTr("commands enabled") + " = " + variable
    }

    Process { id: processGet;
        onReadChannelFinished: {
            var text = readAll();
            textArea2.text = text;
            setDataText(textArea2.text);
        }
    }
    property string processGetCommand: ""
    property var processGetArgs: [ ]

    property string image_data: ""
    Process { id: processSet;
        onReadChannelFinished: {
            var data = readAll();
            image_data = data
            if(image_data.substr(0,22) === "data:image/png;base64," ||
               image_data.substr(0,19) === "data:image/svg+xml;")
            {
                helpTextArea.opacity = 0.01
                helpText = ""
                image.source = image_data
                image.opacity = 1.0
            }
            else
            {
                if(command_key === "h")
                    setHelpText( data, false )
                else
                    setHelpText( data, true )
            }
            unsetBusyTimer.start()
        }
    }
    property string processSetCommand: ""
    property var processSetArgs: [ ]
    property string command_set_delimiter: "="
    property string command_set_option: ""
    property string command_key: ""


    function interactiveCallback( key, value, type, group, setOnly )
    {
        // skip "detail" only groups
        if(group.mandatory.length === 0 && group.optional.length === 0)
            return

        // skip optional options from groups with mandatory requirement
        if(group.mandatory.length && !group.mandatory.match(key))
            return

        var sCb = processSetCommand
        command_key = key
        if(processSetCommand.length && setOnly >= 0)
        {
            var arg = key
            if(command_set_option.length === 0)
            {
                if(key === "#" || key === "@")
                    arg = null;
                else if(key.length > 1)
                    arg = "--" + key
                else if(key.length === 1)
                    arg = "-" + key
            }
            var v = JSON.stringify(value);
            if(v.length)
                if(typeof command_set_delimiter !== "undefined")
                {
                    if(typeof value === "string" && type !== "bool")
                    {
                        if(value.length !== 0)
                        {
                            if(key === "#" || key === "@")
                                arg = value
                            else if(arg.length > 0)
                                arg += command_set_delimiter + value
                            else
                                arg += value
                        }
                    }
                    else if(type === "bool" && value === "false")
                    {
                        if(key === "#" || key === "@")
                            arg = v
                        else if(arg.length > 0)
                            arg += command_set_delimiter + v
                        else
                            arg += v
                    }
                }
            var args = []
            args = processSetArgs.slice()
            var count = args.length
            if(command_set_option.length === 0)
            {
                if(arg !== null)
                    args[args.length] = arg
            }
            else
            {
                args[args.length] = command_set_option
                if(arg !== null)
                    args[args.length] = arg
            }
            if(app_debug)
                statusText = "command_set: " + processSetCommand + " " + args

            // create the args from group::optional options and add them to the mandatory arg from above
            // TODO: detect mandatory exclusion, e.g. a|b
            var n = optionsModel.count
            var i
            for( i = 0; i < n; ++i )
            {
                var opt = optionsModel.get(i)
                arg = opt.key
                if(arg.match(key))
                    continue

                if(!(group.mandatory.match(arg) ||
                     (group.optional !== null && group.optional.match(arg))))
                    continue

                // activate value using default from JSON
                var changed = false
                if(typeof group.changed !== "undefined")
                    changed = (group.changed.match(arg) !== null)
                if(changed === true &&
                   !opt.value.length)
                    opt.value = opt.default

                if(!(opt.value.length !== 0 &&
                     !(opt.type === "bool" &&
                       opt.value === "false")))
                    continue

                if(command_set_option.length === 0)
                {
                    if(opt.key === "#" || opt.key === "@")
                        ;
                    else if(opt.key.length > 1)
                        arg = "--" + opt.key
                    else if(key.length === 1)
                        arg = "-" + opt.key
                }
                v = JSON.stringify(opt.value);
                if(v.length &&
                    !(opt.type === "bool"))
                    if(typeof command_set_delimiter !== "undefined")
                    {
                        if(typeof opt.value === "string")
                        {
                            if(opt.value.length &&
                                !(opt.type === "bool" && opt.value === "false"))
                            {
                                if(arg.length > 0)
                                    arg += command_set_delimiter + opt.value
                                else
                                    arg += opt.value
                            }
                        }
                        else
                        {
                            if(arg.length > 0)
                                arg += command_set_delimiter + v
                            else
                                arg += v
                        }
                    }

                count = args.length
                if(command_set_option.length === 0)
                    args[args.length] = arg
                else
                {
                    args[args.length] = command_set_option
                    args[args.length] = arg
                }
            }

            statusText = JSON.stringify(args)
            processSet.start( processSetCommand, args )
            setBusyTimer.start()
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
        // icon is set in oyjlArgsQmlStart_() {app.setWindowIcon()}
        //icon = ":/images/logo-sw" //do not use here, as it will block custom icon
        logo = "/images/logo-sw"
    }

    property var permission: []
    property var permission_description: []
    onPermissionChanged: // show description for single permission; skip granted and dont ask ones
        if(permission.length > 0)
        {
            var desc = permission_description[0];
            top_message = desc;
        }
    top.onVisibleChanged: {
        var vis = top.visible
        if(vis === false)
        {
            var reply = appData.requestPermission(permission[0]);
            setHelpText( reply, false )
            permission_description.shift()
            var p = permission
            p.shift()
            permission = p
        }
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
        if(helpText.charAt(0) === '<') // assume rich text
            helpTextArea.textFormat = Qt.RichText
        else
            helpTextArea.textFormat = Qt.PlainText

        var t = helpText
        if(t.match(/\033\[/)) // convert ansi color + format codes to HTML markup
        {
            t = t.replace(/\033\[1m/g, "<b>")
            t = t.replace(/\033\[3m/g, "<i>")
            t = t.replace(/\033\[4m/g, "<u>")
            t = t.replace(/\033\[0;31m/g, "<b>")
            t = t.replace(/\033\[0;32m/g, "<b>")
            t = t.replace(/\033\[0;34m/g, "<b>")
            t = t.replace(/\033\[0m/g, "</u></b></i>")
            t = t.replace(/ /g, '&nbsp;')
            t = t.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;')
            t = t.replace(/\n/g, "<br />")
            t = "<div style\"word-wrap:nowhere;font-family:monospace;\"" + t + "</div>"
            helpText = t
            helpTextArea.font.family = "sans";
            helpTextArea.textFormat = Qt.RichText
        }

        helpTextChanging = false
        image.opacity = 0.01
        helpTextArea.opacity = 1.0
    }
    function setHelpText( text, is_mono )
    {
        if(is_mono === true)
            helpTextArea.font.family = "monospace";
        else
            helpTextArea.font.family = "sans";
        helpText = text;
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
            objectName: "firstPage"

            SplitView {
                id: split
                objectName: "split"
                width: firstPage.width
                height: firstPage.height
                orientation: Qt.Vertical

                Column {
                    id: column
                    objectName: "column"
                    width: parent.width
                    height: firstPage.height * 0.67
                    padding: dens

                    TextArea {
                        id: introTextField
                        objectName: "introTextField"
                        width: firstPage.width - 2*dens

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
                        height: firstPage.height - introTextField.height - helpFlickable.height - 2*dens
                        model: optionsModel
                        callback: interactiveCallback
                        color: bg
                    }
                }

                Flickable {
                    id: helpFlickable
                    objectName: "helpFlickable"
                    width: firstPage.width - dens

                    flickableDirection: Flickable.VerticalFlick

                    Image {
                        id: image
                        objectName: "image"
                        width: firstPage.width
                        height: helpFlickable.height
                        horizontalAlignment: Image.AlignHCenter
                        fillMode: Image.PreserveAspectFit
                    }

                    TextArea.flickable: TextArea {
                        id: helpTextArea
                        objectName: "helpTextArea"
                        width: helpFlickable.width
                        height: helpFlickable.height

                        Accessible.name: "Text Area Help"

                        font.family: "monospace"
                        textFormat: Qt.RichText // Html
                        wrapMode: TextEdit.Wrap
                        text: helpText

                        color: fg
                        background: Rectangle { color: bg }
                    }
                    ScrollBar.vertical: ScrollBar { }
                }
            }
        }
        HalfPage {
            id: twoPage
            objectName: "twoPage"
            color: bg

            Rectangle {
                width: parent.width
                height: logoImage.height
                color: myPalette.window
                Image {
                    id: logoImage
                    width: parent.width
                    horizontalAlignment: Image.AlignHCenter
                    fillMode: Image.PreserveAspectFit
                    source: uiLogo
                }
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

                    color: fg
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
                ScrollBar.vertical: ScrollBar { }
            }
        }
        Rectangle {
            id: threePage
            objectName: "threePage"
            width: pages.width
            height: pages.height
            color: "transparent"

            Flickable {
                width: threePage.width
                height: threePage.height
                flickableDirection: Flickable.VerticalFlick
                TextArea.flickable: TextArea {
                    id: textArea2

                    color: fg
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
                ScrollBar.vertical: ScrollBar { }
            }
        }
        Rectangle {
            width: pages.width
            height: pages.height
            id: aboutPage
            objectName: "aboutPage"
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

    function setOptions( group, groupName, groupDescription )
    {
        if(typeof groupDescriptions[groupName] === "undefined")
            groupDescriptions[groupName] = groupDescription

        var options = group.options
        for( var index in options )
        {
            var opt = options[index];
            var def = opt.default;
            var current = -1;
            var suggest;
            var choices = opt.choices
            var type = opt.type
            var dbl = {"start":0,"end":1}
            var run = 0
            // see mandatory key
            if(group.mandatory.length && group.mandatory.match(opt.key))
                run = 1
            // detect optonal active role
            else if(group.mandatory.length === 0 && group.optional.length && group.optional.match(opt.key))
                run = 2
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
            var l = 0;
            if(typeof name !== "undefined" && name !== null)
                l = name.length;
            if( l === 0 )
              name = opt.key;
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
                default: opt.default,
                group: group,
                run: run,
                value: ""
            }
            var text = JSON.stringify(o);
            o.text = text;
            optionsModel.append(o)
        }
    }

    Process { id: logoProcess; onReadyRead: {uiLogo = readAll();
            var name = uiLogo
            if(app_debug) statusText = "found uiLogo(logoProcess): " + name;
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

        var name = appData.findLogo( cmm.logo )
        if(name !== "undefined")
        {
            if(app_debug)
                statusText = "found uiLogo(setDataText): " + name;
            icon = name
        }
        if(name.substr(0,1,":") === ":")
            name = name.replace(/:/, 'qrc:')
        if(name !== "undefined")
            uiLogo = name
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
            if(!(name === null && item.type === "documentation"))
                html += "<tr><td align=\"right\" style=\"padding-right:10;word-wrap:break-word;\">" + label + ":</td><td style=\"font-weight:bold;\">" + name
            if( typeof desc !== "undefined" && item.type !== "documentation" )
                html += "<br /> " + desc
            if( typeof desc !== "undefined" && item.type === "documentation" )
                cmmHelp = desc
            html += "</td></tr>"

            if(item.type === "permissions")
            {
                var perm_arr = name.split("\n");
                var perm_descs = desc.split("\n");
                var newpa = []; // new permission array
                var newpd = []; // new permission_description array
                var pos = 0;
                perm_arr.forEach(function(item, index, array) {
                    if(item.substr(0,7) === "android" && Qt.platform.os === "android")
                    {
                        var has = appData.hasPermission(item);
                        if(has !== 0) // filter granted-and dont ask permissions out
                        {
                            helpText += item + " " + (has === 1 ? qsTr("granted") : qsTr("denied")) // Android vocables
                            if(has === -1)
                                helpText += " - " + qsTr("Never ask again") // Android vocables
                            helpText += "\n"
                        } else
                        {
                            newpa[pos] = item;
                            newpd[pos] = perm_descs[index];
                            ++pos;
                        }
                    }
                  });
                permission_description = newpd
                permission = newpa
            }
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
            var mandatory = group.mandatory
            var optional  = group.optional
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
                    setOptions( group, groupName2, help2 )
                }
            else
            {
                desc = P.getTranslatedItem( group, "description", loc )
                if(typeof desc !== "undefined")
                    groupName = desc
                setOptions( group, groupName, help )
            }
        }
    }

    closeFunction: function() { appData.writeJSON( outputJSON ) }
}
