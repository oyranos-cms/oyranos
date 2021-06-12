# oyranos-config v0.9.7 {#oyranosconfigcs}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [GENERAL OPTIONS](#general_options) [ENVIRONMENT VARIABLES](#environmentvariables) [EXAMPLES](#examples) [SEE AS WELL](#seeaswell) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyranos-config"</strong> *1* <em>"September 23, 2020"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-config v0.9.7 - Config

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-config</strong> <a href="#get"><strong>-g</strong>=<em>XPATH</em></a> | <strong>-s</strong>=<em>XPATH:VALUE</em> | <strong>-l</strong> | <strong>--dump-db</strong> | <strong>-p</strong> [<strong>-v</strong>] [<strong>-z</strong>]
<br />
<strong>oyranos-config</strong> <a href="#daemon"><strong>-d</strong>=<em>0|1</em></a> [<strong>-v</strong>]
<br />
<strong>oyranos-config</strong> <a href="#syscolordir"><strong>--syscolordir</strong></a> | <strong>--usercolordir</strong> | <strong>--iccdirname</strong> | <strong>--settingsdirname</strong> | <strong>--cmmdir</strong> | <strong>--metadir</strong> [<strong>-v</strong>] [<strong>-z</strong>]
<br />
<strong>oyranos-config</strong> <a href="#Version"><strong>--Version</strong></a> | <strong>--api-version</strong> | <strong>--num-version</strong> | <strong>--git-version</strong> [<strong>-v</strong>]
<br />
<strong>oyranos-config</strong> <a href="#cflags"><strong>--cflags</strong></a> | <strong>--ldflags</strong> | <strong>--ldstaticflags</strong> | <strong>--sourcedir</strong> | <strong>--builddir</strong> [<strong>-v</strong>]
<br />
 <a href="#synopsis"><strong>oyranos-config</strong></a> <a href="#export"><strong>-X</strong>=<em>json|json+command|man|markdown</em></a> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|web|...</em> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The tool can read and set OpenICC DB options, and display paths and static information.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="get">Persistent Settings</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>-g</strong>=<em>XPATH</em> | <strong>-s</strong>=<em>XPATH:VALUE</em> | <strong>-l</strong> | <strong>--dump-db</strong> | <strong>-p</strong> [<strong>-v</strong>] [<strong>-z</strong>]

&nbsp;&nbsp;Handle OpenICC DB configuration on low level.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--get</strong>=<em>XPATH</em></td> <td>Get a Value </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--set</strong>=<em>XPATH:VALUE</em></td> <td>Set a Value </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list</strong></td> <td>List existing paths inside DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--dump-db</strong></td> <td>Dump OpenICC DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--path</strong></td> <td>Show DB File</td> </tr>
</table>

<h3 id="daemon">Observe config changes</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>-d</strong>=<em>0|1</em> [<strong>-v</strong>]

&nbsp;&nbsp;Will only work on command line.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--daemon</strong>=<em>0|1</em></td> <td>Watch DB changes
  <table>
   <tr><td style='padding-left:0.5em'><strong>-d 0</strong></td><td># Deactivate</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d 1</strong></td><td># Activate</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="syscolordir">Show Install Paths</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>--syscolordir</strong> | <strong>--usercolordir</strong> | <strong>--iccdirname</strong> | <strong>--settingsdirname</strong> | <strong>--cmmdir</strong> | <strong>--metadir</strong> [<strong>-v</strong>] [<strong>-z</strong>]

&nbsp;&nbsp;Show statically configured and compiled in paths of Oyranos CMS.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--syscolordir</strong></td> <td>Path to system main color directory</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--usercolordir</strong></td> <td>Path to users main color directory</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--iccdirname</strong></td> <td>ICC profile directory name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--settingsdirname</strong></td> <td>Oyranos settings directory name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--cmmdir</strong></td> <td>Oyranos CMM directory name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--metadir</strong></td> <td>Oyranos meta module directory name</td> </tr>
</table>

<h3 id="Version">Show Version</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>--Version</strong> | <strong>--api-version</strong> | <strong>--num-version</strong> | <strong>--git-version</strong> [<strong>-v</strong>]

&nbsp;&nbsp;Release Version follow of a Major(API|ABI)-Minor(Feature)-Micro(Patch|Bug Fix) scheme. For orientation in git the last release, commit number, SHA1 ID and Year-month-day parts are available.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--Version</strong></td> <td>Show official version<br />API|ABI-Feature-Patch|BugFix Release</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--api-version</strong></td> <td>Show version of API</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--num-version</strong></td> <td>Show version as a simple number<br />10000*API+100*Feature+Patch</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--git-version</strong></td> <td>Show version as in git<br />lastReleaseVersion-gitCommitNumber-gitCommitSHA1ID-Year-month-day</td> </tr>
</table>

<h3 id="cflags">Miscellaneous options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>--cflags</strong> | <strong>--ldflags</strong> | <strong>--ldstaticflags</strong> | <strong>--sourcedir</strong> | <strong>--builddir</strong> [<strong>-v</strong>]

&nbsp;&nbsp;These strings can be used to compile programs.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--cflags</strong></td> <td>compiler flags</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--ldflags</strong></td> <td>dynamic link flags</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--ldstaticflags</strong></td> <td>static linking flags</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--sourcedir</strong></td> <td>Oyranos local source directory name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--builddir</strong></td> <td>Oyranos local build directory name</td> </tr>
</table>


<h2>GENERAL OPTIONS <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="export">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|web|...</em> [<strong>-v</strong>]

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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|web|...</em></td> <td>Select Renderer<br />Select and possibly configure Renderer. -R="gui" will just launch a graphical UI. -R="port_number:api_path:TLS_private_key:TLS_CA_certificate:style.css" will launch a local Web Server, which listens on local port.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R gui</strong></td><td># Gui</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R web</strong></td><td># Web</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R -</strong></td><td># </td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-z</strong>|<strong>--system-wide</strong></td> <td>System wide DB setting</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>upovídaný výstup</td> </tr>
</table>


<h2>ENVIRONMENT VARIABLES <a href="#toc" name="environmentvariables">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;set the Oyranos debug level.
  <br />
&nbsp;&nbsp;Alternatively the -v option can be used.
  <br />
&nbsp;&nbsp;Valid integer range is from 1-20.
#### OY_MODULE_PATH
&nbsp;&nbsp;route Oyranos to additional directories containing modules.

<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

#### Show a settings value
&nbsp;&nbsp;oyranos-config -g org/freedesktop/openicc/behaviour/effect_switch
#### Change a setting
&nbsp;&nbsp;oyranos-config -s org/freedesktop/openicc/behaviour/effect_switch:1
#### Show all settings with values
&nbsp;&nbsp;oyranos-config -l -v
#### Watch events
&nbsp;&nbsp;oyranos-config -d 1 -v > log-file.txt
#### Compile a simple programm
&nbsp;&nbsp;cc `oyranos-config --cflags` myFile.c `oyranos-config --ldflags` -o myProg
#### Show system wide visible profiles from the Oyranos installation path
&nbsp;&nbsp;ls `oyranos-config --syscolordir --iccdirname`

<h2>SEE AS WELL <a href="#toc" name="seeaswell">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos-config-synnefo](oyranosconfigsynnefo.html)<a href="oyranosconfigsynnefo.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

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

