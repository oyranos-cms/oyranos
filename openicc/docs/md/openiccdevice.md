# openicc\-device v0.1.1 {#openiccdevice}
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

*"openicc\-device"* *1* *""* "User Commands"
## NAME <a name="name"></a>
openicc\-device v0.1.1 \- OpenICC Device
## SYNOPSIS <a name="synopsis"></a>
**openicc\-device** <strong>\-l</strong> [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-j</strong>] [<strong>\-n</strong>] [<strong>\-b</strong>=<em>FILENAME</em>] [<strong>\-v</strong>]
<br />
**openicc\-device** <strong>\-a</strong> <strong>\-f</strong>=<em>FILENAME</em> [<strong>\-b</strong>=<em>FILENAME</em>] [<strong>\-v</strong>]
<br />
**openicc\-device** <strong>\-e</strong> <strong>\-d</strong>=<em>NUMBER</em> [<strong>\-b</strong>=<em>FILENAME</em>] [<strong>\-v</strong>]
<br />
**openicc\-device** <strong>\-p</strong> [<strong>\-s</strong>] [<strong>\-v</strong>]
<br />
## DESCRIPTION <a name="description"></a>
Manipulation of OpenICC color management data base device entries.
## OPTIONS <a name="options"></a>
### Print the Devices in the DB
&nbsp;&nbsp;**openicc\-device** <strong>\-l</strong> [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-j</strong>] [<strong>\-n</strong>] [<strong>\-b</strong>=<em>FILENAME</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-devices</strong></td> <td>List Devices</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device</strong> <em>NUMBER</em></td> <td>Device position (NUMBER:0 [≥0 ≤100])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--dump-json</strong></td> <td>Dump OpenICC JSON</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--long</strong></td> <td>List all key/values pairs</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong> <em>FILENAME</em></td> <td>DB File Name<br />File Name of OpenICC Device Data Base JSON </tr>
</table>

### Add Device to DB
&nbsp;&nbsp;**openicc-device** <strong>\-a</strong> <strong>\-f</strong>=<em>FILENAME</em> [<strong>\-b</strong>=<em>FILENAME</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--add</strong></td> <td>Add Device to DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--file-name</strong> <em>FILENAME</em></td> <td>File Name<br />The File Name of the OpenICC Device in Json format. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong> <em>FILENAME</em></td> <td>DB File Name<br />File Name of OpenICC Device Data Base JSON </tr>
</table>

### Erase a Devices from the DB
&nbsp;&nbsp;**openicc-device** <strong>\-e</strong> <strong>\-d</strong>=<em>NUMBER</em> [<strong>\-b</strong>=<em>FILENAME</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--erase-device</strong></td> <td>Erase Devices</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device</strong> <em>NUMBER</em></td> <td>Device position (NUMBER:0 [≥0 ≤100])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong> <em>FILENAME</em></td> <td>DB File Name<br />File Name of OpenICC Device Data Base JSON </tr>
</table>

### Show Filepath to the DB
&nbsp;&nbsp;**openicc-device** <strong>\-p</strong> [<strong>\-s</strong>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--show-path</strong></td> <td>Show Path</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--scope</strong></td> <td>System</td> </tr>
</table>

### General options

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong> <em>FILENAME</em></td> <td>DB File Name<br />File Name of OpenICC Device Data Base JSON </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong> <em>json|json+command|man|markdown</em></td> <td>Export formated text<br />Get UI converted into text formats
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X man</strong></td><td># Man</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X markdown</strong></td><td># Markdown</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json</strong></td><td># Json</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json+command</strong></td><td># Json + Command</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X export</strong></td><td># Export</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>verbose</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Help</td> </tr>
</table>

## AUTHOR <a name="author"></a>
Kai-Uwe Behrmann http://www.openicc.org
## COPYRIGHT <a name="copyright"></a>
*Copyright 2018 Kai\-Uwe Behrmann*


### License <a name="license"></a>
newBSD
## BUGS <a name="bugs"></a>
[https://www.github.com/OpenICC/config/issues](https://www.github.com/OpenICC/config/issues)

