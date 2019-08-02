# oyranos\-profile\-graph v0.9.7 {#oyranosprofilegraph}
*"oyranos\-profile\-graph"* *1* *"March 24, 2019"* "User Commands"
## NAME
oyranos\-profile\-graph v0.9.7 \- Oyranos Profile Graph
## SYNOPSIS
**oyranos\-profile\-graph** [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong> *NUMBER*] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
**oyranos\-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-k</strong> *NUMBER* [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-i</strong> *STRING* [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-s</strong> *FILE* <strong>\-p</strong> *FORMAT* <strong>\-z</strong> [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-P</strong> *STRING*] [<strong>\-o</strong> *\-|FILE*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-X</strong> *json|json+command|man|markdown* | <strong>\-h</strong> [<strong>\-v</strong>]
## DESCRIPTION
The  oyranos\-profile\-graph programm converts ICC profiles or embedded ICC profiles from images to a graph image. By default the program shows the saturation line of the specified profiles and writes to stdout.
## OPTIONS
### 2D Graph from profiles
&nbsp;&nbsp;**oyranos\-profile\-graph** [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong> *NUMBER*] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

Create a 2D Graph containing the saturation line from a ICC Profile.

*  *l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE*	ICC Profile
* <strong>\-d</strong>|<strong>\-\-change\-thickness</strong> *NUMBER*	Specify increase of the thickness of the graph lines (NUMBER:0.7 [≥\-1000 ≤1000])
* <strong>\-x</strong>|<strong>\-\-xyy</strong>	Use CIE*xyY *x*y plane for saturation line projection
* <strong>\-c</strong>|<strong>\-\-no\-blackbody</strong>	Omit white line of lambert light emitters
* <strong>\-n</strong>|<strong>\-\-no\-spectral\-line</strong>	Omit the spectral line
* <strong>\-2</strong>|<strong>\-\-icc\-version\-2</strong>	Select ICC v2 Profiles
* <strong>\-4</strong>|<strong>\-\-icc\-version\-4</strong>	Select ICC v4 Profiles
* <strong>\-r</strong>|<strong>\-\-no\-repair</strong>	No Profile repair of ICC profile ID

### Standard Observer 1931 2° Graph
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]

* <strong>\-S</strong>|<strong>\-\-standard\-observer</strong>	CIE Standard Observer 1931 2°

### 1964 10° Observer Graph
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]

* <strong>\-O</strong>|<strong>\-\-observer\-64</strong>	CIE Observer 1964 10°

### Blackbody Radiator Spectrum Graph
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-k</strong> *NUMBER* [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]

* <strong>\-k</strong>|<strong>\-\-kelvin</strong> *NUMBER*	Blackbody Radiator (NUMBER:0 [≥0 ≤25000])

### Illuminant Spectrum Graph
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-i</strong> *STRING* [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|FILE*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]

* <strong>\-i</strong>|<strong>\-\-illuminant</strong> *STRING*	Illuminant Spectrum
   * <strong>\-i A</strong>		# Illuminant A
   * <strong>\-i D50</strong>		# Illuminant D50
   * <strong>\-i D55</strong>		# Illuminant D55
   * <strong>\-i D65</strong>		# Illuminant D65
   * <strong>\-i D65T</strong>		# Illuminant D65 T
   * <strong>\-i D75</strong>		# Illuminant D75
   * <strong>\-i D93</strong>		# Illuminant D93

### Spectral Input Graph
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-s</strong> *FILE* <strong>\-p</strong> *FORMAT* <strong>\-z</strong> [<strong>\-t</strong> *NUMBER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMBER*] [<strong>\-R</strong>] [<strong>\-P</strong> *STRING*] [<strong>\-o</strong> *\-|FILE*] [<strong>\-v</strong>]

* <strong>\-s</strong>|<strong>\-\-spectral</strong> *FILE*	Spectral Input
* <strong>\-p</strong>|<strong>\-\-spectral\-format</strong> *FORMAT*	Specify spectral output file format
   * <strong>\-p png</strong>		# PNG
   * <strong>\-p svg</strong>		# SVG
   * <strong>\-p csv</strong>		# CSV
   * <strong>\-p ncc</strong>		# NCC
   * <strong>\-p cgats</strong>		# CGATS
   * <strong>\-p icc\-xml</strong>		# Icc XML
   * <strong>\-p ppm</strong>		# PPM
* <strong>\-P</strong>|<strong>\-\-pattern</strong> *STRING*	Filter of Color Names
* <strong>\-z</strong>|<strong>\-\-scale</strong>	Scale the height of the spectrum graph

### General options
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-X</strong> *json|json+command|man|markdown* | <strong>\-h</strong> [<strong>\-v</strong>]

* <strong>\-t</strong>|<strong>\-\-thickness</strong> *NUMBER*	Specify the thickness of the graph lines (NUMBER:1 [≥0 ≤10])
* <strong>\-b</strong>|<strong>\-\-no\-border</strong>	Omit border in graph
* <strong>\-g</strong>|<strong>\-\-no\-color</strong>	Draw Gray
* <strong>\-w</strong>|<strong>\-\-width</strong> *NUMBER*	Specify output image width in pixel (NUMBER:128 [≥64 ≤4096])
* <strong>\-R</strong>|<strong>\-\-raster</strong>	Draw Raster
* <strong>\-o</strong>|<strong>\-\-output</strong> *\-|FILE*	Specify output file name, default is stdout
* <strong>\-f</strong>|<strong>\-\-format</strong> *FORMAT*	Specify output file format png or svg, default is png
   * <strong>\-f png</strong>		# PNG
   * <strong>\-f svg</strong>		# SVG
* <strong>\-h</strong>|<strong>\-\-help</strong>	Help
* <strong>\-X</strong>|<strong>\-\-export</strong> *json|json+command|man|markdown*	Export formated text: Get UI converted into text formats
   * <strong>\-X man</strong>		# Man
   * <strong>\-X markdown</strong>		# Markdown
   * <strong>\-X json</strong>		# Json
   * <strong>\-X json+command</strong>		# Json + Command
   * <strong>\-X export</strong>		# Export
* <strong>\-v</strong>|<strong>\-\-verbose</strong>	verbose

## ENVIRONMENT VARIABLES
### OY\_DEBUG
&nbsp;&nbsp;set the Oyranos debug level.
  <br />
&nbsp;&nbsp;Alternatively the \-v option can be used.
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
## AUTHOR
Kai\-Uwe Behrmann http://www.oyranos.org
## COPYRIGHT
*Copyright 2018 Kai\-Uwe Behrmann*


### License
newBSD
## BUGS
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

