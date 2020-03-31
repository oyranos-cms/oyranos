# oyranos\-profile\-graph v0.9.7 {#oyranosprofilegraphde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [UMGEBUNGSVARIABLEN](#environmentvariables) [BEISPIELE](#examples) [SIEHE AUCH](#seeaswell) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

*"oyranos\-profile\-graph"* *1* *"March 24, 2019"* "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos\-profile\-graph v0.9.7 \- Oyranos Profile Zeichner

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

**oyranos\-profile\-graph** [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong>=<em>NUMMER</em>] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
**oyranos\-profile\-graph** <strong>\-H</strong>=<em>NUMMER</em> [ <em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em>] [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-m</strong>=<em>FORMAT</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-p</strong>=<em>FORMAT</em>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-k</strong>=<em>NUMMER</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-u</strong>=<em>WORT</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-s</strong>=<em>DATEI</em> <strong>\-p</strong>=<em>FORMAT</em> <strong>\-z</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-P</strong>=<em>WORT</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-i</strong>=<em>DATEI</em> <strong>\-I</strong>=<em>SEITE</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-X</strong>=<em>json|json+command|man|markdown</em> | <strong>\-h</strong> [<strong>\-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das oyranos\-profile\-graph Programm erzeugt aus einem ICC Profil oder eingebetteten ICC Profil in eine Grafik. Vorgabemäßig malt es die Sättigungslinie eines anzugebenden ICC Profiles und schreibt die Grafik nach stdout.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

#### 2D Zeichnung vom Farbprofil
&nbsp;&nbsp;**oyranos\-profile\-graph** [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong>=<em>NUMMER</em>] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

&nbsp;&nbsp;Erzeuge aus einem ICC Profil eine Grafik mit seiner Sättigungslinie.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'> <em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em></td> <td>ICC Profil </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--change-thickness</strong> <em>NUMMER</em></td> <td>Gib die Verstärkung der Liniendicke an (NUMMER:0,7 [≥-1000 ≤1000])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xyy</strong></td> <td>Benutze CIE*xyY *x*y Ebene für Projektion</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--no-blackbody</strong></td> <td>Lasse Weißlinie von Lambertstrahlern fort</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--no-spectral-line</strong></td> <td>Lasse Spektrallinie fort</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Wähle ICC V2 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Wähle ICC V4 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--no-repair</strong></td> <td>Lasse Reparatur der Profil ID weg</td> </tr>
</table>

#### Farbpalette vom Farbprofil
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-H</strong>=<em>NUMMER</em> [ <em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em>] [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-m</strong>=<em>FORMAT</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-p</strong>=<em>FORMAT</em>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>]

&nbsp;&nbsp;Erzeuge ein 2D Farbpalette mit allen möglichen Farben innerhalb des Farbumfanges des ICC Fabprofiles.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-H</strong>|<strong>--hlc</strong> <em>NUMMER</em></td> <td>HLC Farbatlas: Wähle ein Seite des HLC Farbatlas mit dem Farbton. -H=365 wählt all Farben. (NUMMER:0 [≥0 ≤365])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'> <em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em></td> <td>ICC Profil </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--swatch-format</strong> <em>FORMAT</em></td> <td>Gib Ausgabeformat an
  <table>
   <tr><td style='padding-left:0.5em'><strong>-m png</strong></td><td># PNG</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-m svg</strong></td><td># SVG</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-m ncc</strong></td><td># NCC</td></tr>
  </table>
  </td>
 </tr>
</table>

#### Standard Betrachter 1931 2° Zeichnung
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-S</strong>|<strong>--standard-observer</strong></td> <td>CIE Standard Betrachter 1931 2°</td> </tr>
</table>

#### 1964 10° Betrachter Zeichnung
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-O</strong>|<strong>--observer-64</strong></td> <td>CIE Betrachter 1064 10°</td> </tr>
</table>

#### Spektrumgraph eines Lambertstrahlers
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-k</strong>=<em>NUMMER</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--kelvin</strong> <em>NUMMER</em></td> <td>Lambertstrahler (NUMMER:0 [≥0 ≤25000])</td> </tr>
</table>

#### Beleuchtungsspektrumzeichnung
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-u</strong>=<em>WORT</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--illuminant</strong> <em>WORT</em></td> <td>Beleuchtungsspektrum
  <table>
   <tr><td style='padding-left:0.5em'><strong>-u A</strong></td><td># Lichtart A</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u D50</strong></td><td># Lichtart D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u D55</strong></td><td># Lichtart D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u D65</strong></td><td># Lichtart D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u D65T</strong></td><td># Lichtart D65 T</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u D75</strong></td><td># Lichtart D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-u D93</strong></td><td># Lichtart D93</td></tr>
  </table>
  </td>
 </tr>
</table>

#### Spektrumgraph
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-s</strong>=<em>DATEI</em> <strong>\-p</strong>=<em>FORMAT</em> <strong>\-z</strong> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-P</strong>=<em>WORT</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--spectral</strong> <em>DATEI</em></td> <td>Spektrale Eingabe </tr>
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

#### Zeige Farbseite
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-i</strong>=<em>DATEI</em> <strong>\-I</strong>=<em>SEITE</em> [<strong>\-t</strong>=<em>NUMMER</em>] [<strong>\-b</strong>] [<strong>\-l</strong>=<em>NUMMER</em>] [<strong>\-g</strong>] [<strong>\-w</strong>=<em>NUMMER</em>] [<strong>\-T</strong>] [<strong>\-f</strong>=<em>FORMAT</em>] [<strong>\-o</strong>=<em>\-|DATEI</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--import</strong> <em>DATEI</em></td> <td>Farbseiten Eingabe<br />Unterstützt werden Farbseiten im NCC Format, welche ein Seiten layout mit referenzierten rgb Werten besitzen. Diese werden auf eine Seite platziert. Solche Seiten können erzeugt werden z.B. mit oyranos-profile-graph --hlc=NUMMER -f ncc </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-I</strong>|<strong>--index</strong> <em>SEITE</em></td> <td>Seitenauswahl<br />Gib einen Seitenname als Wort oder eine Seitennummer an. -1 listet alle Seitennamen der importierten Datei an. </tr>
</table>

#### Allgemeine Optionen
&nbsp;&nbsp;**oyranos-profile\-graph** <strong>\-X</strong>=<em>json|json+command|man|markdown</em> | <strong>\-h</strong> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--thickness</strong> <em>NUMMER</em></td> <td>Gib die Liniendicke an (NUMMER:1 [≥0 ≤10])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--no-border</strong></td> <td>Lasse Rand aus in Zeichnung</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--lightness</strong> <em>NUMMER</em></td> <td>Hintergrund Helligkeit (NUMMER:-1 [≥-1 ≤100])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--no-color</strong></td> <td>Zeichne grau</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--width</strong> <em>NUMMER</em></td> <td>Gib Ausgabebildbreite in Pixel an (NUMMER:128 [≥64 ≤4096])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-T</strong>|<strong>--raster</strong></td> <td>Zeichne Gitter</td> </tr>
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


<h2>UMGEBUNGSVARIABLEN <a href="#toc" name="environmentvariables">&uarr;</a></h2>

#### OY\_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
  <br />
&nbsp;&nbsp;Die -v Option kann alternativ benutzt werden.
  <br />
&nbsp;&nbsp;Der gültige Bereich ist 1\-20.
#### XDG\_DATA\_HOME XDG\_DATA\_DIRS
&nbsp;&nbsp;richte Oyranos auf obere Verzeichnisse, welche Resourcen enthalten. An die Pfade für ICC Farbprofile wird "color/icc" angehangen. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal

<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Zeichne ICC Profil
&nbsp;&nbsp;oyranos\-profile\-graph ICC\_PROFILE
#### Zeige Sättigungslinien von zwei Profilen in CIE*ab 256 Bildpunkte breit, ohne Spectrallinie und mit dickeren Linien:
&nbsp;&nbsp;oyranos\-profile\-graph \-w 256 \-s \-t 3 sRGB.icc ProPhoto\-RGB.icc
#### Zeige HLC Farbatlas Muster
&nbsp;&nbsp;oyranos\-profile\-graph \-H=90 \-o HLC\_H090.png cmyk web
  <br />
&nbsp;&nbsp;Farbmuster werden nur angezeigt, wenn sie im Farbraum von sowohl CMYK und dem web Farbprofil enthalten sind.
#### Zeige die Standard Betrachter Spektralfunktion als Kurven:
&nbsp;&nbsp;oyranos\-profile\-graph \-\-standard\-observer \-o CIE\-StdObserver.png

<h2>SIEHE AUCH <a href="#toc" name="seeaswell">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos\-profile](oyranosprofile.html)<a href="oyranosprofile.md">(1)</a>&nbsp;&nbsp;[oyranos\-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos\-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai\-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005\-2020 Kai\-Uwe Behrmann and others*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos\-cms/oyranos/issues</a>

