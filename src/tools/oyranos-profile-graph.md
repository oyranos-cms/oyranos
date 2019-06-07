# oyranos\-profile\-graph v0.9.7 {#oyranosprofilegraph}
*"oyranos\-profile\-graph"* *1* *"March 24, 2019"* "User Commands"
## NAME
oyranos\-profile\-graph v0.9.7 \- Oyranos Profile Graph
## SYNOPSIS
**oyranos\-profile\-graph** [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-o *\-|FILE*] [\-f *FORMAT*] [\-c] [\-x] [\-d *NUMBER*] [\-n] [\-2] [\-4] [\-r] [\-v] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
**oyranos\-profile\-graph** \-S [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-o *\-|FILE*] [\-f *FORMAT*] [\-v]
<br />
**oyranos\-profile\-graph** \-O [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-o *\-|FILE*] [\-f *FORMAT*] [\-v]
<br />
**oyranos\-profile\-graph** \-k *NUMBER* [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-o *\-|FILE*] [\-f *FORMAT*] [\-v]
<br />
**oyranos\-profile\-graph** \-i *STRING* [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-o *\-|FILE*] [\-f *FORMAT*] [\-v]
<br />
**oyranos\-profile\-graph** \-s *FILE* \-p *FORMAT* \-z [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-P *STRING*] [\-o *\-|FILE*] [\-v]
<br />
**oyranos\-profile\-graph** \-X *json|json+command|man|markdown*|  \-h [\-v]
## DESCRIPTION
The  oyranos\-profile\-graph programm converts ICC profiles or embedded ICC profiles from images to a graph image. By default the program shows the saturation line of the specified profiles and writes to stdout.
## OPTIONS
### 2D Graph from profiles
**oyranos\-profile\-graph** [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-o *\-|FILE*] [\-f *FORMAT*] [\-c] [\-x] [\-d *NUMBER*] [\-n] [\-2] [\-4] [\-r] [\-v] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

Create a 2D Graph containing the saturation line from a ICC Profile.

*  *l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE*	ICC Profile
* \-d|\-\-change\-thickness *NUMBER*	Specify increase of the thickness of the graph lines (NUMBER:0.7 [≥\-1000 ≤1000])
* \-x|\-\-xyy	Use CIE*xyY *x*y plane for saturation line projection
* \-c|\-\-no\-blackbody	Omit white line of lambert light emitters
* \-n|\-\-no\-spectral\-line	Omit the spectral line
* \-2|\-\-icc\-version\-2	Select ICC v2 Profiles
* \-4|\-\-icc\-version\-4	Select ICC v4 Profiles
* \-r|\-\-no\-repair	No Profile repair of ICC profile ID

### Standard Observer 1931 2° Graph
**oyranos\-profile\-graph** \-S [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-o *\-|FILE*] [\-f *FORMAT*] [\-v]

* \-S|\-\-standard\-observer	CIE Standard Observer 1931 2°

### 1964 10° Observer Graph
**oyranos\-profile\-graph** \-O [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-o *\-|FILE*] [\-f *FORMAT*] [\-v]

* \-O|\-\-observer\-64	CIE Observer 1964 10°

### Blackbody Radiator Spectrum Graph
**oyranos\-profile\-graph** \-k *NUMBER* [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-o *\-|FILE*] [\-f *FORMAT*] [\-v]

* \-k|\-\-kelvin *NUMBER*	Blackbody Radiator (NUMBER:0 [≥0 ≤25000])

### Illuminant Spectrum Graph
**oyranos\-profile\-graph** \-i *STRING* [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-o *\-|FILE*] [\-f *FORMAT*] [\-v]

* \-i|\-\-illuminant *STRING*	Illuminant Spectrum
   * \-i A		# Illuminant A
   * \-i D50		# Illuminant D50
   * \-i D55		# Illuminant D55
   * \-i D65		# Illuminant D65
   * \-i D65T		# Illuminant D65 T
   * \-i D75		# Illuminant D75
   * \-i D93		# Illuminant D93

### Spectral Input Graph
**oyranos\-profile\-graph** \-s *FILE* \-p *FORMAT* \-z [\-t *NUMBER*] [\-b] [\-g] [\-w *NUMBER*] [\-R] [\-P *STRING*] [\-o *\-|FILE*] [\-v]

* \-s|\-\-spectral *FILE*	Spectral Input
* \-p|\-\-spectral\-format *FORMAT*	Specify spectral output file format
   * \-p png		# PNG
   * \-p svg		# SVG
   * \-p csv		# CSV
   * \-p ncc		# NCC
   * \-p cgats		# CGATS
   * \-p icc\-xml		# Icc XML
   * \-p ppm		# PPM
* \-P|\-\-pattern *STRING*	Filter of Color Names
* \-z|\-\-scale	Scale the height of the spectrum graph

### General options
**oyranos\-profile\-graph** \-X *json|json+command|man|markdown*|  \-h [\-v]

* \-t|\-\-thickness *NUMBER*	Specify the thickness of the graph lines (NUMBER:1 [≥0 ≤10])
* \-b|\-\-no\-border	Omit border in graph
* \-g|\-\-no\-color	Draw Gray
* \-w|\-\-width *NUMBER*	Specify output image width in pixel (NUMBER:128 [≥64 ≤4096])
* \-R|\-\-raster	Draw Raster
* \-o|\-\-output *\-|FILE*	Specify output file name, default is stdout
* \-f|\-\-format *FORMAT*	Specify output file format png or svg, default is png
   * \-f png		# PNG
   * \-f svg		# SVG
* \-h|\-\-help	Help
* \-X|\-\-export *json|json+command|man|markdown*	Export formated text: Get UI converted into text formats
   * \-X json		# Json
   * \-X json+command		# Json + Command
   * \-X man		# Man
   * \-X markdown		# Markdown
* \-v|\-\-verbose	verbose

## ENVIRONMENT VARIABLES
### OY\_DEBUG
set the Oyranos debug level. Alternatively the \-v option can be used. Valid integer range is from 1\-20.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal  
## EXAMPLES
### Show graph of a ICC profile
oyranos\-profile\-graph ICC\_PROFILE 
### Show the saturation lines of two profiles in CIE*ab 256 pixel width, without spectral line and with thicker lines:
oyranos\-profile\-graph \-w 256 \-s \-t 3 sRGB.icc ProPhoto\-RGB.icc 
### Show the standard observer spectral function as curves:
oyranos\-profile\-graph \-\-standard\-observer \-o CIE\-StdObserver.png 
## AUTHOR
Kai\-Uwe Behrmann http://www.oyranos.org
## COPYRIGHT
*Copyright 2018 Kai\-Uwe Behrmann*


### License
newBSD
## BUGS
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

