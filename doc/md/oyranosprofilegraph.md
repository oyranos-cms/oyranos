# oyranos\-profile\-graph v0.9.7 {#oyranosprofilegraph}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [GENERAL OPTIONS](#general_options) [ENVIRONMENT VARIABLES](#environmentvariables) [EXAMPLES](#examples) [SEE AS WELL](#seeaswell) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyranos-profile-graph"</strong> *1* <em>"June 2, 2020"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-profile-graph v0.9.7 - Oyranos Profile Graph

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-profile-graph</strong> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-c</strong>] [<strong>-x</strong>] [<strong>-d</strong>=<em>NUMBER</em>] [<strong>-n</strong>] [<strong>-2</strong>] [<strong>-4</strong>] [<strong>-r</strong>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE ...
<br />
<strong>oyranos-profile-graph</strong> <a href="#hue"><strong>-H</strong>=<em>NUMBER</em></a> [<strong>-L</strong>=<em>NUMBER</em>] [<strong>-C</strong>=<em>NUMBER</em>] [<em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em> ...] [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-m</strong>=<em>FORMAT</em>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-p</strong>=<em>FORMAT</em>] [<strong>-2</strong>] [<strong>-4</strong>] [<strong>-r</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-profile-graph</strong> <a href="#standard-observer"><strong>-S</strong></a> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-profile-graph</strong> <a href="#observer-64"><strong>-O</strong></a> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-profile-graph</strong> <a href="#kelvin"><strong>-k</strong>=<em>NUMBER</em></a> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-profile-graph</strong> <a href="#illuminant"><strong>-u</strong>=<em>STRING</em></a> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-profile-graph</strong> <a href="#spectral"><strong>-s</strong>=<em>FILE</em></a> <strong>-p</strong>=<em>FORMAT</em> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-P</strong>=<em>STRING</em>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-v</strong>] [<strong>-z</strong>]
<br />
<strong>oyranos-profile-graph</strong> <a href="#import"><strong>-i</strong>=<em>FILE</em></a> [<strong>-I</strong>=<em>PAGE</em>] [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-profile-graph</strong> <a href="#export"><strong>-X</strong>=<em>json|json+command|man|markdown</em></a> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The  oyranos-profile-graph programm converts ICC profiles or embedded ICC profiles from images to a graph image. By default the program shows the saturation line of the specified profiles and writes to stdout.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3>2D Graph from profiles</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile-graph</strong></a> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-c</strong>] [<strong>-x</strong>] [<strong>-d</strong>=<em>NUMBER</em>] [<strong>-n</strong>] [<strong>-2</strong>] [<strong>-4</strong>] [<strong>-r</strong>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE ...

&nbsp;&nbsp;Create a 2D Graph containing the saturation line from a ICC Profile.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em> ...</td> <td>ICC Profile </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--change-thickness</strong>=<em>NUMBER</em></td> <td>Specify increase of the thickness of the graph lines (NUMBER:0.7 [≥0 ≤2 Δ0.05])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xyy</strong></td> <td>Use CIE*xyY *x*y plane for saturation line projection</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--no-blackbody</strong></td> <td>Omit white line of lambert light emitters</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--no-spectral-line</strong></td> <td>Omit the spectral line</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Select ICC v2 Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Select ICC v4 Profiles</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--no-repair</strong></td> <td>No Profile repair of ICC profile ID</td> </tr>
</table>

<h3 id="hue">HLC Color Atlas Pages</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile-graph</strong></a> <strong>-H</strong>=<em>NUMBER</em> [<strong>-L</strong>=<em>NUMBER</em>] [<strong>-C</strong>=<em>NUMBER</em>] [<em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em> ...] [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-m</strong>=<em>FORMAT</em>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-p</strong>=<em>FORMAT</em>] [<strong>-2</strong>] [<strong>-4</strong>] [<strong>-r</strong>] [<strong>-v</strong>]

&nbsp;&nbsp;Create a 2D Graph containing the possible color patches inside the ICC Profile gamut. More information about HLC Color Atlas can be found on www.freiefarbe.de

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-H</strong>|<strong>--hue</strong>=<em>NUMBER</em></td> <td>HLC Color Atlas: Select a page by hue color angle in the HLC Color Atlas. -H=365 will output all hues. (NUMBER:0 [≥0 ≤365 Δ5])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-L</strong>|<strong>--lightness</strong>=<em>NUMBER</em></td> <td>HLC Color Atlas: Select a page by lightness in the HLC Color Atlas. (NUMBER:-5 [≥-5 ≤100 Δ5])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-C</strong>|<strong>--chroma</strong>=<em>NUMBER</em></td> <td>HLC Color Atlas: Select a page by chroma (saturation) in the HLC Color Atlas. (NUMBER:-5 [≥-5 ≤130 Δ5])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em> ...</td> <td>ICC Profile </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--swatch-format</strong>=<em>FORMAT</em></td> <td>Specify output file format
  <table>
   <tr><td style='padding-left:0.5em'><strong>-m</strong> png</td><td># PNG - PNG Raster</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-m</strong> svg</td><td># SVG - SVG Vector</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-m</strong> ncc</td><td># NCC - Named Color Collection</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="standard-observer">Standard Observer 1931 2° Graph</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile-graph</strong></a> <strong>-S</strong> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-S</strong>|<strong>--standard-observer</strong></td> <td>CIE Standard Observer 1931 2°</td> </tr>
</table>

<h3 id="observer-64">1964 10° Observer Graph</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile-graph</strong></a> <strong>-O</strong> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-O</strong>|<strong>--observer-64</strong></td> <td>CIE Observer 1964 10°</td> </tr>
</table>

<h3 id="kelvin">Blackbody Radiator Spectrum Graph</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile-graph</strong></a> <strong>-k</strong>=<em>NUMBER</em> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--kelvin</strong>=<em>NUMBER</em></td> <td>Blackbody Radiator (NUMBER:0 [≥0 ≤25000 Δ100])</td> </tr>
</table>

<h3 id="illuminant">Illuminant Spectrum Graph</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile-graph</strong></a> <strong>-u</strong>=<em>STRING</em> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--illuminant</strong>=<em>STRING</em></td> <td>Illuminant Spectrum
  <table>
   <tr><td style='padding-left:0.5em'><strong>-u</strong> A</td><td># Illuminant A - CIE A spectral power distribution</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u</strong> D50</td><td># Illuminant D50 - CIE D50 spectral power distribution (computed)</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u</strong> D55</td><td># Illuminant D55 - CIE D55 spectral power distribution (computed)</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u</strong> D65</td><td># Illuminant D65 - CIE D65 spectral power distribution (computed)</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u</strong> D65T</td><td># Illuminant D65 T - CIE D65 spectral power distribution</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u</strong> D75</td><td># Illuminant D75 - CIE D75 spectral power distribution (computed)</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u</strong> D93</td><td># Illuminant D93 - CIE D93 spectral power distribution (computed)</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="spectral">Spectral Input Graph</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile-graph</strong></a> <strong>-s</strong>=<em>FILE</em> <strong>-p</strong>=<em>FORMAT</em> [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-P</strong>=<em>STRING</em>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-v</strong>] [<strong>-z</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--spectral</strong>=<em>FILE</em></td> <td>Spectral Input </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--spectral-format</strong>=<em>FORMAT</em></td> <td>Specify spectral output file format
  <table>
   <tr><td style='padding-left:0.5em'><strong>-p</strong> png</td><td># PNG - PNG Raster</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p</strong> svg</td><td># SVG - SVG Vector</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p</strong> csv</td><td># CSV - CSV Values</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p</strong> ncc</td><td># NCC - Named Color Collection</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p</strong> cgats</td><td># CGATS - CGATS Values</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p</strong> icc-xml</td><td># Icc XML - ICC Named Color Values</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p</strong> ppm</td><td># PPM - Spectral PAM Image</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-P</strong>|<strong>--pattern</strong>=<em>STRING</em></td> <td>Filter of Color Names  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-z</strong>|<strong>--scale</strong></td> <td>Scale the height of the spectrum graph</td> </tr>
</table>

<h3 id="import">Render Color Page</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile-graph</strong></a> <strong>-i</strong>=<em>FILE</em> [<strong>-I</strong>=<em>PAGE</em>] [<strong>-t</strong>=<em>NUMBER</em>] [<strong>-b</strong>] [<strong>-l</strong>=<em>NUMBER</em>] [<strong>-g</strong>] [<strong>-w</strong>=<em>NUMBER</em>] [<strong>-T</strong>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-o</strong>=<em>-|FILE</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--import</strong>=<em>FILE</em></td> <td>Color Page Input<br />Supported is a color page in NCC format, which contains pages layout with referenced rgb values. Those are placed on a sheed. Such pages are created by e.g. oyranos-profile-graph --hlc=NUMBER -f ncc </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-I</strong>|<strong>--index</strong>=<em>PAGE</em></td> <td>Page Selection<br />Specify a page name as string or page index as number. -1 will list all page names of the imported file. </tr>
</table>


<h2>GENERAL OPTIONS <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="export">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile-graph</strong></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--thickness</strong>=<em>NUMBER</em></td> <td>Specify the thickness of the graph lines (NUMBER:1 [≥0 ≤10 Δ0.05])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--no-border</strong></td> <td>Omit border in graph</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--background-lightness</strong>=<em>NUMBER</em></td> <td>Background Lightness (NUMBER:-1 [≥-1 ≤100 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--no-color</strong></td> <td>Draw Gray</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--width</strong>=<em>NUMBER</em></td> <td>Specify output image width in pixel (NUMBER:128 [≥64 ≤4096 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-T</strong>|<strong>--raster</strong></td> <td>Draw Raster</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>-|FILE</em></td> <td>Specify output file name, default is stdout  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong>=<em>FORMAT</em></td> <td>Specify output file format png or svg, default is png
  <table>
   <tr><td style='padding-left:0.5em'><strong>-f</strong> png</td><td># PNG - PNG Raster</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f</strong> svg</td><td># SVG - SVG Vector</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Print help text<br />Show usage information and hints for the tool.</td> </tr>
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
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Start Web Server - Start a local Web Service to connect a Webbrowser with. Use the -R=web:help sub option to see more information.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
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

#### Show graph of a ICC profile
&nbsp;&nbsp;oyranos-profile-graph ICC_PROFILE
#### Show the saturation lines of two profiles in CIE*ab 256 pixel width, without spectral line and with thicker lines:
&nbsp;&nbsp;oyranos-profile-graph -w 256 -n -t 3 sRGB.icc ProPhoto-RGB.icc
#### Show HLC Color Atlas patches
&nbsp;&nbsp;oyranos-profile-graph -H=90 -o HLC_H090.png cmyk web
  <br />
&nbsp;&nbsp;Color patches are only shown, if they are in gamut of the default CMYK and web profile.
#### Show the standard observer spectral function as curves:
&nbsp;&nbsp;oyranos-profile-graph --standard-observer -o CIE-StdObserver.png

<h2>SEE AS WELL <a href="#toc" name="seeaswell">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-profile](oyranosprofile.html)<a href="oyranosprofile.md">(1)</a>&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2021 Kai-Uwe Behrmann and others*


<a name="license"></a>
### License
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

