# oyranos\-profile\-graph v0.9.7 {#oyranosprofilegraph}
*"oyranos\-profile\-graph"* *1* *"March 24, 2019"* "User Commands"
## NAME
oyranos\-profile\-graph v0.9.7 \- Oyranos Profile Graph
## SYNOPSIS
**oyranos\-profile\-graph** [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
**oyranos\-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-k</strong>=<em>NUMBER</em> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-i</strong>=<em>STRING</em> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-s</strong>=<em>FILE</em> <strong>\-p</strong>=<em>FORMAT</em> <strong>\-z</strong> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-P</strong>=<em>STRING</em>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-X</strong>=<em>json|json+command|man|markdown</em> | <strong>\-h</strong> [<strong>\-v</strong>]
## DESCRIPTION
The  oyranos\-profile\-graph programm converts ICC profiles or embedded ICC profiles from images to a graph image. By default the program shows the saturation line of the specified profiles and writes to stdout.
## OPTIONS
### 2D Graph from profiles
&nbsp;&nbsp;**oyranos\-profile\-graph** [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong>=<em>NUMBER</em>] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

Create a 2D Graph containing the saturation line from a ICC Profile.

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'> <em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em></td> <td>ICC Profile </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--change-thickness</strong> <em>NUMBER</em></td> <td>Specify increase of the thickness of the graph lines (NUMBER:0.7 [≥-1000 ≤1000])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xyy</strong></td> <td>Use CIE*xyY *x*y plane for saturation line projection</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--no-blackbody</strong></td> <td>Omit white line of lambert light emitters</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--no-spectral-line</strong></td> <td>Omit the spectral line</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Select ICC v2 Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Select ICC v4 Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--no-repair</strong></td> <td>No Profile repair of ICC profile ID</td> </tr>
</table>

### Standard Observer 1931 2° Graph
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-S</strong>|<strong>--standard-observer</strong></td> <td>CIE Standard Observer 1931 2°</td> </tr>
</table>

### 1964 10° Observer Graph
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-O</strong>|<strong>--observer-64</strong></td> <td>CIE Observer 1964 10°</td> </tr>
</table>

### Blackbody Radiator Spectrum Graph
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-k</strong>=<em>NUMBER</em> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--kelvin</strong> <em>NUMBER</em></td> <td>Blackbody Radiator (NUMBER:0 [≥0 ≤25000])</td> </tr>
</table>

### Illuminant Spectrum Graph
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-i</strong>=<em>STRING</em> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--illuminant</strong> <em>STRING</em></td> <td>Illuminant Spectrum
  <table>
   <tr><td style='padding-left:0.5em'><strong>-i A</strong></td><td># Illuminant A</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D50</strong></td><td># Illuminant D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D55</strong></td><td># Illuminant D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D65</strong></td><td># Illuminant D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D65T</strong></td><td># Illuminant D65 T</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D75</strong></td><td># Illuminant D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D93</strong></td><td># Illuminant D93</td></tr>
  </table>
  </td>
 </tr>
</table>

### Spectral Input Graph
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-s</strong>=<em>FILE</em> <strong>\-p</strong>=<em>FORMAT</em> <strong>\-z</strong> [<strong>\-t</strong>=<em>NUMBER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMBER</em>] [<strong>\-R</strong>] [<strong>\-P</strong>=<em>STRING</em>] [<strong>\-o</strong>=<em>\-|FILE</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--spectral</strong> <em>FILE</em></td> <td>Spectral Input </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--spectral-format</strong> <em>FORMAT</em></td> <td>Specify spectral output file format
  <table>
   <tr><td style='padding-left:0.5em'><strong>-p png</strong></td><td># PNG</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p svg</strong></td><td># SVG</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p csv</strong></td><td># CSV</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p ncc</strong></td><td># NCC</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p cgats</strong></td><td># CGATS</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p icc-xml</strong></td><td># Icc XML</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p ppm</strong></td><td># PPM</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-P</strong>|<strong>--pattern</strong> <em>STRING</em></td> <td>Filter of Color Names </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-z</strong>|<strong>--scale</strong></td> <td>Scale the height of the spectrum graph</td> </tr>
</table>

### General options
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-X</strong>=<em>json|json+command|man|markdown</em> | <strong>\-h</strong> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--thickness</strong> <em>NUMBER</em></td> <td>Specify the thickness of the graph lines (NUMBER:1 [≥0 ≤10])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--no-border</strong></td> <td>Omit border in graph</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--no-color</strong></td> <td>Draw Gray</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--width</strong> <em>NUMBER</em></td> <td>Specify output image width in pixel (NUMBER:128 [≥64 ≤4096])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--raster</strong></td> <td>Draw Raster</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong> <em>-|FILE</em></td> <td>Specify output file name, default is stdout </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong> <em>FORMAT</em></td> <td>Specify output file format png or svg, default is png
  <table>
   <tr><td style='padding-left:0.5em'><strong>-f png</strong></td><td># PNG</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f svg</strong></td><td># SVG</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Help</td> </tr>
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
</table>

## ENVIRONMENT VARIABLES
### OY\_DEBUG
&nbsp;&nbsp;set the Oyranos debug level.
  <br />
&nbsp;&nbsp;Alternatively the -v option can be used.
  <br />
&nbsp;&nbsp;Valid integer range is from 1\-20.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
## EXAMPLES
### Show graph of a ICC profile
&nbsp;&nbsp;oyranos\-profile\-graph ICC\_PROFILE
### Show the saturation lines of two profiles in CIE*ab 256 pixel width, without spectral line and with thicker lines:
&nbsp;&nbsp;oyranos\-profile\-graph \-w 256 \-s \-t 3 sRGB.icc ProPhoto\-RGB.icc
### Show the standard observer spectral function as curves:
&nbsp;&nbsp;oyranos\-profile\-graph \-\-standard\-observer \-o CIE\-StdObserver.png
## SEE AS WELL
### oyranos\-profile(1) oyranos\-config(1) oyranos\-policy(1) oyranos(3)
### http://www.oyranos.org
## AUTHOR
Kai\-Uwe Behrmann http://www.oyranos.org
## COPYRIGHT
*© 2005\-2019 Kai\-Uwe Behrmann and others*


### License
newBSD
## BUGS
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

