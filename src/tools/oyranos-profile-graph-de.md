# oyranos\-profile\-graph v0.9.7 {#oyranosprofilegraphde}
*"oyranos\-profile\-graph"* *1* *"March 24, 2019"* "User Commands"
## NAME
oyranos\-profile\-graph v0.9.7 \- Oyranos Profile Zeichner
## ÜBERSICHT
**oyranos\-profile\-graph** [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-c] [\-x] [\-d *NUMMER*] [\-n] [\-2] [\-4] [\-r] [\-v] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
**oyranos\-profile\-graph** \-S [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-v]
<br />
**oyranos\-profile\-graph** \-O [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-v]
<br />
**oyranos\-profile\-graph** \-k *NUMMER* [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-v]
<br />
**oyranos\-profile\-graph** \-i *STRING* [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-v]
<br />
**oyranos\-profile\-graph** \-s *DATEI* \-p *FORMAT* [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-P *STRING*] [\-o *\-|DATEI*] [\-v]
<br />
**oyranos\-profile\-graph** \-X *json|json+command|man|markdown*|  \-h [\-v]
## BESCHREIBUNG
Das oyranos\-profile\-graph Programm erzeugt aus einem ICC Profil oder eingebetteten ICC Profil in eine Grafik. Vorgabemäßig malt es die Sättigungslinie eines anzugebenden ICC Profiles und schreibt die Grafik nach stdout.
## OPTIONEN
### 2D Zeichnung vom Farbprofil
**oyranos\-profile\-graph** [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-c] [\-x] [\-d *NUMMER*] [\-n] [\-2] [\-4] [\-r] [\-v] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

Erzeuge aus einem ICC Profil eine Grafik mit seiner Sättigungslinie.

*  *l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE*	ICC Profil
* \-d|\-\-change\-thickness *NUMMER*	Gib die Verstärkung der Liniendicke an (NUMMER:0,7 [≥\-1000 ≤1000])
* \-x|\-\-xyy	Benutze CIE*xyY *x*y Ebene für Projektion
* \-c|\-\-no\-blackbody	Lasse Weißlinie von Lambertstrahlern fort
* \-n|\-\-no\-spectral\-line	Lasse Spektrallinie fort
* \-2|\-\-icc\-version\-2	Wähle ICC V2 Profile
* \-4|\-\-icc\-version\-4	Wähle ICC V4 Profile
* \-r|\-\-no\-repair	Lasse Reparatur der Profil ID weg

### Standard Betrachter 1931 2° Zeichnung
**oyranos\-profile\-graph** \-S [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-v]

* \-S|\-\-standard\-observer	CIE Standard Betrachter 1931 2°

### 1964 10° Betrachter Zeichnung
**oyranos\-profile\-graph** \-O [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-v]

* \-O|\-\-observer\-64	CIE Betrachter 1064 10°

### Spektrumgraph eines Lambertstrahlers
**oyranos\-profile\-graph** \-k *NUMMER* [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-v]

* \-k|\-\-kelvin *NUMMER*	Lambertstrahler (NUMMER:0 [≥0 ≤25000])

### Beleuchtungsspektrumzeichnung
**oyranos\-profile\-graph** \-i *STRING* [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-o *\-|DATEI*] [\-f *FORMAT*] [\-v]

* \-i|\-\-illuminant *STRING*	Beleuchtungsspektrum
   * \-i A		# Lichtart A
   * \-i D50		# Lichtart D50
   * \-i D55		# Lichtart D55
   * \-i D65		# Lichtart D65
   * \-i D75		# Lichtart D75
   * \-i D93		# Lichtart D93

### Spektrumgraph
**oyranos\-profile\-graph** \-s *DATEI* \-p *FORMAT* [\-t *NUMMER*] [\-b] [\-g] [\-w *NUMMER*] [\-R] [\-P *STRING*] [\-o *\-|DATEI*] [\-v]

* \-s|\-\-spectral *DATEI*	Spektrale Eingabe
* \-p|\-\-spectral\-format *FORMAT*	Gib spektrales Ausgabeformat an
   * \-p png		# PNG
   * \-p svg		# SVG
   * \-p csv		# CSV
   * \-p ncc		# NCC
   * \-p cgats		# CGATS
   * \-p icc\-xml		# Icc XML
   * \-p ppm		# PPM
* \-P|\-\-pattern *STRING*	Filter für Farbnamen

### Allgemeine Optionen
**oyranos\-profile\-graph** \-X *json|json+command|man|markdown*|  \-h [\-v]

* \-t|\-\-thickness *NUMMER*	Gib die Liniendicke an (NUMMER:1 [≥0 ≤10])
* \-b|\-\-no\-border	Lasse Rand aus in Zeichnung
* \-g|\-\-no\-color	Zeichne grau
* \-w|\-\-width *NUMMER*	Gib Ausgabebildbreite in Pixel an (NUMMER:128 [≥64 ≤4096])
* \-R|\-\-raster	Zeichne Gitter
* \-o|\-\-output *\-|DATEI*	Gib Ausgabedateiname an, voreingestellt ist stdout
* \-f|\-\-format *FORMAT*	Gib Ausgabeformat PNG oder SVG an, voreingestellt ist PNG
   * \-f png		# PNG
   * \-f svg		# SVG
* \-h|\-\-help	Hilfe
* \-X|\-\-export *json|json+command|man|markdown*	Exportiere formatierten Text: Hole Benutzerschnittstelle als Text
   * \-X json		# Json
   * \-X json+command		# Json + Kommando
   * \-X man		# Handbuch
   * \-X markdown		# Markdown
* \-v|\-\-verbose	plaudernd

## UMGEBUNGSVARIABLEN
### OY\_DEBUG
Setze das Oyranos Fehlersuchniveau. Die \-v Option kann alternativ benutzt werden. Der gültige Bereich ist 1\-20.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal  
## BEISPIELE
### Zeichne ICC Profil
oyranos\-profile\-graph ICC\_PROFILE 
### Zeige Sättigungslinien von zwei Profilen in CIE*ab 256 Bildpunkte breit, ohne Spectrallinie und mit dickeren Linien:
oyranos\-profile\-graph \-w 256 \-s \-t 3 sRGB.icc ProPhoto\-RGB.icc 
### Zeige die Standard Betrachter Spektralfunktion als Kurven:
oyranos\-profile\-graph \-\-standard\-observer \-o CIE\-StdObserver.png 
## AUTOR
Kai\-Uwe Behrmann http://www.oyranos.org
## KOPIERRECHT
*Copyright 2018 Kai\-Uwe Behrmann*


### Lizenz
newBSD
## FEHLER
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

