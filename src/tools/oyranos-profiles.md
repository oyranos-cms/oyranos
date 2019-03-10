# oyranos\-profiles v0.9.7 {#oyranosprofiles}
*"oyranos\-profiles"* *1* *"October 11, 2018"* "User Commands"
## NAME
oyranos\-profiles v0.9.7 \- Oyranos Profiles
## SYNOPSIS
**oyranos\-profiles** \-l [\-f] [\-e] [\-a] [\-c] [\-d] [\-k] [\-n] [\-o] [\-i] [\-2] [\-4] [\-P] [\-T *KEY;VALUE*] [\-v]
<br />
**oyranos\-profiles** \-p [\-u|\-s|\-y|\-m] [\-v]
<br />
**oyranos\-profiles** \-I *ICC\_PROFILE*|  \-t *TAXI\_ID* [\-u|\-s|\-y|\-m] [\-g] [\-v]
<br />
## DESCRIPTION
The tool can list installed profiles, search paths and can help install a ICC color profile in a search path.
## OPTIONS
### List of available ICC color profiles
**oyranos\-profiles** \-l [\-f] [\-e] [\-a] [\-c] [\-d] [\-k] [\-n] [\-o] [\-i] [\-2] [\-4] [\-P] [\-T *KEY;VALUE*] [\-v]

* \-l|\-\-list\-profiles	List Profiles
* \-f|\-\-full\-names	List profile full names: Show path name and file name.
* \-e|\-\-internale\-names	List profile internal names: The text string comes from the 'desc' tag.
* \-a|\-\-abstract	Select Abstract profiles
* \-c|\-\-color\-space	Select Color Space profiles
* \-d|\-\-display	Select Monitor profiles
* \-k|\-\-device\-link	Select Device Link profiles
* \-n|\-\-named\-color	Select Named Color profiles
* \-o|\-\-output	Select Output profiles
* \-i|\-\-input	Select Input profiles
* \-2|\-\-icc\-version\-2	Select ICC v2 Profiles
* \-4|\-\-icc\-version\-4	Select ICC v4 Profiles
* \-P|\-\-path	Show profiles containing a string as part of their full name: PATH\_SUB\_STRING
* \-T|\-\-meta *KEY;VALUE*	Filter for meta tag key/value pair: Show profiles containing a certain key/value pair of their meta tag. VALUE can contain '*' to allow for substring matching.
* \-D|\-\-duplicates	Show identical multiple installed profiles

### List search paths
**oyranos\-profiles** \-p [\-u|\-s|\-y|\-m] [\-v]

* \-p|\-\-list\-paths	List ICC Profile Paths
* \-u|\-\-user	User path
* \-s|\-\-system	System path
* \-y|\-\-oyranos	Oyranos path
* \-m|\-\-machine	Machine path

### Install Profile
**oyranos\-profiles** \-I *ICC\_PROFILE*|  \-t *TAXI\_ID* [\-u|\-s|\-y|\-m] [\-g] [\-v]

* \-I|\-\-install *ICC\_PROFILE*	Install Profile
* \-t|\-\-taxi *TAXI\_ID*	ICC Taxi Profile DB
* \-u|\-\-user	User path
* \-s|\-\-system	System path
* \-y|\-\-oyranos	Oyranos path
* \-m|\-\-machine	Machine path
* \-g|\-\-gui	Use Graphical User Interface

### General options

* \-r|\-\-no\-repair	No Profile repair of ICC profile ID
* \-X|\-\-export *json|json+command|man|markdown*	Export formated text: Get UI converted into text formats
   * \-X json		# Json
   * \-X json+command		# Json + Command
   * \-X man		# Man
   * \-X markdown		# Markdown
* \-v|\-\-verbose	verbose
* \-h|\-\-help	Help

## ENVIRONMENT VARIABLES
### OY\_DEBUG
set the Oyranos debug level. Alternatively the \-v option can be used. Valid integer range is from 1\-20.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal  
## EXAMPLES
### List all installed profiles by internal name
oyranos\-profiles \-le 
### List all installed profiles of the display and output device classes
oyranos\-profiles \-l \-od 
### List all installed profiles in user path
oyranos\-profiles \-lfu 
### Install a profile for the actual user and show error messages in a GUI
oyranos\-profiles \-\-install profilename \-u \-\-gui 
### Install a profile for the actual user and show error messages in a GUI
oyranos\-profiles \-\-install \- \-\-taxi=taxi\_id/0 \-\-gui \-d \-u 
### Show file infos
SAVEIFS=$IFS ; IFS=$'\n\b'; profiles=(\`oyranos\-profiles \-ldf\`); IFS=$SAVEIFS; for file in "${profiles[@]}"; do ls "$file"; done  
## AUTHOR
Kai\-Uwe Behrmann http://www.oyranos.org
## COPYRIGHT
*Copyright 2018 Kai\-Uwe Behrmann*


### License
newBSD
## BUGS
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

