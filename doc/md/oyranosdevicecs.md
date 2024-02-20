# oyranos\-device v0.9.7 {#oyranosdevicecs}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [GENERAL OPTIONS](#general_options) [ENVIRONMENT](#environment) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyranos-device"</strong> *1* <em>"May 13, 2021"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-device v0.9.7 - Oyranos Color Devices

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-device</strong> <a href="#assign"><strong>-a</strong></a> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em> [<strong>-p</strong><em>[=ICC_FILE_NAME]</em>] [<strong>--system-wide</strong>]
<br />
<strong>oyranos-device</strong> <a href="#erase"><strong>-e</strong></a> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em> [<strong>--system-wide</strong>]
<br />
<strong>oyranos-device</strong> <a href="#setup"><strong>-s</strong></a> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em>
<br />
<strong>oyranos-device</strong> <a href="#unset"><strong>-u</strong></a> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em>
<br />
<strong>oyranos-device</strong> <a href="#list"><strong>-l</strong></a> [<strong>-v</strong>] [<strong>--short</strong>] [<strong>--print-class</strong>]
<br />
<strong>oyranos-device</strong> <a href="#list"><strong>-l</strong></a> [<strong>-c</strong>=<em>CLASS</em>] [<strong>-d</strong>=<em>NUMBER</em>|<strong>--device-name</strong>=<em>NAME</em>] [<strong>--short</strong>] [<strong>--path</strong>] [<strong>-r</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-device</strong> <a href="#list-profiles"><strong>--list-profiles</strong></a> [<strong>-c</strong>=<em>CLASS</em>] [<strong>-d</strong>=<em>NUMBER</em>] [<strong>--show-non-device-related</strong>]
<br />
<strong>oyranos-device</strong> <a href="#list-taxi-profiles"><strong>--list-taxi-profiles</strong></a> [<strong>-c</strong>=<em>CLASS</em>] [<strong>-d</strong>=<em>NUMBER</em>] [<strong>--show-non-device-related</strong>]
<br />
<strong>oyranos-device</strong> <a href="#format"><strong>-f</strong>=<em>icc|openicc+rank-map|openicc|openicc-rank-map</em></a> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em> <strong>-j</strong>=<em>FILENAME</em> <strong>-k</strong>=<em>FILENAME</em> [<strong>-o</strong>=<em>FILENAME</em>] [<strong>--only-db</strong>] [<strong>-m</strong>]
<br />
<strong>oyranos-device</strong> <a href="#calibration"><strong>--calibration</strong>=<em>FILENAME</em></a> [<strong>-m</strong>] [<strong>-j</strong>=<em>FILENAME</em>] [<strong>-f</strong>=<em>icc|openicc+rank-map|openicc|openicc-rank-map</em>] [<strong>-n</strong>=<em>ICC_FILE_NAME</em>]
<br />
<strong>oyranos-device</strong> <a href="#help"><strong>-h</strong>=<em>synopsis|...</em></a> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The oyranos-device program shows and administrates ICC profiles for color devices.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3>Set basic parameters</h3>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--device-class</strong>=<em>CLASS</em></td> <td>use device class. Useful device classes are monitor, scanner, printer, camera.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-c</strong> monitor</td><td># Monitor</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c</strong> printer</td><td># Tiskárna</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c</strong> camera</td><td># Camera</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c</strong> scanner</td><td># Skener</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device-pos</strong>=<em>NUMBER</em></td> <td>device position start from zero </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--device-name</strong>=<em>NAME</em></td> <td>alternatively specify the name of a device </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Select ICC v2 Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Select ICC v4 Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--skip-x-color-region-target</strong></td> <td>skip X Color Management device profile</td> </tr>
</table>

<h3 id="assign">Assign profile to device</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-a</strong> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em> [<strong>-p</strong><em>[=ICC_FILE_NAME]</em>] [<strong>--system-wide</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--assign</strong></td> <td>add configuration to OpenICC DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--profile-name</strong><em>[=ICC_FILE_NAME]</em></td> <td>jméno profil souboru<br />Can be "" empty string or "automatic" or a real profile name. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--system-wide</strong></td> <td>add computer wide</td> </tr>
</table>

<h3 id="erase">Unassign profile from device</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-e</strong> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em> [<strong>--system-wide</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--erase</strong></td> <td>remove configuration from OpenICC DB</td> </tr>
</table>

<h3 id="setup">Setup device</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-s</strong> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em>

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--setup</strong></td> <td>configure session from OpenICC DB</td> </tr>
</table>

<h3 id="unset">Unset device</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-u</strong> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em>

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--unset</strong></td> <td>reset session configuration to zero</td> </tr>
</table>

<h3 id="list">List device classes</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-l</strong> [<strong>-v</strong>] [<strong>--short</strong>] [<strong>--print-class</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list</strong></td> <td>List device classes</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>upovídaný výstup</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--short</strong></td> <td>print module ID or profile name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--print-class</strong></td> <td>print the modules device class</td> </tr>
</table>

<h3 id="list">List devices</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-l</strong> [<strong>-c</strong>=<em>CLASS</em>] [<strong>-d</strong>=<em>NUMBER</em>|<strong>--device-name</strong>=<em>NAME</em>] [<strong>--short</strong>] [<strong>--path</strong>] [<strong>-r</strong>] [<strong>-v</strong>]

&nbsp;&nbsp;Needs -c option.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list</strong></td> <td>List device classes</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--device-class</strong>=<em>CLASS</em></td> <td>use device class. Useful device classes are monitor, scanner, printer, camera.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-c</strong> monitor</td><td># Monitor</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c</strong> printer</td><td># Tiskárna</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c</strong> camera</td><td># Camera</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c</strong> scanner</td><td># Skener</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--short</strong></td> <td>print module ID or profile name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--path</strong></td> <td>print the full file name</td> </tr>
</table>

<h3 id="list-profiles">List local DB profiles for selected device</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>--list-profiles</strong> [<strong>-c</strong>=<em>CLASS</em>] [<strong>-d</strong>=<em>NUMBER</em>] [<strong>--show-non-device-related</strong>]

&nbsp;&nbsp;Needs -c and -d options.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--list-profiles</strong></td> <td>List local DB profiles for selected device</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--show-non-device-related</strong></td> <td>show as well non matching profiles</td> </tr>
</table>

<h3 id="list-taxi-profiles">List Taxi DB profiles for selected device</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>--list-taxi-profiles</strong> [<strong>-c</strong>=<em>CLASS</em>] [<strong>-d</strong>=<em>NUMBER</em>] [<strong>--show-non-device-related</strong>]

&nbsp;&nbsp;Needs -c and -d options.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--list-taxi-profiles</strong></td> <td>List Taxi DB profiles for selected device</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--show-non-device-related</strong></td> <td>show as well non matching profiles</td> </tr>
</table>

<h3 id="format">Dump device color state</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-f</strong>=<em>icc|openicc+rank-map|openicc|openicc-rank-map</em> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em> <strong>-j</strong>=<em>FILENAME</em> <strong>-k</strong>=<em>FILENAME</em> [<strong>-o</strong>=<em>FILENAME</em>] [<strong>--only-db</strong>] [<strong>-m</strong>]

&nbsp;&nbsp;Needs -c and -d options.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong>=<em>icc|openicc+rank-map|openicc|openicc-rank-map</em></td> <td>dump configuration data
  <table>
   <tr><td style='padding-left:0.5em'><strong>-f</strong> icc</td><td># write assigned ICC profile</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f</strong> fallback-icc</td><td># create fallback ICC profile</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f</strong> openicc+rank-map</td><td># create OpenICC device color state JSON including the rank map</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f</strong> openicc</td><td># create OpenICC device color state JSON</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f</strong> openicc-rank-map</td><td># create OpenICC device color state rank map JSON</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>FILENAME</em></td> <td>write to specified file  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--device-json</strong>=<em>FILENAME</em></td> <td>use device JSON alternatively to -c and -d options </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--rank-json</strong>=<em>FILENAME</em></td> <td>use rank map JSON alternatively to -c and -d options </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--only-db</strong></td> <td>use only DB keys for -f=openicc</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--device-meta-tag</strong></td> <td>embedd device and driver information into ICC meta tag</td> </tr>
</table>

<h3 id="calibration">Convert Calibration Data</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>--calibration</strong>=<em>FILENAME</em> [<strong>-m</strong>] [<strong>-j</strong>=<em>FILENAME</em>] [<strong>-f</strong>=<em>icc|openicc+rank-map|openicc|openicc-rank-map</em>] [<strong>-n</strong>=<em>ICC_FILE_NAME</em>]

&nbsp;&nbsp;Use for VCGT or printer calibrations. The command generates a device link profile. This can be used for insertion into a conversion, which does not use native calibration, like a opt out window with Compiz compicc window color management.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--calibration</strong>=<em>FILENAME</em></td> <td>Generate a device link profile.<br />FILENAME can be output of xcalib -p . </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--device-meta-tag</strong></td> <td>embedd device and driver information into ICC meta tag</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--device-json</strong>=<em>FILENAME</em></td> <td>use device JSON alternatively to -c and -d options </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--new-profile-name</strong>=<em>ICC_FILE_NAME</em></td> <td>jméno profil souboru  </td>
 </tr>
</table>


<h2>GENERAL OPTIONS <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="help">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-h</strong>=<em>synopsis|...</em> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong>=<em>synopsis|...</em></td> <td>Print help text<br />Show usage information and hints for the tool.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Export formated text<br />Get UI converted into text formats
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> man</td><td># Man : Unix Man page - Get a unix man page</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> markdown</td><td># Markdown : Formated text - Get formated text</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json</td><td># Json : GUI - Get a Oyjl Json UI declaration</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json+command</td><td># Json + Command : GUI + Command - Get Oyjl Json UI declaration incuding command</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : All available data - Get UI data for developers. The format can be converted by the oyjl-args tool.</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Select Renderer<br />Select and possibly configure Renderer. -R="gui" will just launch a graphical UI. -R="web:port=port_number:https_key=TLS_private_key_filename:https_cert=TLS_CA_certificate_filename:css=layout_filename.css" will launch a local Web Server, which listens on local port.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> gui</td><td># Gui : Show UI - Display a interactive graphical User Interface.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> cli</td><td># Cli : Show UI - Print on Command Line Interface.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Start Web Server - Start a local Web Service to connect a Webbrowser with. Use the -R=web:help sub option to see more information.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>upovídaný výstup</td> </tr>
</table>


<h2>ENVIRONMENT <a href="#toc" name="environment">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;set the Oyranos debug level. Alternatively the -v option can be used.
#### XDG_DATA_HOME XDG_DATA_DIRS
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended.
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal

<h2>SEE ALSO <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos-profiles](oyranosprofiles.html)<a href="oyranosprofiles.md">(1)</a>&nbsp;&nbsp;[oyranos-profile](oyranosprofile.html)<a href="oyranosprofile.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2021 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Licence
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

