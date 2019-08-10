# oyranos\-profile  {#oyranosprofile}
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [ENVIRONMENT](#environment) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

*"oyranos\-profile"* *1* *"March 06, 2015"* "User Commands"
## NAME <a name="name"></a>
oyranos\-profile  \- Oyranos CMS ICC profile tool
## SYNOPSIS <a name="synopsis"></a>
**oyranos\-profile** PROFILENAME
<br />
**oyranos\-profile** <strong>\-l</strong>=<em>PROFILENAME</em>
<br />
**oyranos\-profile** <strong>\-r</strong>=<em>NUMBER</em> PROFILENAME
<br />
**oyranos\-profile** <strong>\-m</strong> [<strong>\-w</strong>=<em>NAME</em>] PROFILENAME
<br />
**oyranos\-profile** <strong>\-\-ppmcie</strong>=<em>PROFILENAME</em>
<br />
**oyranos\-profile** <strong>\-o</strong>=<em>FILENAME</em> [<strong>\-c</strong>=<em>CLASS</em>] [<strong>\-f</strong>=<em>xml</em>]
<br />
**oyranos\-profile** <strong>\-w</strong>=<em>NAME</em> [<strong>\-j</strong>=<em>FILENAME</em>] [<strong>\-m</strong>] PROFILENAME
<br />
**oyranos\-profile** [<strong>\-h</strong>]
## DESCRIPTION <a name="description"></a>
The oyranos\-profile programm shows informations about a ICC profile and allows some modifications.
## OPTIONS <a name="options"></a>
&nbsp;&nbsp;**oyranos\-profile** PROFILENAME

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'> <em>PROFILENAME</em></td> <td><br />can  be  file  name,  internal description string, ICC profile ID or wildcard "rgb", "cmyk", "gray", "lab", "xyz", "web", "rgbi", "cmyki", "grayi", "labi", "xyzi".  Wildcards ending with "i" are assumed profiles. "web" is a sRGB profile. The other wildcards are editing profiles. </tr>
</table>

&nbsp;&nbsp;**oyranos-profile** <strong>\-l</strong>=<em>PROFILENAME</em>

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong> <em>PROFILENAME</em></td> <td>list contained tags additional to overview and header. </tr>
</table>

&nbsp;&nbsp;**oyranos-profile** <strong>\-r</strong>=<em>NUMBER</em> PROFILENAME

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong> <em>NUMBER</em></td> <td>FILENAME<br />remove selected tag number. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'> <em>PROFILENAME</em></td> <td><br />can  be  file  name,  internal description string, ICC profile ID or wildcard "rgb", "cmyk", "gray", "lab", "xyz", "web", "rgbi", "cmyki", "grayi", "labi", "xyzi".  Wildcards ending with "i" are assumed profiles. "web" is a sRGB profile. The other wildcards are editing profiles. </tr>
</table>

&nbsp;&nbsp;**oyranos-profile** <strong>\-m</strong> [<strong>\-w</strong>=<em>NAME</em>] PROFILENAME

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong></td> <td>show internal hash value.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'> <em>PROFILENAME</em></td> <td><br />can  be  file  name,  internal description string, ICC profile ID or wildcard "rgb", "cmyk", "gray", "lab", "xyz", "web", "rgbi", "cmyki", "grayi", "labi", "xyzi".  Wildcards ending with "i" are assumed profiles. "web" is a sRGB profile. The other wildcards are editing profiles. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong> <em>NAME</em></td> <td>FILENAME write profile with correct hash PROFILENAME<br />write ICC profile. The -w option specifies the new internal and external profile name. PROFILENAME specifies the source profile. </tr>
</table>

&nbsp;&nbsp;**oyranos-profile** <strong>\-\-ppmcie</strong>=<em>PROFILENAME</em>

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--ppmcie</strong> <em>PROFILENAME</em></td> <td>-o FILENAME<br />show CIE*xy chromaticities, if available, for use with ppmcie. </tr>
</table>

&nbsp;&nbsp;**oyranos-profile** <strong>\-o</strong>=<em>FILENAME</em> [<strong>\-c</strong>=<em>CLASS</em>] [<strong>\-f</strong>=<em>xml</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong> <em>FILENAME</em></td> <td><br />write device informations to OpenICC JSON. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong> <em>CLASS</em></td> <td>NAME use device class. Useful device classes are monitor, scanner, printer, camera. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong> <em>xml</em></td> <td>use IccXML format </tr>
</table>

&nbsp;&nbsp;**oyranos-profile** <strong>\-w</strong>=<em>NAME</em> [<strong>\-j</strong>=<em>FILENAME</em>] [<strong>\-m</strong>] PROFILENAME

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong> <em>NAME</em></td> <td>FILENAME write profile with correct hash PROFILENAME<br />write ICC profile. The -w option specifies the new internal and external profile name. PROFILENAME specifies the source profile. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'> <em>PROFILENAME</em></td> <td><br />can  be  file  name,  internal description string, ICC profile ID or wildcard "rgb", "cmyk", "gray", "lab", "xyz", "web", "rgbi", "cmyki", "grayi", "labi", "xyzi".  Wildcards ending with "i" are assumed profiles. "web" is a sRGB profile. The other wildcards are editing profiles. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong> <em>FILENAME</em></td> <td>embed OpenICC JSON device from file </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong></td> <td>show internal hash value.</td> </tr>
</table>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td><br />print a help text</td> </tr>
</table>

## ENVIRONMENT <a name="environment"></a>
### OY\_DEBUG
&nbsp;&nbsp;set the Oyranos debug level. Alternatively the -v option can be used.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended.
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
## EXAMPLES <a name="examples"></a>
### Show overview and header of profile
&nbsp;&nbsp;oyranos\-profile sRGB.icc
### Show first tags content of profile
&nbsp;&nbsp;oyranos\-profile \-lv \-p=1 sRGB.icc
### Show the profile hash sum
&nbsp;&nbsp;oyranos\-profile \-m sRGB.icc
### Show the RGB primaries of a matrix profile inside a CIE*xy diagram
&nbsp;&nbsp;ppmcie \`oyranos\-profile \-\-ppmcie sRGB.icc\` > sRGB\_cie\-xy.ppm
### Add calibration data to meta tag of a device profile
&nbsp;&nbsp;oyranos\-profile \-w my\_profile \-j my\_device.json my\_profile.icc
### Pass the profile to a external tool
&nbsp;&nbsp;iccdump "\`oyranos\-profile \-\-path cmyk\`"
## SEE ALSO <a name="seealso"></a>
###  [oyranos\-profiles](oyranosprofiles.html)<a href="oyranosprofiles.md">(1)</a>  [oyranos\-profile\-graph](oyranosprofilegraph.html)<a href="oyranosprofilegraph.md">(1)</a>  [oyranos\-config\-fltk](oyranosconfigfltk.html)<a href="oyranosconfigfltk.md">(1)</a>  [oyranos\-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>  [oyranos](oyranos.html)<a href="oyranos.md">(3)</a>  [ppmcie](ppmcie.html)<a href="ppmcie.md">(1)</a>
### http://www.oyranos.org
## AUTHOR <a name="author"></a>
Kai\-Uwe Behrmann (ku.b (at) gmx.de) 
## COPYRIGHT <a name="copyright"></a>
*(c) 2011\-2015, Kai\-Uwe Behrmann and others*


### License <a name="license"></a>
new BSD <http://www.opensource.org/licenses/BSD\-3\-Clause>
## BUGS <a name="bugs"></a>
at: [https://github.com/oyranos\-cms/oyranos/issues](https://github.com/oyranos\-cms/oyranos/issues)

