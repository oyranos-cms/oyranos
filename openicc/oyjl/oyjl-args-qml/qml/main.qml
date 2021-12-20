/** @file main.qml
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2021 (C) Kai-Uwe Behrmann
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
import "marked.js" as Mark

AppWindow {
    id: mainWindow
    objectName: "mainWindow"

    width: if( fullscreen === false )
               100*dens
           else
               width

    signal fileChanged(var variable) // Input
    onFileChanged: { setDataText( appData.getJSON( variable ) ); var fn = "---"; if(variable[0] !== '{') fn = variable;
        statusText = qsTr("Loaded") + " " + fn
        if(processSetCommand.length)
            processSet.start( processSetCommand, processSetArgs );
        var json = appData.dumpOptions();
        setHelpText(json);
    }

    signal outputChanged( var variable ) // Output
    property string outputJSON: ""
    onOutputChanged: { outputJSON = variable; statusText = "output = " + outputJSON }

    signal commandsChanged( var variable ) // Commands
    property string commandsJSON: ""
    onCommandsChanged: {
        var v = variable
        var cJ = commandsJSON
        if(variable === "+")
          commandsJSON = appDataJsonString
        else
        {
          if(variable[0] === '{')
            commandsJSON = variable;
          else
            commandsJSON = appData.readFile( variable );
            if(commandsJSON.length === 0)
                commandsJSON = "{ \"command_set\":\"" + variable + "\" }";
        }
        textArea2.text = commandsJSON
        cJ = commandsJSON
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

    property var currentArgs: [] // args for the command_set_option + active options
    Process { id: processGet;
        onReadChannelFinished: {
            var text = readAll();
            textArea2.text = text;
            setDataText(textArea2.text);
        }
    }
    property string processGetCommand: ""
    property var processGetArgs: [ ]
    onProcessGetArgs: statusText = "processGetArgs: " + processGetArgs

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
                if(command_key === "h" || command_key === "V")
                    setHelpText( data, false )
                else
                    setHelpText( data, true )
            }
            var text = readErr();
            if(text.length)
                textArea2.text = text;

            // The next process needs to be in serial for stdout and stderr to work.
            var pGC = processGetCommand

            // add all actual args in order to show them the pGC
            var pGA = []
            var n = processGetArgs.length
            var i
            for(i = 0; i < n; ++i)
                pGA.push(processGetArgs[i])
            var cA = currentArgs
            n = cA.length
            for(i = 0; i < n; ++i)
                if(!hasArg(pGA,cA[i]))
                    pGA.push(cA[i])

            if(processGetCommand.length && setOnly <= 0)
            {
                if(app_debug)
                    statusText = "command_get: " + processGetCommand + " " + pGA + " ex:" + mandatory_exclusive
                processGet.start( processGetCommand, pGA, "getUi" )
                processGet.waitForFinished()
                if(app_debug)
                    statusText = "command_get: " + processGetCommand + " " + pGA + " finished"
            }
            currentArgs = []

            unsetBusyTimer.start()
        }
    }
    property string processSetCommand: ""
    property var processSetArgs: [ ]
    property string command_set_delimiter: "="
    property string command_set_option: ""
    property string command_key: ""


    function argToKey(arg)
    {
        var okey = arg
        if(okey[0] === '-')
            okey = okey.substr(1,okey.length-1)
        if(okey[1] === '-')
            okey = okey.substr(2,okey.length-2)
        if(okey.match('='))
            okey = okey.substr(0,okey.indexOf('='))
        return okey;
    }

    function hasArg(args, key)
    {
        var arr = [];
        var arrn = 0;
        if(app_debug)
            statusText = "hasArgs( args=" + args + ", key=" + key + " )"
        if(typeof args === "string")
        {
            arr = args.split(new RegExp('[,|]', 'g'))
            arrn = arr.length
        }
        var j
        for( j = 0; j < arrn; ++j )
        {
            var okey = argToKey(arr[j])
            var k = argToKey(key)
            if(app_debug)
                statusText = "arr[j]=" + arr[j] + " okey=" + okey + " key=" + key + " k=" + k
            if(okey == k)
                return true;
        }
        return false;
    }

    function addArg1( args, key, value, type, sub_command, split )
    {
        var arg = null;
        if(key === "#" || key === "@")
            arg = null;
        else if(key.length > 1)
            arg = (sub_command ? "":"--") + key
        else if(key.length === 1)
            arg = "-" + key

        var v = JSON.stringify(value);

        if(type === "bool" && value === "false")
            return

        var av = appData.getOption(key);
        if(av === "false")
            return
        if(split)
        {
            if(!av.match(value))
                return
        } else
            if(typeof value !== "undefined" && av !== value)
                return

        if(av.length)
        {
            if(!split)
                value = av
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
        }

        var count = args.length
        if(command_set_option.length === 0)
        {
            if(arg !== null)
                args[count] = arg
        }
        else
        {
            args[count] = command_set_option
            if(arg !== null)
                args[count] = arg
        }
    }
    function addArg( args, key, value, type, sub_command, repetition )
    {
        if(repetition)
        {
            var arr = value.split(new RegExp('[;]', 'g'));
            var arrn = arr.length;
            var i;
            for( i = 0; i < arrn; ++i )
                addArg1( args, key, arr[i], type, sub_command, arrn > 1 );
        } else
            addArg1( args, key, value, type, sub_command, false );
    }

    property var setOnly: false
    function interactiveCallback( key, value, type, group, setOnly_ )
    {
        var opts = optionsModel
        var n = optionsModel.count
        var i
        var opt
        var explicite = typeof group.explicite !== "undefined"
        var pass = !explicite
        var mkey = false
        var arr = []
        var arrn = 0

        setOnly = setOnly_

        statusText = group.description

        var sub_command = false
        if(typeof group.sub_command !== "undefined")
            sub_command = true
        var mandatory_found = false
        if(!sub_command && typeof group.mandatory !== "undefined" && group.mandatory.length && hasArg(group.mandatory, key))
        {
            mandatory_found = true
            mkey = key;
        }
        // passed in key might be not a mandatory one ...
        if(!mandatory_found && typeof group.mandatory !== "undefined" && group.mandatory.length)
        {
            // put mandatory in front especially for sub_command style
            arr = group.mandatory.split(new RegExp('[,|]', 'g'))
            arrn = arr.length
            if(arrn)
                mkey = arr[0];
            mandatory_found = true
        }

        for( i = 0; i < n; ++i )
        {
            opt = optionsModel.get(i)
            var arg = opt.key
            var akey = mkey?mkey:key;
            if(arg == akey)
            {
                // activate value using default from JSON
                var changed = false
                if(typeof group.changed !== "undefined")
                    changed = hasArg(group.changed, arg)
                if(changed === true &&
                   !opt.value.length &&
                   opt.changed.length)
                    opt.value = opt.default
                if(!(key == akey))
                {
                    key = opt.key
                    value = opt.value
                    type = opt.type
                }
                break;
            }
            else
                opt = false
        }
        if(!pass && opt !== false && opt.immediate)
            pass = true;

        // skip "detail" only groups
        if(!pass && group.mandatory.length === 0 && group.optional.length === 0)
            return

        // skip optional options from groups with mandatory requirement
        if(!pass && group.mandatory.length && !hasArg(group.mandatory, key))
            return

        var mandatory_exclusive = false
        if(typeof group.mandatory !== "undefined" && group.mandatory.length && group.mandatory.match(/[|]/))
            mandatory_exclusive = true

        var args = []
        args = processSetArgs.slice()

        // set mandatory switch when no other mandatory option is visible in group
        if(!(args.length && args[0].match(mkey)) && key === mkey && opt.type === "bool" && value === "false")
            args[0] = sub_command ? "" : mkey.length === 1 ? "-" + mkey : "--" + mkey;


        var sCb = processSetCommand
        command_key = key
        if(processSetCommand.length && setOnly >= 0)
        {
            arg = key

            if( hasArg(args, key) === false ||
                opt.repetition === true )
                addArg( args, key, value, type, sub_command, opt.repetition )

            if(app_debug)
                statusText = "command_set: " + processSetCommand + " " + args

            // create the args from group::optional/group::mandatory options and add them to the passed in arg from above
            // TODO: detect mandatory exclusion, e.g. a|b
            for( i = 0; i < n; ++i )
            {
                opt = optionsModel.get(i)
                var found = 0
                var first_mandatory = false
                arg = opt.key
                if(mkey == arg)
                    continue

                if(typeof group.mandatory !== "undefined")
                {
                    arr = group.mandatory.split(new RegExp('[,|]', 'g'))
                    arrn = arr.length
                }
                var j
                for( j = 0; j < arrn; ++j )
                {
                    if(arr[j] === arg &&
                            ((j === 0 && !mandatory_found) || !mandatory_exclusive))
                    {
                        found = 1;
                        if(j === 0)
                            first_mandatory = true;
                    }
                }
                if(!found && group.optional !== null)
                {
                    if(typeof group.optional !== "undefined")
                    {
                        arr = group.optional.split(new RegExp('[,|]', 'g'))
                        arrn = arr.length
                    }
                    for( j = 0; j < arrn; ++j )
                        if(arr[j] === arg)
                            found = 1
                }
                if(found === 0)
                    continue

                // activate value using default from JSON
                changed = false
                if(typeof group.changed !== "undefined")
                    changed = (group.changed.match(arg) !== null)
                if(changed === true &&
                   !opt.value.length &&
                   opt.changed.length)
                    opt.value = opt.default

                if( !(first_mandatory && !mandatory_found) &&
                    !(opt.value.length !== 0 &&
                     !(opt.type === "bool" &&
                       opt.value === "false")))
                    continue

                if( hasArg(args, opt.key) === false ||
                    opt.repetition === true )
                    addArg( args, opt.key, opt.value, opt.type, sub_command, opt.repetition )
            }
            n = group.options.length
            for( i = 0; i < n; ++i )
            {
                opt = group.options[i]
                if( typeof opt.changed != "undefined" && opt.changed.length && hasArg(args, opt.key) === false )
                    addArg( args, opt.key, opt.value, opt.type, sub_command, opt.repetition )
            }

            if(app_debug)
                statusText = "command_set: " + processSetCommand + " " + args + " ex:" + mandatory_exclusive + " mand:" + group.mandatory

            currentArgs = [];
            currentArgs = args;

            statusText = JSON.stringify(args)
            processSet.start( processSetCommand, currentArgs, "set" )
            setBusyTimer.start()
            processSet.waitForFinished()
            if(app_debug)
                statusText = "command_set: " + processSetCommand + " " + currentArgs + " finished"
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
    property real initialHelpTextFontPointSize: 0
    onHelpTextChanged: {
        if(helpTextChanging)
            return
        helpTextChanging = true
        var start = helpText.substr(0,14);
        var t = helpText

        if(start === '<?xml version=')
            helpTextArea.textFormat = Qt.PlainText
        else
        if( t.charAt(0) === '<' ||
            t.match(/\033\[/) ||
            t.match(/oyjl-html-format-marker/) ) // assume rich text
            helpTextArea.textFormat = Qt.RichText
        else
            helpTextArea.textFormat = Qt.PlainText

        if(t.match(/\033\[/)) // convert ansi color + format codes to HTML markup
        {
            t = t.replace(/\033\[1m/g, "<b>")
            t = t.replace(/\033\[3m/g, "<i>")
            t = t.replace(/\033\[4m/g, "<u>")
            t = t.replace(/\033\[0m/g, "</u></b></i></font>")
            t = t.replace(/\033\[00m/g, "</font>")
            t = t.replace(/ /g, '&nbsp;')
            t = t.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;')
            t = t.replace(/\n/g, "</font><br />")
            // some color codes
            t = t.replace(/\033\[31m/g, "<font color=red>")
            t = t.replace(/\033\[0;31m/g, "<font color=red>")
            t = t.replace(/\033\[00;31m/g, "<font color=red>")
            t = t.replace(/\033\[01;31m/g, "<font color=red>")
            t = t.replace(/\033\[38;2;240;0;0m/g, "<font color=red>")
            t = t.replace(/\033\[32m/g, "<font color=green>")
            t = t.replace(/\033\[0;32m/g, "<font color=green>")
            t = t.replace(/\033\[00;32m/g, "<font color=green>")
            t = t.replace(/\033\[01;32m/g, "<font color=green>")
            t = t.replace(/\033\[38;2;250;0;0m/g, "<font color=green>")
            t = t.replace(/\033\[33m/g, "<font color=lime>")
            t = t.replace(/\033\[0;33m/g, "<font color=lime>")
            t = t.replace(/\033\[00;33m/g, "<font color=lime>")
            t = t.replace(/\033\[01;33m/g, "<font color=lime>")
            t = t.replace(/\033\[34m/g, "<font color=dodgerblue>")
            t = t.replace(/\033\[0;34m/g, "<font color=dodgerblue>")
            t = t.replace(/\033\[00;34m/g, "<font color=dodgerblue>")
            t = t.replace(/\033\[01;34m/g, "<font color=dodgerblue>")
            t = t.replace(/\033\[38;2;150;0;0m/g, "<font color=dodgerblue>")
            t = t.replace(/\033\[35m/g, "<font color=magenta>")
            t = t.replace(/\033\[0;35m/g, "<font color=magenta>")
            t = t.replace(/\033\[00;35m/g, "<font color=magenta>")
            t = t.replace(/\033\[01;35m/g, "<font color=magenta>")
            t = t.replace(/\033\[36m/g, "<font color=cyan>")
            t = t.replace(/\033\[0;36m/g, "<font color=cyan>")
            t = t.replace(/\033\[00;36m/g, "<font color=cyan>")
            t = t.replace(/\033\[01;36m/g, "<font color=cyan>")

            t = "<div style\"word-wrap:nowhere;\">" + t + "</div>"
            helpText = Link.linkify( t );
            helpTextArea.font.family = "sans";
            helpTextArea.textFormat = Qt.RichText
        }
        else if(t.match(/^{/)) // convert JSON to HTML markup
        {
            var odd = true;
            var high = "";
            for( var i = 0; i < t.length; ++i )
            {
                var item = t[i]
                if(item === '"')
                {
                    if(odd)
                        high += item + '<i>'
                    else
                        high += "</i>" + item
                    odd = !odd
                }
                else
                    high += item;
            };
            high = high.replace(/ /g, '&nbsp;')
            high = high.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;')
            high = high.replace(/\n/g, "<br />")
            helpText = Link.linkify( high );
            helpTextArea.font.family = "sans";
            helpTextArea.textFormat = Qt.RichText
        }
        else if(t.match(/^---/)) // convert YAML to HTML markup
        {
            high = "";
            for( i = 0; i < t.length; ++i )
            {
                item = t[i]
                if(item === '\n')
                    high += '</i></b>' + item + '<b>'
                else if(item === ':')
                    high += '</b>' + item + '<i>'
                else
                    high += item;
            };
            high = high.replace(/ /g, '&nbsp;')
            high = high.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;')
            high = high.replace(/\n/g, "<br />")
            helpText = Link.linkify( high );
            helpTextArea.font.family = "sans";
            helpTextArea.textFormat = Qt.RichText
        }
        else if(t.match(/^</)) // convert XML to HTML markup
        {
            high = "";
            for( i = 0; i < t.length; ++i )
            {
                item = t[i]
                if(item === '<')
                    high += '</i>&lt;<b>'
                else if(item === '>')
                    high += '</b>&gt;<i>'
                else
                    high += item;
            };
            high = high.replace(/ /g, '&nbsp;')
            high = high.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;')
            high = high.replace(/\n/g, "<br />")
            helpText = high
            helpTextArea.font.family = "sans";
            helpTextArea.textFormat = Qt.RichText
        }
        else if(t.match(/^#/) && !t.match(/^#include/)) // try markdown
        {
            var m = Mark.mark(this)
            high = m.parse(t)
            helpText = high;
            helpTextArea.font.family = "sans";
            helpTextArea.textFormat = Qt.RichText
        } else if(!t.match(/oyjl-html-format-marker/))
        {
            var text = Link.linkify( t );
            if(text.length !== helpText.length)
            {
                t = t.replace(/</g, '&lt;')
                t = t.replace(/>/g, '&gt;')
                t = t.replace(/ /g, '&nbsp;')
                t = t.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;')
                t = t.replace(/\n/g, "<br />")
                text = Link.linkify( t );
                helpTextArea.textFormat = Qt.RichText
            }
            helpText = text;
        } else
          helpText = t;
        helpText = helpText.replace(/oyjl-html-format-marker/, '')

        helpTextChanging = false
        image.opacity = 0.01
        helpTextArea.opacity = 1.0
    }
    function setHelpText( text, is_mono )
    {
        if(initialHelpTextFontPointSize === 0)
        {
            var n = helpTextArea.font.pointSize * 1.0;
            initialHelpTextFontPointSize = n;
        }

        var t = JSON.stringify(text)
        helpTextArea.readOnly = true // deselect selection and it's font attributes
        helpTextArea.font.pointSize = initialHelpTextFontPointSize; // reset font size
        helpTextArea.readOnly = false

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
        }
        HalfPage {
            id: twoPage
            objectName: "twoPage"
            color: bg

            Rectangle {
                width: parent.width
                height: logoImage.height
                color: "gray" // myPalette.window
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
        Rectangle {
            width: pages.width
            height: pages.height
            id: logPage
            objectName: "logPage"
            color: bg

            Flickable {
                id: textAreaLogFlick
                width: parent.width
                height: parent.height - font.pixelSize * 3 // keep some space for the button

                Keys.onDownPressed: { contentY += 3*h; returnToBounds() }
                Keys.onUpPressed:   { contentY -= 3*h; returnToBounds() }
            TextArea.flickable: TextArea {
                id: textAreaLog

                Accessible.name: "Text Area Log"
                width: logPage.width
                height: logPage.height
                color: fg
                background: Rectangle { color: bg }
                textFormat: Qt.PlainText
                wrapMode: TextEdit.Wrap
                readOnly: true
                text: showJson ? appDataJsonString : logHistory
            }
            }
            Button {
                id: textButtonLog
                width: parent.width - textAreaLog.font.pixelSize * 2 // make this button big
                x: parent.width/2 - width/2 // place in the middle
                anchors.top: textAreaLogFlick.bottom
                text: showJson ? qsTr("Show Log") : qsTr("Show JSON")
                onClicked: {
                    showJson = !showJson
                }
            }
        }
    }

    property string dataText;
    property var appJsonObject;
    property real deviceLabelWidth: 20;
    property string loc: "";
    property var catalog: {"translations":""}

    function setOptions( group, groupName, groupDescription )
    {
        if(typeof groupDescriptions[groupName] === "undefined")
            groupDescriptions[groupName] = groupDescription

        var explicite = typeof group.explicite !== "undefined"

        var options = group.options
        for( var index in options )
        {
            var opt = options[index];
            var def = opt.default;
            var key = opt.key
            var nick = opt.nick;
            var current = "";
            var suggest = "";
            var choices = [];
            var type = "";
            var dbl = {"start":0,"end":1}
            var immediate = typeof opt.immediate !== "undefined"
            var repetition = typeof opt.repetition !== "undefined"
            var run = 0
            var value = ""
            var value_name = ""
            var default_var = opt.default
            var loc_var = loc
            var changed = ""

            if(typeof opt.type !== "undefined")
                type = opt.type
            if(typeof opt.value_name !== "undefined")
              value_name = opt.value_name;
            // see mandatory key
            if(typeof group.mandatory !== "undefined" && group.mandatory.length && group.mandatory.match(opt.key))
                run = 1
            // detect optonal active role
            else if(typeof group.mandatory !== "undefined" && group.mandatory.length === 0 && group.optional.length && group.optional.match(opt.key))
                run = 2
            if( type === "double" )
                // try slider
            {
                var start = 0
                var end = 1
                var tick = 0
                if(typeof opt.start !== "undefined") start = opt.start
                if(typeof opt.end !== "undefined") end = opt.end
                if(typeof opt.tick !== "undefined") tick = opt.tick
                if(typeof opt.default !== "undefined")
                {
                    current = opt.default
                    value = current;
                }
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
                if( opt.default === "1" )
                    value = "true";
                else
                    value = "false";
            }
            if( type === "string")
            {
                var opt_ = opt
                if(typeof opt_.suggest !== "undefined")
                    suggest = opt_.suggest;
            }

            if(typeof opt.choices !== "undefined")
                choices = opt.choices;
            var name
            if(type === "choice")
            for( var i in choices )
            {
                if( typeof choices[i].name != "string" )
                    continue;
                var item = choices[i];
                var cnick = item.nick;
                if(cnick === def)
                    current = name;
            }

            if(typeof opt.changed !== "undefined")
            {
                current = value.length ? value : opt.changed;
                changed = current;
            }

            if(changed.length)
                appData.setOption(key, changed);

            name = P.getTranslatedItem( opt, "name", loc, catalog );
            var l = 0;
            if(typeof name !== "undefined" && name !== null)
                l = name.length;
            if( l === 0 )
              name = opt.key;
            var desc = P.getTranslatedItem( opt, "description", loc, catalog );
            var help = P.getTranslatedItem( opt, "help", loc, catalog );
            if(typeof groupName === "undefined")
                groupName = ""
            if(typeof nick === "undefined")
                nick = ""
            if(typeof desc === "undefined")
                desc = ""
            if(typeof help === "undefined")
                help = ""
            if(typeof default_var === "undefined")
                default_var = "0"
            var o = {
                choices: choices,
                current: current,
                changed: changed,
                dbl: dbl,
                default: default_var,
                description: desc,
                group: group,
                groupName: groupName,
                help: help,
                immediate: immediate,
                key: key,
                loc: loc_var,
                name: name,
                nick: nick,
                repetition: repetition,
                run: run,
                suggest: suggest,
                type: type,
                value: value,
                value_name: value_name
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
        appData.clearOptions()
        dataText = t;
        appDataJsonString = t;
        appJsonObject = JSON.parse(t);
        if( appJsonObject === "undefined" )
            logText = "Parsing failed: " + t;
        var j = appJsonObject;
        if(typeof j.LOCALE_info !== "undefined")
            loc = j.LOCALE_info;
        if(typeof j.org !== "undefined" &&
           typeof j.org.freedesktop !== "undefined" &&
           typeof j.org.freedesktop.oyjl !== "undefined" &&
           typeof j.org.freedesktop.oyjl.translations !== "undefined" )
                catalog = j.org.freedesktop.oyjl.translations

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
        introText = P.getTranslatedItem( cmm, "name", loc, catalog );
        appName = introText
        // extract CMM infos
        var html
        cmmHelp = ""
        html = "<html><body><p align=\"center\"><table border=\"0\" style=\"border-spacing:10px\">" +
                "<tr><td align=\"right\" style=\"padding-right:10;\">" + P.getTranslatedItem( cmm, "label", loc, catalog ) + ":</td><td style=\"font-weight:bold;\">" + P.getTranslatedItem( cmm, "description", loc, catalog ) + "</td></tr>"
        html += "<tr><td align=\"right\" style=\"padding-right:10;\">ID:</td><td style=\"font-weight:bold;\">" + cmm.nick + "</td>"
        for( var index in cmm.information )
        {
            var item = cmm.information[index]
            if(item.type === "date")
                continue
            var label = P.getTranslatedItem( item, "label", loc, catalog );
            name = P.getTranslatedItem( item, "name", loc, catalog );
            var desc = P.getTranslatedItem( item, "description", loc, catalog );
            if(!((typeof(name) === "undefined" || name === null) && item.type === "documentation"))
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
            var groupName = P.getTranslatedItem( group, "name", loc, catalog );
            if(typeof groupName === "undefined")
            {
                desc = P.getTranslatedItem( group, "description", loc, catalog )
                if(typeof desc !== "undefined")
                    groupName = desc
            }
            var help = P.getTranslatedItem( group, "help", loc, catalog );

            var synopsis = "";
            if(typeof group.synopsis !== "undefined")
            {
              synopsis = group.synopsis;
              if(synopsis.length)
              {
                  if(typeof help !== "undefined")
                  {
                      help = help.replace(/ /g, '&nbsp;')
                      help = help.replace(/\t/g, '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;')
                      help = help.replace(/\n/g, "<br />")
                      help = Link.linkify( help );
                      help += "<br /><br />" + synopsis;
                  }
                  else
                      help = synopsis;
              }
            }
            if(typeof options !== "undefined")
                setOptions( group, groupName, help )

            // render one group level below
            if( typeof group.groups !== "undefined" )
                for( var g2 in group.groups )
                {
                    var g = group.groups[g2]
                    options = g.options
                    desc = P.getTranslatedItem( g, "name", loc, catalog )
                    if(typeof desc === "undefined")
                        desc = P.getTranslatedItem( g, "description", loc, catalog )
                    if(typeof desc !== "undefined")
                        desc = Link.linkify( desc );
                    var groupName2 = groupName + " : " + desc
                    var help2 = ""
                    if(typeof help !== "undefined")
                        help2 = help + "<br /><br />"
                    if(typeof groupName2 !== "undefined")
                        help2 += P.getTranslatedItem( g, "help", loc, catalog );
                    if(typeof g.synopsis !== "undefined")
                    {
                      synopsis = g.synopsis;
                      if(synopsis.length)
                        help2 +="<br />" + synopsis;
                    }
                    statusText = "Synopsis: " + synopsis
                    setOptions( g, groupName2, help2 )
                }
        }
    }

    closeFunction: function() { appData.writeJSON( outputJSON ) }
}
