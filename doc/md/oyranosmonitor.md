# oyranos\-monitor  {#oyranosmonitor}
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [ENVIRONMENT](#environment) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

*"oyranos\-monitor"* *1* *"February 22, 2018"* "User Commands"
## NAME <a name="name"></a>
oyranos\-monitor  \- Oyranos CMS monitor configuration
## SYNOPSIS <a name="synopsis"></a>
**oyranos\-monitor** | <strong>\-s</strong> [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor** [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-\-system\-wide</strong>] [<strong>\-v</strong>] PROFILENAME
<br />
**oyranos\-monitor** <strong>\-e</strong> [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-\-system\-wide</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor** <strong>\-l</strong> [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-\-short</strong>] [<strong>\-\-path</strong>] [<strong>\-c</strong>] [<strong>\-v</strong>] [<strong>\-\-module</strong>=<em>MODULENAME</em>]
<br />
**oyranos\-monitor** <strong>\-\-modules</strong> [<strong>\-v</strong>]
<br />
**oyranos\-monitor** <strong>\-f</strong>=<em>edid|icc|edid\_icc|vcgt</em> [<strong>\-o</strong>=<em>OUT\_FILENAME|default:stdout</em>] [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-m</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor** <strong>\-\-add\-edid</strong>=<em>FILENAME</em> <strong>\-\-profile</strong>=<em>FILENAME</em> [<strong>\-\-name</strong>=<em>NAME</em>] [<strong>\-v</strong>]
## DESCRIPTION <a name="description"></a>
The oyranos\-monitor programm let you query and set the monitor profile(s) within the Oyranos color management system (CMS). Profiles are stored in a data base, the Oyranos DB.
## OPTIONS <a name="options"></a>
&nbsp;&nbsp;**oyranos\-monitor** | <strong>\-s</strong> [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'>|<strong>--</strong></td> <td>No args<br />Run command without arguments</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--setup</strong></td> <td>Setup a ICC profile for a all or a selected monitor [default] Set up the X11 server with the Oyranos DB stored monitor ICC profile(s). This option is optional.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong> <em>num</em></td> <td>POSITION , -x=POSITION select the screen at the x position. by different screens, otherwise it will be ambiguous. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong> <em>num</em></td> <td>POSITION , -y=POSITION select the screen at the y position. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong> <em>NUMBER</em></td> <td>, -d=NUMBER position in device list. "oyranos-monitor -l | wc -l". </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
</table>

&nbsp;&nbsp;**oyranos-monitor** [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-\-system\-wide</strong>] [<strong>\-v</strong>] PROFILENAME

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'> <em>PROFILENAME</em></td> <td>assign a ICC profile to a monitor device<br />Assign a monitor ICC profile to the selected screen and store in the Oyranos DB. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong> <em>num</em></td> <td>POSITION , -x=POSITION select the screen at the x position. by different screens, otherwise it will be ambiguous. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong> <em>num</em></td> <td>POSITION , -y=POSITION select the screen at the y position. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong> <em>NUMBER</em></td> <td>, -d=NUMBER position in device list. "oyranos-monitor -l | wc -l". </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--system-wide</strong></td> <td>do system wide, might need admin or root privileges</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
</table>

&nbsp;&nbsp;**oyranos-monitor** <strong>\-e</strong> [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-\-system\-wide</strong>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--erase</strong></td> <td>release a ICC profile from a monitor device<br />Reset the hardware gamma table to the defaults and erase from the Oyranos DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong> <em>num</em></td> <td>POSITION , -x=POSITION select the screen at the x position. by different screens, otherwise it will be ambiguous. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong> <em>num</em></td> <td>POSITION , -y=POSITION select the screen at the y position. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong> <em>NUMBER</em></td> <td>, -d=NUMBER position in device list. "oyranos-monitor -l | wc -l". </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--system-wide</strong></td> <td>do system wide, might need admin or root privileges</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
</table>

&nbsp;&nbsp;**oyranos-monitor** <strong>\-l</strong> [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-\-short</strong>] [<strong>\-\-path</strong>] [<strong>\-c</strong>] [<strong>\-v</strong>] [<strong>\-\-module</strong>=<em>MODULENAME</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list</strong></td> <td>list devices</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong> <em>num</em></td> <td>POSITION , -x=POSITION select the screen at the x position. by different screens, otherwise it will be ambiguous. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong> <em>num</em></td> <td>POSITION , -y=POSITION select the screen at the y position. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong> <em>NUMBER</em></td> <td>, -d=NUMBER position in device list. "oyranos-monitor -l | wc -l". </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--short</strong></td> <td>show only the ICC profiles file name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--path</strong></td> <td>show the full ICC profile path and file name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--x-color-region-target</strong></td> <td>show the X Color Management (XCM) device profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--module</strong> <em>MODULENAME</em></td> <td>select module </tr>
</table>

&nbsp;&nbsp;**oyranos-monitor** <strong>\-\-modules</strong> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--modules</strong></td> <td>list monitor modules<br />list monitor modules</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
</table>

&nbsp;&nbsp;**oyranos-monitor** <strong>\-f</strong>=<em>edid|icc|edid\_icc|vcgt</em> [<strong>\-o</strong>=<em>OUT\_FILENAME|default:stdout</em>] [<strong>\-x</strong>=<em>num</em>] [<strong>\-y</strong>=<em>num</em>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-m</strong>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong> <em>edid|icc|edid\_icc|vcgt</em></td> <td>[edid|icc|edid_icc|vcgt] Get data<br />Select a data format. Possible are edid for server side EDID data, icc for a server side ICC profile, edid_icc for a ICC profile created from  server  side EDID and vcgt for effect and monitor white point merged into a possibly altered VCGT tag. Without the -o/--ouput option the output is written to stdout. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong> <em>OUT_FILENAME|default:stdout</em></td> <td>OUT_FILENAME Write data selected by -f/--format to the given filename. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong> <em>num</em></td> <td>POSITION , -x=POSITION select the screen at the x position. by different screens, otherwise it will be ambiguous. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong> <em>num</em></td> <td>POSITION , -y=POSITION select the screen at the y position. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong> <em>NUMBER</em></td> <td>, -d=NUMBER position in device list. "oyranos-monitor -l | wc -l". </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong></td> <td>(--device-meta-tag)  switch  is  accepted  by the -f=icc and -f=edid_icc option. It embeddes device and driver informations about the actual device for<br />later easier ICC profile to device assignment.  This becomes useful for sharing ICC profiles.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
</table>

&nbsp;&nbsp;**oyranos-monitor** <strong>\-\-add\-edid</strong>=<em>FILENAME</em> <strong>\-\-profile</strong>=<em>FILENAME</em> [<strong>\-\-name</strong>=<em>NAME</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--add-edid</strong> <em>FILENAME</em></td> <td>EDID_FILENAME<br />Embedd EDID keys to a ICC profile as meta tag. Requires --profile. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--profile</strong> <em>FILENAME</em></td> <td>ICC_FILENAME ICC profile. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--name</strong> <em>NAME</em></td> <td>ICC_INTERNAL_NAME String for ICC profile internal name. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
</table>

## ENVIRONMENT <a name="environment"></a>
### DISPLAY
&nbsp;&nbsp;On X11 systems the display is selected by this variable.
### OY_DEBUG
&nbsp;&nbsp;set the Oyranos debug level. Alternatively the -v option can be used.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended.
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
### OY\_MODULE\_PATH
&nbsp;&nbsp;route Oyranos to additional directories containing modules.
## EXAMPLES <a name="examples"></a>
### Put the following in a setup script like .xinitrc
&nbsp;&nbsp;# select a monitor profile, load the binary blob into X and
  <br />
&nbsp;&nbsp;# fill the VideoCardGammaTable, if appropriate
  <br />
&nbsp;&nbsp;oyranos\-monitor
### Assign a ICC profile to a screen
&nbsp;&nbsp;oyranos\-monitor \-x pos \-y pos profilename
### Reset a screens hardware LUT in order to do a calibration
&nbsp;&nbsp;oyranos\-monitor \-e \-x pos \-y pos profilename
### Query the server side, network transparent profile
&nbsp;&nbsp;oyranos\-monitor \-x pos \-y pos
### List all Oyranos monitor devices
&nbsp;&nbsp;oyranos\-monitor \-l
### Show the first Oyranos monitor device verbosely
&nbsp;&nbsp;oyranos\-monitor \-l \-v \-d 0
### Pass the monitor profile to a external tool
&nbsp;&nbsp;iccDumpProfile \-v "\`oyranos\-monitor \-l \-d 0 \-c \-\-path\`"
### List all monitor devices through the oyX1 module
&nbsp;&nbsp;oyranos\-monitor \-l \-\-module oyX1
### Dump data in the format following the \-f option
&nbsp;&nbsp;oyranos\-monitor \-f=[edid|icc|edid\_icc] \-o=edid.bin \-x=pos \-y=pos \-m
### Embedd device informations into ICC meta tag
&nbsp;&nbsp;oyranos\-monitor \-\-add\-edid=edid\_filename \-\-profile=profilename.icc \-\-name=profilename
## SEE ALSO <a name="seealso"></a>
###  [oyranos\-config\-fltk](oyranosconfigfltk.html)<a href="oyranosconfigfltk.md">(1)</a>  [oyranos\-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>  [oyranos\-profiles](oyranosprofiles.html)<a href="oyranosprofiles.md">(1)</a>  [oyranos](oyranos.html)<a href="oyranos.md">(3)</a>
### http://www.oyranos.org
## AUTHOR <a name="author"></a>
Kai\-Uwe Behrmann (ku.b (at) gmx.de) 
## COPYRIGHT <a name="copyright"></a>
*(c) 2005\-2018, Kai\-Uwe Behrmann and others*


### License <a name="license"></a>
new BSD <http://www.opensource.org/licenses/BSD\-3\-Clause>
## BUGS <a name="bugs"></a>
at: [https://github.com/oyranos\-cms/oyranos/issues](https://github.com/oyranos\-cms/oyranos/issues)

