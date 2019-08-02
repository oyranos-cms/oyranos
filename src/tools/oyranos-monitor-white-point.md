# oyranos\-monitor\-white\-point v0.9.7 {#oyranosmonitorwhitepoint}
*"oyranos\-monitor\-white\-point"* *1* *"October 11, 2018"* "User Commands"
## NAME
oyranos\-monitor\-white\-point v0.9.7 \- Night Manager
## SYNOPSIS
**oyranos\-monitor\-white\-point** <strong>\-n</strong> *0|1|2|3|4|5|6|7* <strong>\-g</strong> *ICC\_PROFILE* [<strong>\-b</strong> *PERCENT*] [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-w</strong> *0|1|2|3|4|5|6|7* <strong>\-a</strong> *KELVIN* [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-s</strong> *0|1|2|3|4|5|6|7* <strong>\-e</strong> *ICC\_PROFILE* [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-l</strong> | <strong>\-i</strong> *ANGLE\_IN\_DEGREE* <strong>\-o</strong> *ANGLE\_IN\_DEGREE* [<strong>\-t</strong> *ANGLE\_IN\_DEGREE|0:rise/set|\-6:civil|\-12:nautical|\-18:astronomical*] [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-d</strong> *0|1|2* [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-m</strong> | <strong>\-r</strong> | <strong>\-X</strong> *json|json+command|man|markdown* | <strong>\-h</strong> [<strong>\-v</strong>]
## DESCRIPTION
The tool can set the actual white point or set it by local day and night time. A additional effect profile can be selected.
## OPTIONS
### Nightly appearance
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-n</strong> *0|1|2|3|4|5|6|7* <strong>\-g</strong> *ICC\_PROFILE* [<strong>\-b</strong> *PERCENT*] [<strong>\-z</strong>] [<strong>\-v</strong>]

The Night white point mode shall allow to reduce influence of blue light during night time. A white point temperature of around 4000K and lower allows to get easier into sleep and is recommended along with warm room illumination in evening and night times.

* <strong>\-n</strong>|<strong>\-\-night\-white\-point</strong> *0|1|2|3|4|5|6|7*	Set night time mode: A white point temperature of around 4000K and lower allows to get easier into sleep. Enable by setting this option to Automatic (\-n=1) and Temperature to 3000 (\-a=3000).
   * <strong>\-n 0</strong>		# No
   * <strong>\-n 1</strong>		# Automatic
   * <strong>\-n 2</strong>		# Illuminant D50
   * <strong>\-n 3</strong>		# Illuminant D55
   * <strong>\-n 4</strong>		# Illuminant D65
   * <strong>\-n 5</strong>		# Illuminant D75
   * <strong>\-n 6</strong>		# Illuminant D93
* <strong>\-g</strong>|<strong>\-\-night\-effect</strong> *ICC\_PROFILE*	Set night time effect: A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT\_linear=yes .
   * <strong>\-g \-</strong>		# [none]
   * <strong>\-g Effect 1</strong>		# Example Effect 1
   * <strong>\-g Effect 2</strong>		# Example Effect 2
* <strong>\-b</strong>|<strong>\-\-night\-backlight</strong> *PERCENT*	Set Nightly Backlight: The option needs xbacklight installed and supporting your device for dimming the monitor lamp. (PERCENT:4 [≥0 ≤100])

### Actual mode
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-w</strong> *0|1|2|3|4|5|6|7* <strong>\-a</strong> *KELVIN* [<strong>\-z</strong>] [<strong>\-v</strong>]

* <strong>\-w</strong>|<strong>\-\-white\-point</strong> *0|1|2|3|4|5|6|7*	Set white point mode
   * <strong>\-w 0</strong>		# No
   * <strong>\-w 1</strong>		# Automatic
   * <strong>\-w 2</strong>		# Illuminant D50
   * <strong>\-w 3</strong>		# Illuminant D55
   * <strong>\-w 4</strong>		# Illuminant D65
   * <strong>\-w 5</strong>		# Illuminant D75
   * <strong>\-w 6</strong>		# Illuminant D93
* <strong>\-a</strong>|<strong>\-\-automatic</strong> *KELVIN*	A value from 2700 till 8000 Kelvin is expected to show no artefacts (KELVIN:2800 [≥1100 ≤10100])

### Sun light appearance
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-s</strong> *0|1|2|3|4|5|6|7* <strong>\-e</strong> *ICC\_PROFILE* [<strong>\-z</strong>] [<strong>\-v</strong>]

* <strong>\-s</strong>|<strong>\-\-sun\-white\-point</strong> *0|1|2|3|4|5|6|7*	Set day time mode
   * <strong>\-s 0</strong>		# No
   * <strong>\-s 1</strong>		# Automatic
   * <strong>\-s 2</strong>		# Illuminant D50
   * <strong>\-s 3</strong>		# Illuminant D55
   * <strong>\-s 4</strong>		# Illuminant D65
   * <strong>\-s 5</strong>		# Illuminant D75
   * <strong>\-s 6</strong>		# Illuminant D93
* <strong>\-e</strong>|<strong>\-\-sunlight\-effect</strong> *ICC\_PROFILE*	Set day time effect: A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT\_linear=yes .
   * <strong>\-e \-</strong>		# [none]
   * <strong>\-e Effect 1</strong>		# Example Effect 1
   * <strong>\-e Effect 2</strong>		# Example Effect 2

### Location and Twilight
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-l</strong> | <strong>\-i</strong> *ANGLE\_IN\_DEGREE* <strong>\-o</strong> *ANGLE\_IN\_DEGREE* [<strong>\-t</strong> *ANGLE\_IN\_DEGREE|0:rise/set|\-6:civil|\-12:nautical|\-18:astronomical*] [<strong>\-z</strong>] [<strong>\-v</strong>]

* <strong>\-l</strong>|<strong>\-\-location</strong>	Detect location by IP adress
* <strong>\-i</strong>|<strong>\-\-latitude</strong> *ANGLE\_IN\_DEGREE*	Set Latitude (ANGLE\_IN\_DEGREE:0 [≥\-90 ≤90])
* <strong>\-o</strong>|<strong>\-\-longitude</strong> *ANGLE\_IN\_DEGREE*	Set Longitude (ANGLE\_IN\_DEGREE:0 [≥\-180 ≤180])
* <strong>\-t</strong>|<strong>\-\-twilight</strong> *ANGLE\_IN\_DEGREE|0:rise/set|\-6:civil|\-12:nautical|\-18:astronomical*	Set Twilight angle (ANGLE\_IN\_DEGREE|0:rise/set|\-6:civil|\-12:nautical|\-18:astronomical:0 [≥18 ≤\-18])

### Run sunset daemon
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-d</strong> *0|1|2* [<strong>\-v</strong>]

* <strong>\-d</strong>|<strong>\-\-daemon</strong> *0|1|2*	Control user daemon
   * <strong>\-d 0</strong>		# Deactivate
   * <strong>\-d 1</strong>		# Autostart
   * <strong>\-d 2</strong>		# Activate

### General options
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-m</strong> | <strong>\-r</strong> | <strong>\-X</strong> *json|json+command|man|markdown* | <strong>\-h</strong> [<strong>\-v</strong>]

* <strong>\-h</strong>|<strong>\-\-help</strong>	Help
* <strong>\-m</strong>|<strong>\-\-modes</strong>	Show white point modes
* <strong>\-r</strong>|<strong>\-\-sunrise</strong>	Show local time, used geographical location, twilight height angles, sun rise and sun set times
* <strong>\-X</strong>|<strong>\-\-export</strong> *json|json+command|man|markdown*	Export formated text: Get UI converted into text formats
   * <strong>\-X man</strong>		# Man
   * <strong>\-X markdown</strong>		# Markdown
   * <strong>\-X json</strong>		# Json
   * <strong>\-X json+command</strong>		# Json + Command
   * <strong>\-X export</strong>		# Export
* <strong>\-z</strong>|<strong>\-\-system\-wide</strong>	System wide DB setting
* <strong>\-v</strong>|<strong>\-\-verbose</strong>	verbose

## ENVIRONMENT VARIABLES
### OY\_DEBUG
&nbsp;&nbsp;set the Oyranos debug level.
  <br />
&nbsp;&nbsp;Alternatively the \-v option can be used.
  <br />
&nbsp;&nbsp;Valid integer range is from 1\-20.
### OY\_MODULE\_PATH
&nbsp;&nbsp;route Oyranos to additional directories containing modules.
## EXAMPLES
### Enable the daemon, set night white point to 3000 Kelvin and use that in night mode
&nbsp;&nbsp;oyranos\-monitor\-white\-point \-d 2 \-a 3000 \-n 1
### Switch all day light intereference off such as white point and effect
&nbsp;&nbsp;oyranos\-monitor\-white\-point \-s 0 \-e 0
## AUTHOR
Kai\-Uwe Behrmann http://www.oyranos.org
## COPYRIGHT
*Copyright 2018 Kai\-Uwe Behrmann*


### License
newBSD
## BUGS
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

