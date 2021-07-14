# oyranos-profile v0.9.7 {#oyranosprofile}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [GENERAL OPTIONS](#general_options) [ENVIRONMENT](#environment) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyranos-profile"</strong> *1* <em>"March 06, 2015"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-profile v0.9.7 - Oyranos Profile

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-profile</strong> [<strong>--path</strong>] [<strong>--short</strong>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#list-tags"><strong>-l</strong></a> [<strong>-p</strong>=<em>NUMBER</em>] [<strong>-n</strong>=<em>NAME</em>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#remove-tag"><strong>-r</strong>=<em>NUMBER</em></a> l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#list-hash"><strong>-m</strong></a> [<strong>-w</strong>=<em>ICC_FILE_NAME</em>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#ppmcie"><strong>--ppmcie</strong></a> [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#output"><strong>-o</strong>=<em>FILENAME</em></a> [<strong>-c</strong>=<em>CLASS</em>] [<strong>-f</strong>=<em>xml...</em>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#profile-name"><strong>-w</strong>=<em>ICC_FILE_NAME</em></a> [<strong>-j</strong>=<em>FILENAME</em>|<strong>-m</strong>] [<strong>-s</strong>=<em>NAME</em>] [<strong>-2</strong>] [<strong>-4</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The oyranos-profile programm shows informations about a ICC profile and allows some modifications.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3>Show ICC Profile</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> [<strong>--path</strong>] [<strong>--short</strong>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em></td> <td>ICC Profile<br />can  be  file  name,  internal  description  string,  ICC profile ID or wildcard "rgb", "cmyk", "gray", "lab", "xyz", "web", "rgbi", "cmyki", "grayi", "labi", "xyzi".  Wildcards ending with "i" are assumed profiles. "web" is a sRGB profile. The other wildcards are editing profiles. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--path</strong></td> <td>show the full ICC profile path and file name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--short</strong></td> <td>show only the ICC profiles file name</td> </tr>
</table>

<h3 id="list-tags">List included ICC tags</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-l</strong> [<strong>-p</strong>=<em>NUMBER</em>] [<strong>-n</strong>=<em>NAME</em>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-tags</strong></td> <td>list contained tags additional to overview and header.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--tag-pos</strong>=<em>NUMBER</em></td> <td>select tag </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--tag-name</strong>=<em>NAME</em></td> <td>select tag </tr>
</table>

<h3 id="remove-tag">Remove included ICC tag</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-r</strong>=<em>NUMBER</em> l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--remove-tag</strong>=<em>NUMBER</em></td> <td><br />remove selected tag number.  </td>
 </tr>
</table>

<h3 id="list-hash">Show Profile ID</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-m</strong> [<strong>-w</strong>=<em>ICC_FILE_NAME</em>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--list-hash</strong></td> <td>show internal hash value.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--profile-name</strong>=<em>ICC_FILE_NAME</em></td> <td>write profile with correct ID hash<br />The -w option specifies the new internal and external profile name. PROFILENAME specifies the source profile.  </td>
 </tr>
</table>

<h3 id="ppmcie">Show CIE*xy chromaticities</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>--ppmcie</strong> [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--ppmcie</strong></td> <td>show CIE*xy chromaticities, if available, for use with ppmcie.</td> </tr>
</table>

<h3 id="output">Dump Device Infos to OpenICC device JSON</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-o</strong>=<em>FILENAME</em> [<strong>-c</strong>=<em>CLASS</em>] [<strong>-f</strong>=<em>xml...</em>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>FILENAME</em></td> <td><br />write device informations to OpenICC JSON.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--device-class</strong>=<em>CLASS</em></td> <td>use device class. Useful device classes are monitor, scanner, printer, camera.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong>=<em>xml...</em></td> <td>use IccXML format  </td>
 </tr>
</table>

<h3 id="profile-name">Write to ICC profile</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-w</strong>=<em>ICC_FILE_NAME</em> [<strong>-j</strong>=<em>FILENAME</em>|<strong>-m</strong>] [<strong>-s</strong>=<em>NAME</em>] [<strong>-2</strong>] [<strong>-4</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--profile-name</strong>=<em>ICC_FILE_NAME</em></td> <td>write profile with correct ID hash<br />The -w option specifies the new internal and external profile name. PROFILENAME specifies the source profile.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--json-name</strong>=<em>FILENAME</em></td> <td>embed OpenICC JSON device from file  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--list-hash</strong></td> <td>show internal hash value.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--name-space</strong>=<em>NAME</em></td> <td>add prefix  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Select ICC v2 Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Select ICC v4 Profiles</td> </tr>
</table>


<h2>GENERAL OPTIONS <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="help">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Print help text<br />Show usage information and hints for the tool.</td> </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Select Renderer<br />Select and possibly configure Renderer. -R="gui" will just launch a graphical UI.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R gui</strong></td><td># Gui</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R cli</strong></td><td># Cli</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R web</strong></td><td># Web</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R -</strong></td><td># </td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>verbose</td> </tr>
</table>


<h2>ENVIRONMENT <a href="#toc" name="environment">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;set the Oyranos debug level. Alternatively the -v option can be used.
#### XDG_DATA_HOME XDG_DATA_DIRS
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended.
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal

<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

#### Show overview and header of profile
&nbsp;&nbsp;oyranos-profile sRGB.icc
#### Show first tags content of profile
&nbsp;&nbsp;oyranos-profile -lv -p=1 sRGB.icc
#### Show the profile hash sum
&nbsp;&nbsp;oyranos-profile -m sRGB.icc
#### Show the RGB primaries of a matrix profile inside a CIE*xy diagram
&nbsp;&nbsp;ppmcie `oyranos-profile --ppmcie sRGB.icc` > sRGB_cie-xy.ppm
#### Add calibration data to meta tag of a device profile
&nbsp;&nbsp;oyranos-profile -w my_profile -j my_device.json my_profile.icc
#### Pass the profile to a external tool
&nbsp;&nbsp;iccdump "`oyranos-profile --path cmyk`"

<h2>SEE ALSO <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-profiles](oyranosprofiles.html)<a href="oyranosprofiles.md">(1)</a>&nbsp;&nbsp;[oyranos-profile-graph](oyranosprofilegraph.html)<a href="oyranosprofilegraph.md">(1)</a>&nbsp;&nbsp;[oyranos-config-fltk](oyranosconfigfltk.html)<a href="oyranosconfigfltk.md">(1)</a>&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>&nbsp;&nbsp;[ppmcie](ppmcie.html)<a href="ppmcie.md">(1)</a>

 http://www.oyranos.org


<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2021 Kai-Uwe Behrmann and others*


<a name="license"></a>
### License
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

