# oyranos-profiles v0.9.7 {#oyranosprofiles}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [ENVIRONMENT VARIABLES](#environmentvariables) [EXAMPLES](#examples) [SEE AS WELL](#seeaswell) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyranos-profiles"</strong> *1* <em>"October 11, 2018"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-profiles v0.9.7 - Oyranos Profiles

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-profiles</strong> <a href="#list-profiles"><strong>-l</strong></a> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-a</strong>] [<strong>-c</strong>] [<strong>-d</strong>] [<strong>-k</strong>] [<strong>-n</strong>] [<strong>-o</strong>] [<strong>-i</strong>] [<strong>-2</strong>] [<strong>-4</strong>] [<strong>-P</strong>=<em>PATH_SUB_STRING</em>] [<strong>-T</strong>=<em>KEY;VALUE</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-profiles</strong> <a href="#list-paths"><strong>-p</strong></a> [<strong>-u</strong>|<strong>-s</strong>|<strong>-y</strong>|<strong>-m</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-profiles</strong> <a href="#install"><strong>-I</strong>=<em>ICC_PROFILE</em></a> | <strong>-t</strong>=<em>TAXI_ID</em> [<strong>-u</strong>|<strong>-s</strong>|<strong>-y</strong>|<strong>-m</strong>] [<strong>-g</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-profiles</strong> <a href="#help"><strong>-h</strong></a> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong>

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The tool can list installed profiles, search paths and can help install a ICC color profile in a search path.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="list-profiles">List of available ICC color profiles</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profiles</strong></a> <strong>-l</strong> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-a</strong>] [<strong>-c</strong>] [<strong>-d</strong>] [<strong>-k</strong>] [<strong>-n</strong>] [<strong>-o</strong>] [<strong>-i</strong>] [<strong>-2</strong>] [<strong>-4</strong>] [<strong>-P</strong>=<em>PATH_SUB_STRING</em>] [<strong>-T</strong>=<em>KEY;VALUE</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-profiles</strong></td> <td>List Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--full-names</strong></td> <td>List profile full names<br />Show path name and file name.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--internal-names</strong></td> <td>List profile internal names<br />The text string comes from the 'desc' tag.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--abstract</strong></td> <td>Select Abstract profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--color-space</strong></td> <td>Select Color Space profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--display</strong></td> <td>Select Monitor profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--device-link</strong></td> <td>Select Device Link profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--named-color</strong></td> <td>Select Named Color profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong></td> <td>Select Output profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong></td> <td>Select Input profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Select ICC v2 Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Select ICC v4 Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-P</strong>|<strong>--path</strong>=<em>PATH_SUB_STRING</em></td> <td>Show profiles containing a string as part of their full name </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-T</strong>|<strong>--meta</strong>=<em>KEY;VALUE</em></td> <td>Filter for meta tag key/value pair<br />Show profiles containing a certain key/value pair of their meta tag. VALUE can contain '*' to allow for substring matching. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-D</strong>|<strong>--duplicates</strong></td> <td>Show identical multiple installed profiles</td> </tr>
</table>

<h3 id="list-paths">List search paths</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profiles</strong></a> <strong>-p</strong> [<strong>-u</strong>|<strong>-s</strong>|<strong>-y</strong>|<strong>-m</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--list-paths</strong></td> <td>List ICC Profile Paths</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--user</strong></td> <td>User path</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--system</strong></td> <td>System path</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--oyranos</strong></td> <td>Oyranos path</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--machine</strong></td> <td>Machine path</td> </tr>
</table>

<h3 id="install">Install Profile</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profiles</strong></a> <a href="#install"><strong>-I</strong>=<em>ICC_PROFILE</em></a> | <strong>-t</strong>=<em>TAXI_ID</em> [<strong>-u</strong>|<strong>-s</strong>|<strong>-y</strong>|<strong>-m</strong>] [<strong>-g</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-I</strong>|<strong>--install</strong>=<em>ICC_PROFILE</em></td> <td>Install Profile </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--taxi</strong>=<em>TAXI_ID</em></td> <td>ICC Taxi Profile DataBase </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--user</strong></td> <td>User path</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--system</strong></td> <td>System path</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--oyranos</strong></td> <td>Oyranos path</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--machine</strong></td> <td>Machine path</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--gui</strong></td> <td>Use Graphical User Interface</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--test</strong></td> <td>No Action</td> </tr>
</table>

<h3 id="help">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profiles</strong></a> <strong>-h</strong> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong>

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Help</td> </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--no-repair</strong></td> <td>No Profile repair of ICC profile ID</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>verbose</td> </tr>
</table>


<h2>ENVIRONMENT VARIABLES <a href="#toc" name="environmentvariables">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;set the Oyranos debug level.
  <br />
&nbsp;&nbsp;Alternatively the -v option can be used.
  <br />
&nbsp;&nbsp;Valid integer range is from 1-20.
#### XDG_DATA_HOME XDG_DATA_DIRS
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended. http://www.openicc.org/index.php%3Ftitle=OpenIccDirectoryProposal.html

<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

#### List all installed profiles by internal name
&nbsp;&nbsp;oyranos-profiles -le
#### List all installed profiles of the display and output device classes
&nbsp;&nbsp;oyranos-profiles -l -od
#### List all installed profiles in user path
&nbsp;&nbsp;oyranos-profiles -lfu
#### Install a profile for the actual user and show error messages in a GUI
&nbsp;&nbsp;oyranos-profiles --install profilename -u --gui
#### Install a profile for the actual user and show error messages in a GUI
&nbsp;&nbsp;oyranos-profiles --install --taxi=taxi_id/0 --gui -d -u
#### Show file infos
&nbsp;&nbsp;SAVEIFS=$IFS ; IFS=$'\n\b'; profiles=(`oyranos-profiles -ldf`); IFS=$SAVEIFS; for file in "${profiles[@]}"; do ls "$file"; done

<h2>SEE AS WELL <a href="#toc" name="seeaswell">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-profile-graph](oyranosprofilegraph.html)<a href="oyranosprofilegraph.md">(1)</a>&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2020 Kai-Uwe Behrmann and others*


<a name="license"></a>
### License
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

