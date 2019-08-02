# oyranos\-profile\-graph v0.9.7 {#oyranosprofilegraphde}
*"oyranos\-profile\-graph"* *1* *"March 24, 2019"* "User Commands"
## NAME
oyranos\-profile\-graph v0.9.7 \- Oyranos Profile Zeichner
## ÜBERSICHT
**oyranos\-profile\-graph** [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong> *NUMMER*] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
**oyranos\-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-k</strong> *NUMMER* [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-i</strong> *WORT* [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-s</strong> *DATEI* <strong>\-p</strong> *FORMAT* <strong>\-z</strong> [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-P</strong> *WORT*] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-v</strong>]
<br />
**oyranos\-profile\-graph** <strong>\-X</strong> *json|json+command|man|markdown* | <strong>\-h</strong> [<strong>\-v</strong>]
## BESCHREIBUNG
Das oyranos\-profile\-graph Programm erzeugt aus einem ICC Profil oder eingebetteten ICC Profil in eine Grafik. Vorgabemäßig malt es die Sättigungslinie eines anzugebenden ICC Profiles und schreibt die Grafik nach stdout.
## OPTIONEN
### 2D Zeichnung vom Farbprofil
&nbsp;&nbsp;**oyranos\-profile\-graph** [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-c</strong>] [<strong>\-x</strong>] [<strong>\-d</strong> *NUMMER*] [<strong>\-n</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-r</strong>] [<strong>\-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

Erzeuge aus einem ICC Profil eine Grafik mit seiner Sättigungslinie.

*  *l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE*	ICC Profil
* <strong>\-d</strong>|<strong>\-\-change\-thickness</strong> *NUMMER*	Gib die Verstärkung der Liniendicke an (NUMMER:0,7 [≥\-1000 ≤1000])
* <strong>\-x</strong>|<strong>\-\-xyy</strong>	Benutze CIE*xyY *x*y Ebene für Projektion
* <strong>\-c</strong>|<strong>\-\-no\-blackbody</strong>	Lasse Weißlinie von Lambertstrahlern fort
* <strong>\-n</strong>|<strong>\-\-no\-spectral\-line</strong>	Lasse Spektrallinie fort
* <strong>\-2</strong>|<strong>\-\-icc\-version\-2</strong>	Wähle ICC V2 Profile
* <strong>\-4</strong>|<strong>\-\-icc\-version\-4</strong>	Wähle ICC V4 Profile
* <strong>\-r</strong>|<strong>\-\-no\-repair</strong>	Lasse Reparatur der Profil ID weg

### Standard Betrachter 1931 2° Zeichnung
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-S</strong> [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]

* <strong>\-S</strong>|<strong>\-\-standard\-observer</strong>	CIE Standard Betrachter 1931 2°

### 1964 10° Betrachter Zeichnung
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-O</strong> [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]

* <strong>\-O</strong>|<strong>\-\-observer\-64</strong>	CIE Betrachter 1064 10°

### Spektrumgraph eines Lambertstrahlers
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-k</strong> *NUMMER* [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]

* <strong>\-k</strong>|<strong>\-\-kelvin</strong> *NUMMER*	Lambertstrahler (NUMMER:0 [≥0 ≤25000])

### Beleuchtungsspektrumzeichnung
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-i</strong> *WORT* [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-f</strong> *FORMAT*] [<strong>\-v</strong>]

* <strong>\-i</strong>|<strong>\-\-illuminant</strong> *WORT*	Beleuchtungsspektrum
   * <strong>\-i A</strong>		# Lichtart A
   * <strong>\-i D50</strong>		# Lichtart D50
   * <strong>\-i D55</strong>		# Lichtart D55
   * <strong>\-i D65</strong>		# Lichtart D65
   * <strong>\-i D65T</strong>		# Lichtart D65 T
   * <strong>\-i D75</strong>		# Lichtart D75
   * <strong>\-i D93</strong>		# Lichtart D93

### Spektrumgraph
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-s</strong> *DATEI* <strong>\-p</strong> *FORMAT* <strong>\-z</strong> [<strong>\-t</strong> *NUMMER*] [<strong>\-b</strong>] [<strong>\-g</strong>] [<strong>\-w</strong> *NUMMER*] [<strong>\-R</strong>] [<strong>\-P</strong> *WORT*] [<strong>\-o</strong> *\-|DATEI*] [<strong>\-v</strong>]

* <strong>\-s</strong>|<strong>\-\-spectral</strong> *DATEI*	Spektrale Eingabe
* <strong>\-p</strong>|<strong>\-\-spectral\-format</strong> *FORMAT*	Gib spektrales Ausgabeformat an
   * <strong>\-p png</strong>		# PNG
   * <strong>\-p svg</strong>		# SVG
   * <strong>\-p csv</strong>		# CSV
   * <strong>\-p ncc</strong>		# NCC
   * <strong>\-p cgats</strong>		# CGATS
   * <strong>\-p icc\-xml</strong>		# Icc XML
   * <strong>\-p ppm</strong>		# PPM
* <strong>\-P</strong>|<strong>\-\-pattern</strong> *WORT*	Filter für Farbnamen
* <strong>\-z</strong>|<strong>\-\-scale</strong>	Skaliere die Höhe der Spektrumkurve

### Allgemeine Optionen
&nbsp;&nbsp;**oyranos\-profile\-graph** <strong>\-X</strong> *json|json+command|man|markdown* | <strong>\-h</strong> [<strong>\-v</strong>]

* <strong>\-t</strong>|<strong>\-\-thickness</strong> *NUMMER*	Gib die Liniendicke an (NUMMER:1 [≥0 ≤10])
* <strong>\-b</strong>|<strong>\-\-no\-border</strong>	Lasse Rand aus in Zeichnung
* <strong>\-g</strong>|<strong>\-\-no\-color</strong>	Zeichne grau
* <strong>\-w</strong>|<strong>\-\-width</strong> *NUMMER*	Gib Ausgabebildbreite in Pixel an (NUMMER:128 [≥64 ≤4096])
* <strong>\-R</strong>|<strong>\-\-raster</strong>	Zeichne Gitter
* <strong>\-o</strong>|<strong>\-\-output</strong> *\-|DATEI*	Gib Ausgabedateiname an, voreingestellt ist stdout
* <strong>\-f</strong>|<strong>\-\-format</strong> *FORMAT*	Gib Ausgabeformat PNG oder SVG an, voreingestellt ist PNG
   * <strong>\-f png</strong>		# PNG
   * <strong>\-f svg</strong>		# SVG
* <strong>\-h</strong>|<strong>\-\-help</strong>	Hilfe
* <strong>\-X</strong>|<strong>\-\-export</strong> *json|json+command|man|markdown*	Exportiere formatierten Text: Hole Benutzerschnittstelle als Text
   * <strong>\-X man</strong>		# Handbuch
   * <strong>\-X markdown</strong>		# Markdown
   * <strong>\-X json</strong>		# Json
   * <strong>\-X json+command</strong>		# Json + Kommando
   * <strong>\-X export</strong>		# Export
* <strong>\-v</strong>|<strong>\-\-verbose</strong>	plaudernd

## UMGEBUNGSVARIABLEN
### OY\_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
  <br />
&nbsp;&nbsp;Die \-v Option kann alternativ benutzt werden.
  <br />
&nbsp;&nbsp;Der gültige Bereich ist 1\-20.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
## BEISPIELE
### Zeichne ICC Profil
&nbsp;&nbsp;oyranos\-profile\-graph ICC\_PROFILE
### Zeige Sättigungslinien von zwei Profilen in CIE*ab 256 Bildpunkte breit, ohne Spectrallinie und mit dickeren Linien:
&nbsp;&nbsp;oyranos\-profile\-graph \-w 256 \-s \-t 3 sRGB.icc ProPhoto\-RGB.icc
### Zeige die Standard Betrachter Spektralfunktion als Kurven:
&nbsp;&nbsp;oyranos\-profile\-graph \-\-standard\-observer \-o CIE\-StdObserver.png
## AUTOR
Kai\-Uwe Behrmann http://www.oyranos.org
## KOPIERRECHT
*Copyright 2018 Kai\-Uwe Behrmann*


### Lizenz
newBSD
## FEHLER
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

