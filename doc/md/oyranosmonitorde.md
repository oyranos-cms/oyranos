# oyranos-monitor v0.9.7 {#oyranosmonitorde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [ENVIRONMENT](#environment) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyranos-monitor"</strong> *1* <em>"February 22, 2018"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-monitor v0.9.7 - Monitor configuration with Oyranos CMS

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-monitor</strong> | <strong>-s</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-monitor</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--system-wide</strong>] [<strong>-v</strong>] ICC_DATEINAME
<br />
<strong>oyranos-monitor</strong> <a href="#erase"><strong>-e</strong></a> <strong>-u</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--system-wide</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#list"><strong>-l</strong></a> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--short</strong>] [<strong>--path</strong>] [<strong>-c</strong>] [<strong>-v</strong>] [<strong>--module</strong>=<em>MODULENAME</em>] [<strong>-2</strong>] [<strong>-4</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#format"><strong>-f</strong>=<em>edid|icc|edid_icc|vcgt...</em></a> <strong>-a</strong> <strong>--modules</strong> <strong>-L</strong> [<strong>-o</strong>=<em>OUT_FILENAME|default:stdout</em>] [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--display</strong>=<em>NAME</em>] [<strong>-m</strong>] [<strong>-v</strong>] [<strong>-2</strong>] [<strong>-4</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#daemon"><strong>--daemon</strong></a> [<strong>-v</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#add-edid"><strong>--add-edid</strong>=<em>DATEINAME</em></a> <strong>--add-vcgt</strong>=<em>DATEINAME</em> <strong>--profile</strong>=<em>ICC_DATEINAME</em> [<strong>--name</strong>=<em>NAME</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-R</strong>=<em>gui|cli|web|...</em> <strong>-V</strong> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

The oyranos-monitor programm let you query and set the monitor profile(s) within the Oyranos color management system (CMS). Profiles are stored in a data base, the Oyranos DB.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3>Setze grundlegende Parameter</h3>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>=<em>PIXEL</em></td> <td>select the screen at the x position. The Oyranos screens are if possible real devices. A screen position must be unique and not shared by different screens, otherwise it will be ambiguous.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>=<em>PIXEL</em></td> <td>select the screen at the y position.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device-pos</strong>=<em>NUMMER</em></td> <td>position in device list. The numbering of monitors starts with zero for the first device. To get the number of all available devices use "oyranos-monitor -l | wc -l".  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--display</strong>=<em>NAME</em></td> <td>Use DISPLAY  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--module</strong>=<em>MODULENAME</em></td> <td>select module  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--x-color-region-target</strong></td> <td>show the X Color Management (XCM) device profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--system-wide</strong></td> <td>do system wide, might need admin or root privileges</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Wähle ICC V2 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Wähle ICC V4 Profile</td> </tr>
</table>

<h3>Activate Profiles</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> | <strong>-s</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>-v</strong>]

&nbsp;&nbsp;Set up the X11 server with the Oyranos DB stored monitor ICC profile(s).

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'>|</td> <td>No args<br />Run command without arguments</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--setup</strong></td> <td>Setup a ICC profile for all or a selected monitor</td> </tr>
</table>

<h3>Set New Profile</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--system-wide</strong>] [<strong>-v</strong>] ICC_DATEINAME

&nbsp;&nbsp;Assign a monitor ICC profile to the selected screen and store in the Oyranos DB.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><em>ICC_DATEINAME</em></td> <td>ICC device profile for a monitor  </td>
 </tr>
</table>

<h3 id="erase">Erase Profile</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>-e</strong> <strong>-u</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--system-wide</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--erase</strong></td> <td>Release a ICC profile from a monitor device<br />Reset the hardware gamma table to the defaults and erase from the Oyranos DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--unset</strong></td> <td>Release a ICC profile from a monitor device<br />Reset the hardware gamma table to the defaults.</td> </tr>
</table>

<h3 id="list">List Devices</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>-l</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--short</strong>] [<strong>--path</strong>] [<strong>-c</strong>] [<strong>-v</strong>] [<strong>--module</strong>=<em>MODULENAME</em>] [<strong>-2</strong>] [<strong>-4</strong>]

&nbsp;&nbsp;List names and show details.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list</strong></td> <td>list devices</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--short</strong></td> <td>Zeige nur den Profilnamen</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--path</strong></td> <td>kompletter Pfad- und Dateiname</td> </tr>
</table>

<h3 id="format">Give Informations</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>-f</strong>=<em>edid|icc|edid_icc|vcgt...</em> <strong>-a</strong> <strong>--modules</strong> <strong>-L</strong> [<strong>-o</strong>=<em>OUT_FILENAME|default:stdout</em>] [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--display</strong>=<em>NAME</em>] [<strong>-m</strong>] [<strong>-v</strong>] [<strong>-2</strong>] [<strong>-4</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong>=<em>edid|icc|edid_icc|vcgt...</em></td> <td>[edid|icc|edid_icc|vcgt] Get data<br />Select a data format. Possible are edid for server side EDID data, icc for a server side ICC profile, edid_icc for a ICC profile created  from  server side  EDID and vcgt for effect and monitor white point merged into a possibly altered VCGT tag. Without the -o/--ouput option the output is written to stdout.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>OUT_FILENAME|default:stdout</em></td> <td>OUT_FILENAME Write data selected by -f/--format to the given filename.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--device-meta-tag</strong></td> <td>(--device-meta-tag) switch is accepted by the -f=icc and -f=edid_icc option. It embeddes device and driver informations about the actual device for<br />later easier ICC profile to device assignment.  This becomes useful for sharing ICC profiles.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--xcm-active</strong></td> <td><br />Show if X Color Management is active</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--modules</strong></td> <td><br />List monitor modules</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-L</strong>|<strong>--list-taxi</strong></td> <td>Liste Taxi Profile</td> </tr>
</table>

<h3 id="daemon">Run Daemon</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>--daemon</strong> [<strong>-v</strong>]

&nbsp;&nbsp;Keep a session up to date.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--daemon</strong></td> <td><br />Run as daemon to observe monitor hotplug events and update the setup.</td> </tr>
</table>

<h3 id="add-edid">Add Device Meta Data</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>--add-edid</strong>=<em>DATEINAME</em> <strong>--add-vcgt</strong>=<em>DATEINAME</em> <strong>--profile</strong>=<em>ICC_DATEINAME</em> [<strong>--name</strong>=<em>NAME</em>] [<strong>-v</strong>]

&nbsp;&nbsp;Add device information to a profile for automated selection.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--add-edid</strong>=<em>DATEINAME</em></td> <td>EDID Filename<br />Embedd EDID keys to a ICC profile as meta tag. Requires --profile.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--add-vcgt</strong>=<em>DATEINAME</em></td> <td>VCGT Filename<br />Embedd VCGT calibration to a ICC profile as vcgt tag. FILENAME can be output from 'xcalib -p'. Requires --profile.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--profile</strong>=<em>ICC_DATEINAME</em></td> <td>ICC profile.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--name</strong>=<em>NAME</em></td> <td>String for ICC profile internal name.  </td>
 </tr>
</table>

<h3 id="help">General Options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-R</strong>=<em>gui|cli|web|...</em> <strong>-V</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> -</td><td># Vollständige Hilfe : Zeige Hilfe für alle Gruppen</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> synopsis</td><td># Übersicht : Liste Gruppen - Zeige alle Gruppen mit Syntax</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Exportiere formatierten Text<br />Hole Benutzerschnittstelle als Text
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> man</td><td># Handbuch : Unix Handbuchseite - Hole Unix Handbuchseite</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> markdown</td><td># Markdown : Formatierter Text - Hole formatierten Text</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json</td><td># Json : GUI - Hole Oyjl Json Benutzerschnittstelle</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json+command</td><td># Json + Kommando : GUI + Kommando - Hole Oyjl Json Benutzerschnittstelle mit Kommando</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : Alle verfügbaren Daten - Erhalte Daten für Entwickler. Das Format kann mit dem oyjl-args Werkzeug umgewandelt werden.</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Darstellung
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> gui</td><td># Gui : Zeige UI - Zeige eine interaktive grafische Benutzerschnittstelle.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> cli</td><td># Cli : Zeige UI - Zeige Hilfstext für Benutzerschnittstelle auf der Kommandozeile.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Starte Web Server - Starte lokalen Web Service für die Darstellung in einem Webbrowser. Die -R=web:help Unteroption zeigt weitere Informationen an.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudere mehr</td> </tr>
</table>


<h2>ENVIRONMENT <a href="#toc" name="environment">&uarr;</a></h2>

#### DISPLAY
&nbsp;&nbsp;On X11 systems the display is selected by this variable.
#### OY_DEBUG
&nbsp;&nbsp;setze den Oyranos Fehlersuchniveau. Die -v Option kann alternativ benutzt werden.
#### XDG_DATA_HOME XDG_DATA_DIRS
&nbsp;&nbsp;richte Oyranos auf obere Verzeichnisse, welche Resourcen enthalten. An die Pfade für ICC Profile wird "color/icc" angehangen. http://www.openicc.org/index.php%3Ftitle=OpenIccDirectoryProposal.html
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
#### OY_MODULE_PATH
&nbsp;&nbsp;zeige Oyranos zusätzliche Verzeichnisse mit Modulen.

<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Put the following in a setup script like .xinitrc
&nbsp;&nbsp;# select a monitor profile, load the binary blob into X and
  <br />
&nbsp;&nbsp;# fill the VideoCardGammaTable, if appropriate
  <br />
&nbsp;&nbsp;oyranos-monitor
#### Assign a ICC profile to a screen
&nbsp;&nbsp;oyranos-monitor -x pos -y pos profilename
#### Reset a screens hardware LUT in order to do a calibration
&nbsp;&nbsp;oyranos-monitor -e -x pos -y pos profilename
#### Query the server side, network transparent profile
&nbsp;&nbsp;oyranos-monitor -x pos -y pos
#### List all Oyranos monitor devices
&nbsp;&nbsp;oyranos-monitor -l
#### Show the first Oyranos monitor device verbosely
&nbsp;&nbsp;oyranos-monitor -l -v -d 0
#### Pass the monitor profile to a external tool
&nbsp;&nbsp;iccDumpProfile -v "`oyranos-monitor -l -d 0 -c --path`"
#### List all monitor devices through the oyX1 module
&nbsp;&nbsp;oyranos-monitor -l --module oyX1
#### Dump data in the format following the -f option
&nbsp;&nbsp;oyranos-monitor -f=[edid|icc|edid_icc] -o=edid.bin -x=pos -y=pos -m
#### Embedd device informations into ICC meta tag
&nbsp;&nbsp;oyranos-monitor --add-edid=edid_filename --profile=profilename.icc --name=profilename

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-config-fltk](oyranosconfigfltk.html)<a href="oyranosconfigfltk.md">(1)</a>&nbsp;&nbsp;[oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos-profiles](oyranosprofiles.html)<a href="oyranosprofiles.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2021 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

