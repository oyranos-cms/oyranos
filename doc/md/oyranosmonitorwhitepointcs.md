# oyranos\-monitor\-white\-point v0.9.7 {#oyranosmonitorwhitepointcs}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [GENERAL OPTIONS](#general_options) [ENVIRONMENT VARIABLES](#environmentvariables) [EXAMPLES](#examples) [SEE AS WELL](#seeaswell) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyranos-monitor-white-point"</strong> *1* <em>"October 11, 2018"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-monitor-white-point v0.9.7 - Night Manager

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-monitor-white-point</strong> <a href="#night-white-point"><strong>-n</strong>=<em>0|1|2|3|4|5|6|7</em></a> <strong>-g</strong>=<em>ICC_PROFILE</em> <strong>-b</strong>=<em>PERCENT</em> <strong>--night-color-scheme</strong>=<em>STRING</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#white-point"><strong>-w</strong>=<em>0|1|2|3|4|5|6|7</em></a> <strong>-a</strong>=<em>KELVIN</em> [<strong>-z</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#sun-white-point"><strong>-s</strong>=<em>0|1|2|3|4|5|6|7</em></a> <strong>-e</strong>=<em>ICC_PROFILE</em> <strong>--sunlight-color-scheme</strong>=<em>STRING</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#location"><strong>-l</strong></a> | <strong>-i</strong>=<em>ANGLE_IN_DEGREE</em> <strong>-o</strong>=<em>ANGLE_IN_DEGREE</em> [<strong>-t</strong>=<em>ANGLE_IN_DEGREE</em>] [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#daemon"><strong>-d</strong>=<em>0|1|2</em></a> [<strong>-v</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#modes"><strong>-m</strong></a> | <strong>-r</strong><em>[=FORMAT]</em> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The tool can set the actual white point or set it by local day and night time. A additional effect profile can be selected.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="night-white-point">Nightly appearance</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-n</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-g</strong>=<em>ICC_PROFILE</em> <strong>-b</strong>=<em>PERCENT</em> <strong>--night-color-scheme</strong>=<em>STRING</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]

&nbsp;&nbsp;The Night white point mode shall allow to reduce influence of blue light during night time. A white point temperature of around 4000K and lower allows to get easier into sleep and is recommended along with warm room illumination in evening and night times.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--night-white-point</strong>=<em>0|1|2|3|4|5|6|7</em></td> <td>Set night time mode<br />A white point temperature of around 4000K and lower allows to get easier into sleep. Enable by setting this option to Automatic (-n=1) and Temperature to 3000 (-a=3000).
  <table>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 0</td><td># Ne</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 1</td><td># Automatic</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 2</td><td># Iluminant D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 3</td><td># Iluminant D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 4</td><td># Iluminant D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 5</td><td># Illuminant D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 6</td><td># Iluminant D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--night-effect</strong>=<em>ICC_PROFILE</em></td> <td>Set night time effect<br />A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT_linear=yes .
  <table>
   <tr><td style='padding-left:0.5em'><strong>-g</strong> -</td><td># [žádný]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-g</strong> Effect-1</td><td># Example Effect 1 : ICC profile of class abstract, wich affects gamma</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-g</strong> Effect-2</td><td># Example Effect 2 : ICC profile of class abstract, wich affects gamma</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--night-backlight</strong>=<em>PERCENT</em></td> <td>Set Nightly Backlight: The option needs xbacklight installed and supporting your device for dimming the monitor lamp. (PERCENT:4 [≥0 ≤100 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--night-color-scheme</strong>=<em>STRING</em></td> <td>Set nightly typical darker color scheme<br />Use this to switch color scheme day/night dependent.
  <table>
   <tr><td style='padding-left:0.5em'><strong>--night-color-scheme</strong> -</td><td># [žádný]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>--night-color-scheme</strong> Breeze</td><td># Breeze : Detected Color Scheme</td></tr>
   <tr><td style='padding-left:0.5em'><strong>--night-color-scheme</strong> BreezeDark</td><td># Breeze Dark : 2. detected Color Scheme</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="white-point">Actual mode</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-w</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-a</strong>=<em>KELVIN</em> [<strong>-z</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--white-point</strong>=<em>0|1|2|3|4|5|6|7</em></td> <td>Set white point mode
  <table>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 0</td><td># Ne</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 1</td><td># Automatic</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 2</td><td># Iluminant D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 3</td><td># Iluminant D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 4</td><td># Iluminant D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 5</td><td># Illuminant D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 6</td><td># Iluminant D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--automatic</strong>=<em>KELVIN</em></td> <td>A value from 2700 till 8000 Kelvin is expected to show no artefacts (KELVIN:2800 [≥1100 ≤10100 Δ100])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--test</strong></td> <td></td> </tr>
</table>

<h3 id="sun-white-point">Sun light appearance</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-s</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-e</strong>=<em>ICC_PROFILE</em> <strong>--sunlight-color-scheme</strong>=<em>STRING</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--sun-white-point</strong>=<em>0|1|2|3|4|5|6|7</em></td> <td>Set day time mode
  <table>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 0</td><td># Ne</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 1</td><td># Automatic</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 2</td><td># Iluminant D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 3</td><td># Iluminant D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 4</td><td># Iluminant D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 5</td><td># Illuminant D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 6</td><td># Iluminant D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--sunlight-effect</strong>=<em>ICC_PROFILE</em></td> <td>Set day time effect<br />A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT_linear=yes .
  <table>
   <tr><td style='padding-left:0.5em'><strong>-e</strong> -</td><td># [žádný]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-e</strong> Effect-1</td><td># Example Effect 1 : ICC profile of class abstract, wich affects gamma</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-e</strong> Effect-2</td><td># Example Effect 2 : ICC profile of class abstract, wich affects gamma</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--sunlight-color-scheme</strong>=<em>STRING</em></td> <td>Set day time typical brighter color scheme<br />Use this to switch color scheme day/night dependent.
  <table>
   <tr><td style='padding-left:0.5em'><strong>--sunlight-color-scheme</strong> -</td><td># [žádný]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>--sunlight-color-scheme</strong> Breeze</td><td># Breeze : Detected Color Scheme</td></tr>
   <tr><td style='padding-left:0.5em'><strong>--sunlight-color-scheme</strong> BreezeDark</td><td># Breeze Dark : 2. detected Color Scheme</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="location">Location and Twilight</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-l</strong> | <strong>-i</strong>=<em>ANGLE_IN_DEGREE</em> <strong>-o</strong>=<em>ANGLE_IN_DEGREE</em> [<strong>-t</strong>=<em>ANGLE_IN_DEGREE</em>] [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--location</strong></td> <td>Detect location by IP adress</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--latitude</strong>=<em>ANGLE_IN_DEGREE</em></td> <td>Set Latitude (ANGLE_IN_DEGREE:0 [≥-90 ≤90 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--longitude</strong>=<em>ANGLE_IN_DEGREE</em></td> <td>Set Longitude (ANGLE_IN_DEGREE:0 [≥-180 ≤180 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--twilight</strong>=<em>ANGLE_IN_DEGREE</em></td> <td>Set Twilight angle: 0:sunrise/sunset|-6:civil|-12:nautical|-18:astronomical (ANGLE_IN_DEGREE:0 [≥-18 ≤18 Δ1])</td> </tr>
</table>

<h3 id="daemon">Run sunset daemon</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-d</strong>=<em>0|1|2</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--daemon</strong>=<em>0|1|2</em></td> <td>Control user daemon
  <table>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> 0</td><td>#  : Deactivate</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> 1</td><td># Autostart : Autostart</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> 2</td><td># Activate : Activate</td></tr>
  </table>
  </td>
 </tr>
</table>


<h2>GENERAL OPTIONS <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="modes">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-m</strong> | <strong>-r</strong><em>[=FORMAT]</em> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Print help text<br />Show usage information and hints for the tool.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> -</td><td># Full Help : Print help for all groups</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> synopsis</td><td># Synopsis : List groups - Show all groups including syntax</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--modes</strong></td> <td>Show white point modes</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--sunrise</strong><em>[=FORMAT]</em></td> <td>Show local time, used geographical location, twilight height angles, sun rise and sun set times
  <table>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> TEXT</td><td># TEXT</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> JSON</td><td># JSON</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Export formated text<br />Get UI converted into text formats
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> man</td><td># Man : Unix Man page - Get a unix man page</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> markdown</td><td># Markdown : Formated text - Get formated text</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json</td><td># Json : GUI - Get a Oyjl Json UI declaration</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json+command</td><td># Json + Command : GUI + Command - Get Oyjl Json UI declaration incuding command</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : All available data - Get UI data for developers</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Select Renderer<br />Select and possibly configure Renderer. -R="gui" will just launch a graphical UI. -R="web:port=port_number:https_key=TLS_private_key_filename:https_cert=TLS_CA_certificate_filename:css=layout_filename.css" will launch a local Web Server, which listens on local port.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> gui</td><td># Gui : Show UI - Display a interactive graphical User Interface.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> cli</td><td># Cli : Show UI - Print on Command Line Interface.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Start Web Server - Start a local Web Service to connect a Webbrowser with. Supported subargs are: port for port number, https_key and https_cert for passing in encryption filenames, security=readonly|interactive|lazy with "readonly" showing a static page, "interactive" showing GUI elements and "lazy" executing the tool. The "css=layout.css" lets you style your output by CSS.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-z</strong>|<strong>--system-wide</strong></td> <td>System wide DB setting</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--test</strong></td> <td></td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>Verbose</td> </tr>
</table>


<h2>ENVIRONMENT VARIABLES <a href="#toc" name="environmentvariables">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;set the Oyranos debug level.
  <br />
&nbsp;&nbsp;Alternatively the -v option can be used.
  <br />
&nbsp;&nbsp;Valid integer range is from 1-20.
#### OY_MODULE_PATH
&nbsp;&nbsp;route Oyranos to additional directories containing modules.

<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

#### Enable the daemon, set night white point to 3000 Kelvin and use that in night mode
&nbsp;&nbsp;oyranos-monitor-white-point -d 2 -a 3000 -n 1
#### Switch all day light intereference off such as white point and effect
&nbsp;&nbsp;oyranos-monitor-white-point -s 0 -e 0

<h2>SEE AS WELL <a href="#toc" name="seeaswell">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-monitor](oyranosmonitor.html)<a href="oyranosmonitor.md">(1)</a>&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2021 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Licence
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

