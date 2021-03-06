# oyranos\-profile\-graph v0.9.7 {#oyranosprofilegraphde}
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [UMGEBUNGSVARIABLEN](#environmentvariables) [BEISPIELE](#examples) [SIEHE AUCH](#seeaswell) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

*"oyranos\-profile\-graph"* *1* *"March 24, 2019"* "User Commands"
## NAME <a name="name"></a>
oyranos\-profile\-graph v0.9.7 \- Oyranos Profile Zeichner
## ÜBERSICHT <a name="synopsis"></a>
**oyranos\-profile\-graph** [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong>=<em>NUMMER</em>] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
**oyranos\-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-k</strong>=<em>NUMMER</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-i</strong>=<em>WORT</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-s</strong>=<em>DATEI</em> <strong>\-p</strong>=<em>FORMAT</em> <strong>\-z</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-P</strong>=<em>WORT</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-X</strong>=<em>json|json+command|man|markdown</em> | <strong>\-h</strong> [<strong>\-v</strong>]
## BESCHREIBUNG <a name="description"></a>
Das oyranos\-profile\-graph Programm erzeugt aus einem ICC Profil oder eingebetteten ICC Profil in eine Grafik. Vorgabemäßig malt es die Sättigungslinie eines anzugebenden ICC Profiles und schreibt die Grafik nach stdout.
## OPTIONEN <a name="options"></a>
### 2D Zeichnung vom Farbprofil
&nbsp;&nbsp;**oyranos\-profile\-graph** [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong>=<em>NUMMER</em>] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

Erzeuge aus einem ICC Profil eine Grafik mit seiner Sättigungslinie.

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'> <em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em></td> <td>ICC Profil </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--change-thickness</strong> <em>NUMMER</em></td> <td>Gib die Verstärkung der Liniendicke an (NUMMER:0,7 [≥-1000 ≤1000])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xyy</strong></td> <td>Benutze CIE*xyY *x*y Ebene für Projektion</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--no-blackbody</strong></td> <td>Lasse Weißlinie von Lambertstrahlern fort</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--no-spectral-line</strong></td> <td>Lasse Spektrallinie fort</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Wähle ICC V2 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Wähle ICC V4 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--no-repair</strong></td> <td>Lasse Reparatur der Profil ID weg</td> </tr>
</table>

### Standard Betrachter 1931 2° Zeichnung
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-S</strong>|<strong>--standard-observer</strong></td> <td>CIE Standard Betrachter 1931 2°</td> </tr>
</table>

### 1964 10° Betrachter Zeichnung
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-O</strong>|<strong>--observer-64</strong></td> <td>CIE Betrachter 1064 10°</td> </tr>
</table>

### Spektrumgraph eines Lambertstrahlers
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-k</strong>=<em>NUMMER</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--kelvin</strong> <em>NUMMER</em></td> <td>Lambertstrahler (NUMMER:0 [≥0 ≤25000])</td> </tr>
</table>

### Beleuchtungsspektrumzeichnung
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-i</strong>=<em>WORT</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--illuminant</strong> <em>WORT</em></td> <td>Beleuchtungsspektrum
  <table>
   <tr><td style='padding-left:0.5em'><strong>-i A</strong></td><td># Lichtart A</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D50</strong></td><td># Lichtart D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D55</strong></td><td># Lichtart D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D65</strong></td><td># Lichtart D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D65T</strong></td><td># Lichtart D65 T</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D75</strong></td><td># Lichtart D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-i D93</strong></td><td># Lichtart D93</td></tr>
  </table>
  </td>
 </tr>
</table>

### Spektrumgraph
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-s</strong>=<em>DATEI</em> <strong>\-p</strong>=<em>FORMAT</em> <strong>\-z</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-R</strong>] [<strong>\-P</strong>=<em>WORT</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--spectral</strong> <em>DATEI</em></td> <td>Spektrale Eingabe </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--spectral-format</strong> <em>FORMAT</em></td> <td>Gib spektrales Ausgabeformat an
  <table>
   <tr><td style='padding-left:0.5em'><strong>-p png</strong></td><td># PNG</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p svg</strong></td><td># SVG</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p csv</strong></td><td># CSV</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p ncc</strong></td><td># NCC</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p cgats</strong></td><td># CGATS</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p icc-xml</strong></td><td># Icc XML</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-p ppm</strong></td><td># PPM</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-P</strong>|<strong>--pattern</strong> <em>WORT</em></td> <td>Filter für Farbnamen </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-z</strong>|<strong>--scale</strong></td> <td>Skaliere die Höhe der Spektrumkurve</td> </tr>
</table>

### Allgemeine Optionen
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-X</strong>=<em>json|json+command|man|markdown</em> | <strong>\-h</strong> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--thickness</strong> <em>NUMMER</em></td> <td>Gib die Liniendicke an (NUMMER:1 [≥0 ≤10])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--no-border</strong></td> <td>Lasse Rand aus in Zeichnung</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--no-color</strong></td> <td>Zeichne grau</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--width</strong> <em>NUMMER</em></td> <td>Gib Ausgabebildbreite in Pixel an (NUMMER:128 [≥64 ≤4096])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--raster</strong></td> <td>Zeichne Gitter</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong> <em>-|DATEI</em></td> <td>Gib Ausgabedateiname an, voreingestellt ist stdout </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong> <em>FORMAT</em></td> <td>Gib Ausgabeformat PNG oder SVG an, voreingestellt ist PNG
  <table>
   <tr><td style='padding-left:0.5em'><strong>-f png</strong></td><td># PNG</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f svg</strong></td><td># SVG</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Hilfe</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong> <em>json|json+command|man|markdown</em></td> <td>Exportiere formatierten Text<br />Hole Benutzerschnittstelle als Text
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X man</strong></td><td># Handbuch</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X markdown</strong></td><td># Markdown</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json</strong></td><td># Json</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json+command</strong></td><td># Json + Kommando</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X export</strong></td><td># Export</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>

## UMGEBUNGSVARIABLEN <a name="environmentvariables"></a>
### OY\_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
  <br />
&nbsp;&nbsp;Die -v Option kann alternativ benutzt werden.
  <br />
&nbsp;&nbsp;Der gültige Bereich ist 1\-20.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
## BEISPIELE <a name="examples"></a>
### Zeichne ICC Profil
&nbsp;&nbsp;oyranos\-profile\-graph ICC\_PROFILE
### Zeige Sättigungslinien von zwei Profilen in CIE*ab 256 Bildpunkte breit, ohne Spectrallinie und mit dickeren Linien:
&nbsp;&nbsp;oyranos\-profile\-graph \-w 256 \-s \-t 3 sRGB.icc ProPhoto\-RGB.icc
### Zeige die Standard Betrachter Spektralfunktion als Kurven:
&nbsp;&nbsp;oyranos\-profile\-graph \-\-standard\-observer \-o CIE\-StdObserver.png
## SIEHE AUCH <a name="seeaswell"></a>
###  [oyranos\-profile](oyranosprofile.html)<a href="oyranosprofile.md">(1)</a>  [oyranos\-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>  [oyranos\-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>  [oyranos](oyranos.html)<a href="oyranos.md">(3)</a>
### http://www.oyranos.org
## AUTOR <a name="author"></a>
Kai\-Uwe Behrmann http://www.oyranos.org
## KOPIERRECHT <a name="copyright"></a>
*© 2005\-2019 Kai\-Uwe Behrmann and others*


### Lizenz <a name="license"></a>
newBSD
## FEHLER <a name="bugs"></a>
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

