# oyranos\-monitor\-white\-point v0.9.7 {#oyranosmonitorwhitepoint}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [GENERAL OPTIONS](#general_options) [ENVIRONMENT VARIABLES](#environmentvariables) [EXAMPLES](#examples) [SEE AS WELL](#seeaswell) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyranos-monitor-white-point"</strong> *1* <em>"October 11, 2018"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-monitor-white-point v0.9.7 - Night Manager

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-monitor-white-point</strong> <a href="#night-white-point"><strong>-n</strong>=<em>0|1|2|3|4|5|6|7</em></a> <strong>-g</strong>=<em>ICC_PROFILE</em> <strong>-b</strong>=<em>PERCENT</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#white-point"><strong>-w</strong>=<em>0|1|2|3|4|5|6|7</em></a> <strong>-a</strong>=<em>KELVIN</em> [<strong>-z</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#sun-white-point"><strong>-s</strong>=<em>0|1|2|3|4|5|6|7</em></a> <strong>-e</strong>=<em>ICC_PROFILE</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#location"><strong>-l</strong></a> | <strong>-i</strong> <strong>-o</strong> [<strong>-t</strong>] [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#daemon"><strong>-d</strong>=<em>0|1|2</em></a> [<strong>-v</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#modes"><strong>-m</strong></a> | <strong>-r</strong> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The tool can set the actual white point or set it by local day and night time. A additional effect profile can be selected.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="night-white-point">Nightly appearance</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-n</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-g</strong>=<em>ICC_PROFILE</em> <strong>-b</strong>=<em>PERCENT</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]

&nbsp;&nbsp;The Night white point mode shall allow to reduce influence of blue light during night time. A white point temperature of around 4000K and lower allows to get easier into sleep and is recommended along with warm room illumination in evening and night times.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--night-white-point</strong>=<em>0|1|2|3|4|5|6|7</em></td> <td>Set night time mode<br />A white point temperature of around 4000K and lower allows to get easier into sleep. Enable by setting this option to Automatic (-n=1) and Temperature to 3000 (-a=3000).
  <table>
   <tr><td style='padding-left:0.5em'><strong>-n 0</strong></td><td># No</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 1</strong></td><td># Automatic</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 2</strong></td><td># Illuminant D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 3</strong></td><td># Illuminant D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 4</strong></td><td># Illuminant D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 5</strong></td><td># Illuminant D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 6</strong></td><td># Illuminant D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--night-effect</strong>=<em>ICC_PROFILE</em></td> <td>Set night time effect<br />A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT_linear=yes .
  <table>
   <tr><td style='padding-left:0.5em'><strong>-g -</strong></td><td># [none]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-g Effect-1</strong></td><td># Example Effect 1</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-g Effect-2</strong></td><td># Example Effect 2</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--night-backlight</strong>=<em>PERCENT</em></td> <td>Set Nightly Backlight: The option needs xbacklight installed and supporting your device for dimming the monitor lamp. (PERCENT:4 [≥0 ≤100 Δ1])</td> </tr>
</table>

<h3 id="white-point">Actual mode</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-w</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-a</strong>=<em>KELVIN</em> [<strong>-z</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--white-point</strong>=<em>0|1|2|3|4|5|6|7</em></td> <td>Set white point mode
  <table>
   <tr><td style='padding-left:0.5em'><strong>-w 0</strong></td><td># No</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 1</strong></td><td># Automatic</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 2</strong></td><td># Illuminant D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 3</strong></td><td># Illuminant D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 4</strong></td><td># Illuminant D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 5</strong></td><td># Illuminant D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 6</strong></td><td># Illuminant D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--automatic</strong>=<em>KELVIN</em></td> <td>A value from 2700 till 8000 Kelvin is expected to show no artefacts (KELVIN:2800 [≥1100 ≤10100 Δ100])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--test</strong></td> <td></td> </tr>
</table>

<h3 id="sun-white-point">Sun light appearance</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-s</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-e</strong>=<em>ICC_PROFILE</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--sun-white-point</strong>=<em>0|1|2|3|4|5|6|7</em></td> <td>Set day time mode
  <table>
   <tr><td style='padding-left:0.5em'><strong>-s 0</strong></td><td># No</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 1</strong></td><td># Automatic</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 2</strong></td><td># Illuminant D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 3</strong></td><td># Illuminant D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 4</strong></td><td># Illuminant D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 5</strong></td><td># Illuminant D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 6</strong></td><td># Illuminant D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--sunlight-effect</strong>=<em>ICC_PROFILE</em></td> <td>Set day time effect<br />A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT_linear=yes .
  <table>
   <tr><td style='padding-left:0.5em'><strong>-e -</strong></td><td># [none]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-e Effect-1</strong></td><td># Example Effect 1</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-e Effect-2</strong></td><td># Example Effect 2</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="location">Location and Twilight</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-l</strong> | <strong>-i</strong> <strong>-o</strong> [<strong>-t</strong>] [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--location</strong></td> <td>Detect location by IP adress</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--latitude</strong>=<em>ANGLE_IN_DEGREE</em></td> <td>Set Latitude (ANGLE_IN_DEGREE:0 [≥-90 ≤90 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--longitude</strong>=<em>ANGLE_IN_DEGREE</em></td> <td>Set Longitude (ANGLE_IN_DEGREE:0 [≥-180 ≤180 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--twilight</strong>=<em>ANGLE_IN_DEGREE</em></td> <td>Set Twilight angle: 0:sunrise/sunset|-6:civil|-12:nautical|-18:astronomical (ANGLE_IN_DEGREE:0 [≥18 ≤-18 Δ1])</td> </tr>
</table>

<h3 id="daemon">Run sunset daemon</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-d</strong>=<em>0|1|2</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--daemon</strong>=<em>0|1|2</em></td> <td>Control user daemon
  <table>
   <tr><td style='padding-left:0.5em'><strong>-d 0</strong></td><td># Deactivate</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d 1</strong></td><td># Autostart</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d 2</strong></td><td># Activate</td></tr>
  </table>
  </td>
 </tr>
</table>


<h2>GENERAL OPTIONS <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="modes">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-m</strong> | <strong>-r</strong> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Print help text<br />Show usage information and hints for the tool.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--modes</strong></td> <td>Show white point modes</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--sunrise</strong></td> <td>Show local time, used geographical location, twilight height angles, sun rise and sun set times</td> </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Select Renderer<br />Select and possibly configure Renderer. -R="gui" will just launch a graphical UI.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R gui</strong></td><td># Gui</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R cli</strong></td><td># Cli</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R web</strong></td><td># Web</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R -</strong></td><td># </td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-z</strong>|<strong>--system-wide</strong></td> <td>System wide DB setting</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--test</strong></td> <td></td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>verbose</td> </tr>
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
### License
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

