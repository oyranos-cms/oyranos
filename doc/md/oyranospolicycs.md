# oyranos\-policy v0.9.7 {#oyranospolicycs}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [GENERAL OPTIONS](#general_options) [ENVIRONMENT](#environment) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyranos-policy"</strong> *1* <em>"February 13, 2015"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-policy v0.9.7 - Oyranos CMS policy tool

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-policy</strong> <a href="#current-policy"><strong>-c</strong></a> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#list-policies"><strong>-l</strong></a> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#dump"><strong>-d</strong></a> [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#import-policy"><strong>-i</strong>=<em>FILENAME</em></a> [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#list-paths"><strong>-p</strong></a> [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#save-policy"><strong>-s</strong>=<em>FILENAME</em></a> [<strong>--system-wide</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#long-help"><strong>--long-help</strong></a> | <strong>--docbook</strong> [<strong>--doc-title</strong>=<em>TEXT</em>] [<strong>--doc-version</strong>=<em>TEXT</em>]
<br />
<strong>oyranos-policy</strong> <a href="#export"><strong>-X</strong>=<em>json|json+command|man|markdown</em></a> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The tool dumps out / read a configuration of the Oyranos color management system (CMS).

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="current-policy">Current policy</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-c</strong> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--current-policy</strong></td> <td>Show current active policy name</td> </tr>
</table>

<h3 id="list-policies">List available policies</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-l</strong> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-policies</strong></td> <td>List available policies</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--file-name</strong></td> <td>Show full filename including path.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--internal-name</strong></td> <td>Show display name.</td> </tr>
</table>

<h3 id="dump">Dump out the actual settings</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-d</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--dump</strong></td> <td>Dump out all settings in Oyranos' own xml-ish text format</td> </tr>
</table>

<h3 id="import-policy">Select active policy</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-i</strong>=<em>FILENAME</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--import-policy</strong>=<em>FILENAME</em></td> <td>Must be in Oyranos' own xml-ish text format  </td>
 </tr>
</table>

<h3 id="list-paths">List search paths</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-p</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--list-paths</strong></td> <td>List search paths</td> </tr>
</table>

<h3 id="save-policy">Save to a new policy</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-s</strong>=<em>FILENAME</em> [<strong>--system-wide</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--save-policy</strong>=<em>FILENAME</em></td> <td>Save and Install to a new policy  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--system-wide</strong></td> <td>Do system wide, might need admin or root privileges</td> </tr>
</table>

<h3 id="long-help">Dokumentace</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>--long-help</strong> | <strong>--docbook</strong> [<strong>--doc-title</strong>=<em>TEXT</em>] [<strong>--doc-version</strong>=<em>TEXT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--long-help</strong></td> <td>Generate Oyranos Documentation<br />HTML Format</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--docbook</strong></td> <td>Generate Oyranos Documentation<br />Docbook Format</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--doc-title</strong>=<em>TEXT</em></td> <td>  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--doc-version</strong>=<em>TEXT</em></td> <td>  </td>
 </tr>
</table>


<h2>GENERAL OPTIONS <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="export">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Print help text<br />Show usage information and hints for the tool.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h -</strong></td><td># Full Help</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h synopsis</strong></td><td># Synopsis</td></tr>
  </table>
  </td>
 </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Select Renderer<br />Select and possibly configure Renderer. -R="gui" will just launch a graphical UI.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R gui</strong></td><td># Gui</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R cli</strong></td><td># Cli</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R web</strong></td><td># Web</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R -</strong></td><td># </td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>upovídaný výstup</td> </tr>
</table>


<h2>ENVIRONMENT <a href="#toc" name="environment">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;set the Oyranos debug level.
#### XDG_CONFIG_HOME
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for policies have a "color/settings" appended.
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal

<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

#### Dump out the actual settings
&nbsp;&nbsp;oyranos-policy -d
#### Set new policy
&nbsp;&nbsp;oyranos-policy -i policy_filename
#### List available policies
&nbsp;&nbsp;oyranos-policy -l
#### Currently active policy including its file name
&nbsp;&nbsp;oyranos-policy -cfe
#### Save and Install to a new policy
&nbsp;&nbsp;oyranos-policy -s policy_name
#### Print a help text
&nbsp;&nbsp;oyranos-policy -h

<h2>SEE ALSO <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(3)</a>&nbsp;&nbsp;[oyranos-monitor](oyranosmonitor.html)<a href="oyranosmonitor.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2021 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Licence
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

