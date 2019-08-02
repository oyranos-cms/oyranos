# oyranos\-profiles v0.9.7 {#oyranosprofiles}
*"oyranos\-profiles"* *1* *"October 11, 2018"* "User Commands"
## NAME
oyranos\-profiles v0.9.7 \- Oyranos Profiles
## SYNOPSIS
**oyranos\-profiles** <strong>\-l</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-a</strong>] [<strong>\-c</strong>] [<strong>\-d</strong>] [<strong>\-k</strong>] [<strong>\-n</strong>] [<strong>\-o</strong>] [<strong>\-i</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-P</strong>] [<strong>\-T</strong> *KEY;VALUE*] [<strong>\-v</strong>]
<br />
**oyranos\-profiles** <strong>\-p</strong> [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-profiles** <strong>\-I</strong> *ICC\_PROFILE* | <strong>\-t</strong> *TAXI\_ID* [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-g</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-profiles** <strong>\-h</strong> <strong>\-X</strong> *json|json+command|man|markdown*
## DESCRIPTION
The tool can list installed profiles, search paths and can help install a ICC color profile in a search path.
## OPTIONS
### List of available ICC color profiles
&nbsp;&nbsp;**oyranos\-profiles** <strong>\-l</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-a</strong>] [<strong>\-c</strong>] [<strong>\-d</strong>] [<strong>\-k</strong>] [<strong>\-n</strong>] [<strong>\-o</strong>] [<strong>\-i</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-P</strong>] [<strong>\-T</strong> *KEY;VALUE*] [<strong>\-v</strong>]

* <strong>\-l</strong>|<strong>\-\-list\-profiles</strong>	List Profiles
* <strong>\-f</strong>|<strong>\-\-full\-names</strong>	List profile full names: Show path name and file name.
* <strong>\-e</strong>|<strong>\-\-internal\-names</strong>	List profile internal names: The text string comes from the 'desc' tag.
* <strong>\-a</strong>|<strong>\-\-abstract</strong>	Select Abstract profiles
* <strong>\-c</strong>|<strong>\-\-color\-space</strong>	Select Color Space profiles
* <strong>\-d</strong>|<strong>\-\-display</strong>	Select Monitor profiles
* <strong>\-k</strong>|<strong>\-\-device\-link</strong>	Select Device Link profiles
* <strong>\-n</strong>|<strong>\-\-named\-color</strong>	Select Named Color profiles
* <strong>\-o</strong>|<strong>\-\-output</strong>	Select Output profiles
* <strong>\-i</strong>|<strong>\-\-input</strong>	Select Input profiles
* <strong>\-2</strong>|<strong>\-\-icc\-version\-2</strong>	Select ICC v2 Profiles
* <strong>\-4</strong>|<strong>\-\-icc\-version\-4</strong>	Select ICC v4 Profiles
* <strong>\-P</strong>|<strong>\-\-path</strong>	Show profiles containing a string as part of their full name: PATH\_SUB\_STRING
* <strong>\-T</strong>|<strong>\-\-meta</strong> *KEY;VALUE*	Filter for meta tag key/value pair: Show profiles containing a certain key/value pair of their meta tag. VALUE can contain '*' to allow for substring matching.
* <strong>\-D</strong>|<strong>\-\-duplicates</strong>	Show identical multiple installed profiles

### List search paths
&nbsp;&nbsp;**oyranos\-profiles** <strong>\-p</strong> [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-v</strong>]

* <strong>\-p</strong>|<strong>\-\-list\-paths</strong>	List ICC Profile Paths
* <strong>\-u</strong>|<strong>\-\-user</strong>	User path
* <strong>\-s</strong>|<strong>\-\-system</strong>	System path
* <strong>\-y</strong>|<strong>\-\-oyranos</strong>	Oyranos path
* <strong>\-m</strong>|<strong>\-\-machine</strong>	Machine path

### Install Profile
&nbsp;&nbsp;**oyranos\-profiles** <strong>\-I</strong> *ICC\_PROFILE* | <strong>\-t</strong> *TAXI\_ID* [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-g</strong>] [<strong>\-v</strong>]

* <strong>\-I</strong>|<strong>\-\-install</strong> *ICC\_PROFILE*	Install Profile
* <strong>\-t</strong>|<strong>\-\-taxi</strong> *TAXI\_ID*	ICC Taxi Profile DB
* <strong>\-u</strong>|<strong>\-\-user</strong>	User path
* <strong>\-s</strong>|<strong>\-\-system</strong>	System path
* <strong>\-y</strong>|<strong>\-\-oyranos</strong>	Oyranos path
* <strong>\-m</strong>|<strong>\-\-machine</strong>	Machine path
* <strong>\-g</strong>|<strong>\-\-gui</strong>	Use Graphical User Interface

### General options
&nbsp;&nbsp;**oyranos\-profiles** <strong>\-h</strong> <strong>\-X</strong> *json|json+command|man|markdown*

* <strong>\-h</strong>|<strong>\-\-help</strong>	Help
* <strong>\-X</strong>|<strong>\-\-export</strong> *json|json+command|man|markdown*	Export formated text: Get UI converted into text formats
   * <strong>\-X man</strong>		# Man
   * <strong>\-X markdown</strong>		# Markdown
   * <strong>\-X json</strong>		# Json
   * <strong>\-X json+command</strong>		# Json + Command
   * <strong>\-X export</strong>		# Export
* <strong>\-r</strong>|<strong>\-\-no\-repair</strong>	No Profile repair of ICC profile ID
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
### List all installed profiles by internal name
&nbsp;&nbsp;oyranos\-profiles \-le
### List all installed profiles of the display and output device classes
&nbsp;&nbsp;oyranos\-profiles \-l \-od
### List all installed profiles in user path
&nbsp;&nbsp;oyranos\-profiles \-lfu
### Install a profile for the actual user and show error messages in a GUI
&nbsp;&nbsp;oyranos\-profiles \-\-install profilename \-u \-\-gui
### Install a profile for the actual user and show error messages in a GUI
&nbsp;&nbsp;oyranos\-profiles \-\-install \-\-taxi=taxi\_id/0 \-\-gui \-d \-u
### Show file infos
&nbsp;&nbsp;SAVEIFS=$IFS ; IFS=$'\n\b'; profiles=(\`oyranos\-profiles \-ldf\`); IFS=$SAVEIFS; for file in "${profiles[@]}"; do ls "$file"; done
## AUTHOR
Kai\-Uwe Behrmann http://www.oyranos.org
## COPYRIGHT
*Copyright 2018 Kai\-Uwe Behrmann*


### License
newBSD
## BUGS
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

