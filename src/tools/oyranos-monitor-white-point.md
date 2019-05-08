# oyranos\-monitor\-white\-point v0.9.7 {#oyranosmonitorwhitepoint}
*"oyranos\-monitor\-white\-point"* *1* *"October 11, 2018"* "User Commands"
## NAME
oyranos\-monitor\-white\-point v0.9.7 \- Night Manager
## SYNOPSIS
**oyranos\-monitor\-white\-point** \-n *0|1|2|3|4|5|6|7* [\-g *ICC\_PROFILE*] [\-b *PERCENT*] [\-z] [\-v]
<br />
**oyranos\-monitor\-white\-point** \-w *0|1|2|3|4|5|6|7* \-a *KELVIN* [\-z] [\-v]
<br />
**oyranos\-monitor\-white\-point** \-s *0|1|2|3|4|5|6|7* [\-e *ICC\_PROFILE*] [\-z] [\-v]
<br />
**oyranos\-monitor\-white\-point** \-l|  \-i *ANGLE\_IN\_DEGREE* \-o *ANGLE\_IN\_DEGREE* [\-t *ANGLE\_IN\_DEGREE|0:rise/set|\-6:civil|\-12:nautical|\-18:astronomical*] [\-z] [\-v]
<br />
**oyranos\-monitor\-white\-point** \-d *0|1|2* [\-v]
<br />
**oyranos\-monitor\-white\-point** \-m|  \-r|  \-X *json|json+command|man|markdown*|  \-h [\-v]
## DESCRIPTION
The tool can set the actual white point or set it by local day and night time. A additional effect profile can be selected.
## OPTIONS
### Nightly appearance
**oyranos\-monitor\-white\-point** \-n *0|1|2|3|4|5|6|7* [\-g *ICC\_PROFILE*] [\-b *PERCENT*] [\-z] [\-v]

The Night white point mode shall allow to reduce influence of blue light during night time. A white point temperature of around 4000K and lower allows to get easier into sleep and is recommended along with warm room illumination in evening and night times.

* \-n|\-\-night\-white\-point *0|1|2|3|4|5|6|7*	Set night time mode: A white point temperature of around 4000K and lower allows to get easier into sleep. Enable by setting this option to Automatic (\-n=1) and Temperature to 3000 (\-a=3000).
   * \-n 0		# No
   * \-n 1		# Automatic
   * \-n 2		# Illuminant D50
   * \-n 3		# Illuminant D55
   * \-n 4		# Illuminant D65
   * \-n 5		# Illuminant D75
   * \-n 6		# Illuminant D93
* \-g|\-\-night\-effect *ICC\_PROFILE*	Set night time effect: A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT\_linear=yes .
   * \-g \-		# [none]
   * \-g Effect 1		# Example Effect 1
   * \-g Effect 2		# Example Effect 2
* \-b|\-\-night\-backlight *PERCENT*	Set Nightly Backlight: The option needs xbacklight installed and supporting your device for dimming the monitor lamp. (PERCENT:4 [≥0 ≤100])

### Actual mode
**oyranos\-monitor\-white\-point** \-w *0|1|2|3|4|5|6|7* \-a *KELVIN* [\-z] [\-v]

* \-w|\-\-white\-point *0|1|2|3|4|5|6|7*	Set white point mode
   * \-w 0		# No
   * \-w 1		# Automatic
   * \-w 2		# Illuminant D50
   * \-w 3		# Illuminant D55
   * \-w 4		# Illuminant D65
   * \-w 5		# Illuminant D75
   * \-w 6		# Illuminant D93
* \-a|\-\-automatic *KELVIN*	A value from 2700 till 8000 Kelvin is expected to show no artefacts (KELVIN:2800 [≥1100 ≤10100])

### Sun light appearance
**oyranos\-monitor\-white\-point** \-s *0|1|2|3|4|5|6|7* [\-e *ICC\_PROFILE*] [\-z] [\-v]

* \-s|\-\-sun\-white\-point *0|1|2|3|4|5|6|7*	Set day time mode
   * \-s 0		# No
   * \-s 1		# Automatic
   * \-s 2		# Illuminant D50
   * \-s 3		# Illuminant D55
   * \-s 4		# Illuminant D65
   * \-s 5		# Illuminant D75
   * \-s 6		# Illuminant D93
* \-e|\-\-sunlight\-effect *ICC\_PROFILE*	Set day time effect: A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT\_linear=yes .
   * \-e \-		# [none]
   * \-e Effect 1		# Example Effect 1
   * \-e Effect 2		# Example Effect 2

### Location and Twilight
**oyranos\-monitor\-white\-point** \-l|  \-i *ANGLE\_IN\_DEGREE* \-o *ANGLE\_IN\_DEGREE* [\-t *ANGLE\_IN\_DEGREE|0:rise/set|\-6:civil|\-12:nautical|\-18:astronomical*] [\-z] [\-v]

* \-l|\-\-location	Detect location by IP adress
* \-i|\-\-latitude *ANGLE\_IN\_DEGREE*	Set Latitude (ANGLE\_IN\_DEGREE:0 [≥\-90 ≤90])
* \-o|\-\-longitude *ANGLE\_IN\_DEGREE*	Set Longitude (ANGLE\_IN\_DEGREE:0 [≥\-180 ≤180])
* \-t|\-\-twilight *ANGLE\_IN\_DEGREE|0:rise/set|\-6:civil|\-12:nautical|\-18:astronomical*	Set Twilight angle (ANGLE\_IN\_DEGREE|0:rise/set|\-6:civil|\-12:nautical|\-18:astronomical:0 [≥18 ≤\-18])

### Run sunset daemon
**oyranos\-monitor\-white\-point** \-d *0|1|2* [\-v]

* \-d|\-\-daemon *0|1|2*	Control user daemon
   * \-d 0		# Deactivate
   * \-d 1		# Autostart
   * \-d 2		# Activate

### General options
**oyranos\-monitor\-white\-point** \-m|  \-r|  \-X *json|json+command|man|markdown*|  \-h [\-v]

* \-h|\-\-help	Help
* \-m|\-\-modes	Show white point modes
* \-r|\-\-sunrise	Show local time, used geographical location, twilight height angles, sun rise and sun set times
* \-X|\-\-export *json|json+command|man|markdown*	Export formated text: Get UI converted into text formats
   * \-X json		# Json
   * \-X json+command		# Json + Command
   * \-X man		# Man
   * \-X markdown		# Markdown
* \-z|\-\-system\-wide	System wide DB setting
* \-v|\-\-verbose	verbose

## ENVIRONMENT VARIABLES
### OY\_DEBUG
set the Oyranos debug level. Alternatively the \-v option can be used. Valid integer range is from 1\-20.
### OY\_MODULE\_PATH
route Oyranos to additional directories containing modules.  
## EXAMPLES
### Enable the daemon, set night white point to 3000 Kelvin and use that in night mode
oyranos\-monitor\-white\-point \-d 2 \-a 3000 \-n 1 
### Switch all day light intereference off such as white point and effect
oyranos\-monitor\-white\-point \-s 0 \-e 0 
## AUTHOR
Kai\-Uwe Behrmann http://www.oyranos.org
## COPYRIGHT
*Copyright 2018 Kai\-Uwe Behrmann*


### License
newBSD
## BUGS
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

