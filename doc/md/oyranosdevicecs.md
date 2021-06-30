# oyranos\-device v0.9.7 {#oyranosdevicecs}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [GENERAL OPTIONS](#general_options) [ENVIRONMENT](#environment) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyranos-device"</strong> *1* <em>"May 13, 2021"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-device v0.9.7 - Oyranos Color Devices

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-device</strong> <a href="#assign"><strong>-a</strong></a> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em> <strong>-p</strong>=<em>ICC_FILE_NAME</em> [<strong>--system-wide</strong>]
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
 <a href="#synopsis"><strong>oyranos-device</strong></a> <a href="#help"><strong>-h</strong></a> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|web|...</em> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The oyranos-device program shows and administrates ICC profiles for color devices.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3>Set basic parameters</h3>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--device-class</strong>=<em>CLASS</em></td> <td>use device class. Useful device classes are monitor, scanner, printer, camera.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-c monitor</strong></td><td># Monitor</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c printer</strong></td><td># Tiskárna</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c camera</strong></td><td># Camera</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c scanner</strong></td><td># Skener</td></tr>
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

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-a</strong> <strong>-c</strong>=<em>CLASS</em> <strong>-d</strong>=<em>NUMBER</em> <strong>-p</strong>=<em>ICC_FILE_NAME</em> [<strong>--system-wide</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--assign</strong></td> <td>add configuration to OpenICC DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--profile-name</strong>=<em>ICC_FILE_NAME</em></td> <td>profile file name<br />Can be "" empty string or "automatic" or a real profile name. </tr>
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
   <tr><td style='padding-left:0.5em'><strong>-c monitor</strong></td><td># Monitor</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c printer</strong></td><td># Tiskárna</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c camera</strong></td><td># Camera</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c scanner</strong></td><td># Skener</td></tr>
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
   <tr><td style='padding-left:0.5em'><strong>-f icc</strong></td><td># write assigned ICC profile</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f fallback-icc</strong></td><td># create fallback ICC profile</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f openicc+rank-map</strong></td><td># create OpenICC device color state JSON including the rank map</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f openicc</strong></td><td># create OpenICC device color state JSON</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f openicc-rank-map</strong></td><td># create OpenICC device color state rank map JSON</td></tr>
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


<h2>GENERAL OPTIONS <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="help">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-h</strong> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Help</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Export formated text<br />Get UI converted into text formats
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X man</strong></td><td># Man</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X markdown</strong></td><td># Markdown</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json</strong></td><td># Json</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json+command</strong></td><td># Json + Command</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X export</strong></td><td># Export</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|web|...</em></td> <td>Select Renderer<br />Select and possibly configure Renderer. -R="gui" will just launch a graphical UI. -R="port_number:api_path:TLS_private_key:TLS_CA_certificate:style.css" will launch a local Web Server, which listens on local port.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R gui</strong></td><td># Gui</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R web</strong></td><td># Web</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R -</strong></td><td># </td></tr>
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

*© 2005-2020 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Licence
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

