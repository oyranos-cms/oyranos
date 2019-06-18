# oyranos\-monitor\-white\-point v0.9.7 {#oyranosmonitorwhitepointde}
*"oyranos\-monitor\-white\-point"* *1* *"October 11, 2018"* "User Commands"
## NAME
oyranos\-monitor\-white\-point v0.9.7 \- Nacht Manager
## ÜBERSICHT
**oyranos\-monitor\-white\-point** \-n *0|1|2|3|4|5|6|7* \-g *ICC\_PROFIL* [\-b *PROZENT*] [\-z] [\-v]
<br />
**oyranos\-monitor\-white\-point** \-w *0|1|2|3|4|5|6|7* \-a *KELVIN* [\-z] [\-v]
<br />
**oyranos\-monitor\-white\-point** \-s *0|1|2|3|4|5|6|7* \-e *ICC\_PROFIL* [\-z] [\-v]
<br />
**oyranos\-monitor\-white\-point** \-l|  \-i *WINKEL\_IN\_GRAD* \-o *WINKEL\_IN\_GRAD* [\-t *WINKEL\_IN\_GRAD|0:Auf\-/Untergang|\-6:zivil|\-12:nautisch|\-18:astronomisch*] [\-z] [\-v]
<br />
**oyranos\-monitor\-white\-point** \-d *0|1|2* [\-v]
<br />
**oyranos\-monitor\-white\-point** \-m|  \-r|  \-X *json|json+command|man|markdown*|  \-h [\-v]
## BESCHREIBUNG
Das Werkzeug kann den momentanen Weißpunkt setzen oder ihn abhängig von der Tages\- und Nachzeit festlegen. Ein zusätzliches Effektprofil kann eingestellt werden.
## OPTIONEN
### Nächtliche Anmutung
**oyranos\-monitor\-white\-point** \-n *0|1|2|3|4|5|6|7* \-g *ICC\_PROFIL* [\-b *PROZENT*] [\-z] [\-v]

Der Nachtweißpunkmodus soll den Einfluß von blauen Licht auf das menschliche Hormonsystem reduzieren. Ein Weißpunkt mit einer Strahlungstemperatur von 4000 Kelvin und weniger erleichtert den natürlichen Schlafrhytmus zu finden. Dieser wird zusammen mit einem warmen Lichtton für die allgemeine Zimmerbeleuchtung wärend der Abend\-und Nachtstunden empfohlen.

* \-n|\-\-night\-white\-point *0|1|2|3|4|5|6|7*	Setze den Nachtmodus: Ein Weißpunkt mit einer Strahlungstemperatur von 4000 Kelvin und weniger erleichtert den natürlichen Schlafrhytmus zu finden. Dazu wird diese Einstellung auf Automatisch (\-n=1) und die Temperatur auf 3000 (\-a=3000) gestellt.
   * \-n 0		# Nein
   * \-n 1		# Automatisch
   * \-n 2		# Lichtart D50
   * \-n 3		# Lichtart D55
   * \-n 4		# Lichtart D65
   * \-n 5		# Lichtart D75
   * \-n 6		# Lichtart D93
* \-g|\-\-night\-effect *ICC\_PROFIL*	Setze den Nacht Effect: Ein ICC Farbprofil der Klasse Abstrakter Farbraum. Günstig wäre ein Effektprofil, welches nur eindimensionale RGB Kurven benutzt und mit meta:EFFECT\_linear=yes gekennzeichnet ist.
   * \-g \-		# [kein]
   * \-g Effect 1		# Example Effect 1
   * \-g Effect 2		# Example Effect 2
* \-b|\-\-night\-backlight *PROZENT*	Setze Nächtiches Monitorlicht: Die Einstellung benötigt xbacklight für das Abdunkeln der Bildschirmlampe. (PROZENT:4 [≥0 ≤100])

### Aktueller Modus
**oyranos\-monitor\-white\-point** \-w *0|1|2|3|4|5|6|7* \-a *KELVIN* [\-z] [\-v]

* \-w|\-\-white\-point *0|1|2|3|4|5|6|7*	Setze Weißpunktmodus
   * \-w 0		# Nein
   * \-w 1		# Automatisch
   * \-w 2		# Lichtart D50
   * \-w 3		# Lichtart D55
   * \-w 4		# Lichtart D65
   * \-w 5		# Lichtart D75
   * \-w 6		# Lichtart D93
* \-a|\-\-automatic *KELVIN*	Werte zwischen 2700 bis 8000 Kelvin sollten keine Darstellungsfehler hervorrufen (KELVIN:2800 [≥1100 ≤10100])

### Setze Tagesmodus
**oyranos\-monitor\-white\-point** \-s *0|1|2|3|4|5|6|7* \-e *ICC\_PROFIL* [\-z] [\-v]

* \-s|\-\-sun\-white\-point *0|1|2|3|4|5|6|7*	Setze den Tagesmodus
   * \-s 0		# Nein
   * \-s 1		# Automatisch
   * \-s 2		# Lichtart D50
   * \-s 3		# Lichtart D55
   * \-s 4		# Lichtart D65
   * \-s 5		# Lichtart D75
   * \-s 6		# Lichtart D93
* \-e|\-\-sunlight\-effect *ICC\_PROFIL*	Setze den Tages Effect: Ein ICC Farbprofil der Klasse Abstrakter Farbraum. Günstig wäre ein Effektprofil, welches nur eindimensionale RGB Kurven benutzt und mit meta:EFFECT\_linear=yes gekennzeichnet ist.
   * \-e \-		# [kein]
   * \-e Effect 1		# Example Effect 1
   * \-e Effect 2		# Example Effect 2

### Ort und Dämmerung
**oyranos\-monitor\-white\-point** \-l|  \-i *WINKEL\_IN\_GRAD* \-o *WINKEL\_IN\_GRAD* [\-t *WINKEL\_IN\_GRAD|0:Auf\-/Untergang|\-6:zivil|\-12:nautisch|\-18:astronomisch*] [\-z] [\-v]

* \-l|\-\-location	Erhalte Position von IP Adresse
* \-i|\-\-latitude *WINKEL\_IN\_GRAD*	Setze Geographische Breite (WINKEL\_IN\_GRAD:0 [≥\-90 ≤90])
* \-o|\-\-longitude *WINKEL\_IN\_GRAD*	Setze Geographische Länge (WINKEL\_IN\_GRAD:0 [≥\-180 ≤180])
* \-t|\-\-twilight *WINKEL\_IN\_GRAD|0:Auf\-/Untergang|\-6:zivil|\-12:nautisch|\-18:astronomisch*	Setze Dämmerungswinkel (WINKEL\_IN\_GRAD|0:Auf\-/Untergang|\-6:zivil|\-12:nautisch|\-18:astronomisch:0 [≥18 ≤\-18])

### Setze Sonnenuntergangsdienst
**oyranos\-monitor\-white\-point** \-d *0|1|2* [\-v]

* \-d|\-\-daemon *0|1|2*	Setze Sonnenuntergangsdienst
   * \-d 0		# Deaktiviere
   * \-d 1		# Automatischer Start
   * \-d 2		# Aktiviere

### Allgemeine Optionen
**oyranos\-monitor\-white\-point** \-m|  \-r|  \-X *json|json+command|man|markdown*|  \-h [\-v]

* \-h|\-\-help	Hilfe
* \-m|\-\-modes	Zeige Weißpunktmodus
* \-r|\-\-sunrise	Zeige lokale Zeit, benutzte geografische Position, Dämmerungswinkel, Sonnenauf\-und untergangszeiten
* \-X|\-\-export *json|json+command|man|markdown*	Exportiere formatierten Text: Hole Benutzerschnittstelle als Text
   * \-X json		# Json
   * \-X json+command		# Json + Kommando
   * \-X man		# Handbuch
   * \-X markdown		# Markdown
   * \-X export		# Export
* \-z|\-\-system\-wide	Einstellung in der systemweiten DB
* \-v|\-\-verbose	plaudernd

## UMGEBUNGSVARIABLEN
### OY\_DEBUG
Setze das Oyranos Fehlersuchniveau. Die \-v Option kann alternativ benutzt werden. Der gültige Bereich ist 1\-20.
### OY\_MODULE\_PATH
zeige Oyranos zusätzliche Verzeichnisse mit Modulen.  
## BEISPIELE
### Starte den Wächter, setze nächtlichen Weißpunkt zu 3000 Kelvin und benutze diesen im Nachtmodus
oyranos\-monitor\-white\-point \-d 2 \-a 3000 \-n 1 
### Schalte alle Tagesbeeinflussung aus, wie Weißpunkt und Effekt
oyranos\-monitor\-white\-point \-s 0 \-e 0 
## AUTOR
Kai\-Uwe Behrmann http://www.oyranos.org
## KOPIERRECHT
*Copyright 2018 Kai\-Uwe Behrmann*


### Lizenz
newBSD
## FEHLER
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

