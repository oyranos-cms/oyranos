# openicc\-device v0.1.1 {#openiccdevice}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"openicc-device"</strong> *1* <em>""</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

openicc-device v0.1.1 - OpenICC Device

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>openicc-device</strong> <a href="#list-devices"><strong>-l</strong></a> [<strong>-d</strong>=<em>NUMBER</em>] [<strong>-j</strong>] [<strong>-n</strong>] [<strong>-b</strong>=<em>FILENAME</em>] [<strong>-v</strong>]
<br />
<strong>openicc-device</strong> <a href="#add"><strong>-a</strong></a> <strong>-f</strong>=<em>FILENAME</em> [<strong>-b</strong>=<em>FILENAME</em>] [<strong>-v</strong>]
<br />
<strong>openicc-device</strong> <a href="#erase-device"><strong>-e</strong></a> <strong>-d</strong>=<em>NUMBER</em> [<strong>-b</strong>=<em>FILENAME</em>] [<strong>-v</strong>]
<br />
<strong>openicc-device</strong> <a href="#show-path"><strong>-p</strong></a> [<strong>-s</strong>] [<strong>-v</strong>]
<br />
<strong>openicc-device</strong> <a href="#help"><strong>-h</strong></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

Manipulation of OpenICC color management data base device entries.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="list-devices">Print the Devices in the DB</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-l</strong> [<strong>-d</strong>=<em>NUMBER</em>] [<strong>-j</strong>] [<strong>-n</strong>] [<strong>-b</strong>=<em>FILENAME</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-devices</strong></td> <td>List Devices</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device</strong>=<em>NUMBER</em></td> <td>Device position (NUMBER:0 [≥0 ≤100 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--dump-json</strong></td> <td>Dump OpenICC JSON</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--long</strong></td> <td>List all key/values pairs</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong>=<em>FILENAME</em></td> <td>DB File Name<br />File Name of OpenICC Device Data Base JSON  </td>
 </tr>
</table>

<h3 id="add">Add Device to DB</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-a</strong> <strong>-f</strong>=<em>FILENAME</em> [<strong>-b</strong>=<em>FILENAME</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--add</strong></td> <td>Add Device to DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--file-name</strong>=<em>FILENAME</em></td> <td>File Name<br />The File Name of the OpenICC Device in Json format.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong>=<em>FILENAME</em></td> <td>DB File Name<br />File Name of OpenICC Device Data Base JSON  </td>
 </tr>
</table>

<h3 id="erase-device">Erase a Devices from the DB</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-e</strong> <strong>-d</strong>=<em>NUMBER</em> [<strong>-b</strong>=<em>FILENAME</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--erase-device</strong></td> <td>Erase Devices</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device</strong>=<em>NUMBER</em></td> <td>Device position (NUMBER:0 [≥0 ≤100 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong>=<em>FILENAME</em></td> <td>DB File Name<br />File Name of OpenICC Device Data Base JSON  </td>
 </tr>
</table>

<h3 id="show-path">Show Filepath to the DB</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-p</strong> [<strong>-s</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--show-path</strong></td> <td>Show Path</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--system</strong></td> <td>Local System</td> </tr>
</table>

<h3 id="help">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-h</strong> <strong>-X</strong>=<em>json|json+command|man|markdown</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong>=<em>FILENAME</em></td> <td>DB File Name<br />File Name of OpenICC Device Data Base JSON  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Export formated text<br />Get UI converted into text formats
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> man</td><td># Man : Unix Man page - Get a unix man page</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> markdown</td><td># Markdown : Formated text - Get formated text</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json</td><td># Json : GUI - Get a Oyjl Json UI declaration</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json+command</td><td># Json + Command : GUI + Command - Get Oyjl Json UI declaration incuding command</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : All available data - Get UI data for developers</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>verbose</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Help</td> </tr>
</table>


<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.openicc.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright 2018 Kai-Uwe Behrmann*


<a name="license"></a>
### License
newBSD <a href="http://www.openicc.org">http://www.openicc.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/OpenICC/config/issues">https://www.github.com/OpenICC/config/issues</a>

