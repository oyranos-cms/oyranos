# oyranos\-monitor\-white\-point v0.9.7 {#oyranosmonitorwhitepointde}
*"oyranos\-monitor\-white\-point"* *1* *"October 11, 2018"* "User Commands"
## NAME
oyranos\-monitor\-white\-point v0.9.7 \- Nacht Manager
## ÜBERSICHT
**oyranos\-monitor\-white\-point** <strong>\-n</strong> *0|1|2|3|4|5|6|7* <strong>\-g</strong> *ICC\_PROFIL* [<strong>\-b</strong> *PROZENT*] [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-w</strong> *0|1|2|3|4|5|6|7* <strong>\-a</strong> *KELVIN* [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-s</strong> *0|1|2|3|4|5|6|7* <strong>\-e</strong> *ICC\_PROFIL* [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-l</strong> | <strong>\-i</strong> *WINKEL\_IN\_GRAD* <strong>\-o</strong> *WINKEL\_IN\_GRAD* [<strong>\-t</strong> *WINKEL\_IN\_GRAD|0:Auf\-/Untergang|\-6:zivil|\-12:nautisch|\-18:astronomisch*] [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-d</strong> *0|1|2* [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-m</strong> | <strong>\-r</strong> | <strong>\-X</strong> *json|json+command|man|markdown* | <strong>\-h</strong> [<strong>\-v</strong>]
## BESCHREIBUNG
Das Werkzeug kann den momentanen Weißpunkt setzen oder ihn abhängig von der Tages\- und Nachzeit festlegen. Ein zusätzliches Effektprofil kann eingestellt werden.
## OPTIONEN
### Nächtliche Anmutung
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-n</strong> *0|1|2|3|4|5|6|7* <strong>\-g</strong> *ICC\_PROFIL* [<strong>\-b</strong> *PROZENT*] [<strong>\-z</strong>] [<strong>\-v</strong>]

Der Nachtweißpunkmodus soll den Einfluß von blauen Licht auf das menschliche Hormonsystem reduzieren. Ein Weißpunkt mit einer Strahlungstemperatur von 4000 Kelvin und weniger erleichtert den natürlichen Schlafrhytmus zu finden. Dieser wird zusammen mit einem warmen Lichtton für die allgemeine Zimmerbeleuchtung wärend der Abend\-und Nachtstunden empfohlen.

* <strong>\-n</strong>|<strong>\-\-night\-white\-point</strong> *0|1|2|3|4|5|6|7*	Setze den Nachtmodus: Ein Weißpunkt mit einer Strahlungstemperatur von 4000 Kelvin und weniger erleichtert den natürlichen Schlafrhytmus zu finden. Dazu wird diese Einstellung auf Automatisch (\-n=1) und die Temperatur auf 3000 (\-a=3000) gestellt.
   * <strong>\-n 0</strong>		# Nein
   * <strong>\-n 1</strong>		# Automatisch
   * <strong>\-n 2</strong>		# Lichtart D50
   * <strong>\-n 3</strong>		# Lichtart D55
   * <strong>\-n 4</strong>		# Lichtart D65
   * <strong>\-n 5</strong>		# Lichtart D75
   * <strong>\-n 6</strong>		# Lichtart D93
* <strong>\-g</strong>|<strong>\-\-night\-effect</strong> *ICC\_PROFIL*	Setze den Nacht Effect: Ein ICC Farbprofil der Klasse Abstrakter Farbraum. Günstig wäre ein Effektprofil, welches nur eindimensionale RGB Kurven benutzt und mit meta:EFFECT\_linear=yes gekennzeichnet ist.
   * <strong>\-g \-</strong>		# [kein]
   * <strong>\-g Effect 1</strong>		# Example Effect 1
   * <strong>\-g Effect 2</strong>		# Example Effect 2
* <strong>\-b</strong>|<strong>\-\-night\-backlight</strong> *PROZENT*	Setze Nächtiches Monitorlicht: Die Einstellung benötigt xbacklight für das Abdunkeln der Bildschirmlampe. (PROZENT:4 [≥0 ≤100])

### Aktueller Modus
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-w</strong> *0|1|2|3|4|5|6|7* <strong>\-a</strong> *KELVIN* [<strong>\-z</strong>] [<strong>\-v</strong>]

* <strong>\-w</strong>|<strong>\-\-white\-point</strong> *0|1|2|3|4|5|6|7*	Setze Weißpunktmodus
   * <strong>\-w 0</strong>		# Nein
   * <strong>\-w 1</strong>		# Automatisch
   * <strong>\-w 2</strong>		# Lichtart D50
   * <strong>\-w 3</strong>		# Lichtart D55
   * <strong>\-w 4</strong>		# Lichtart D65
   * <strong>\-w 5</strong>		# Lichtart D75
   * <strong>\-w 6</strong>		# Lichtart D93
* <strong>\-a</strong>|<strong>\-\-automatic</strong> *KELVIN*	Werte zwischen 2700 bis 8000 Kelvin sollten keine Darstellungsfehler hervorrufen (KELVIN:2800 [≥1100 ≤10100])

### Setze Tagesmodus
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-s</strong> *0|1|2|3|4|5|6|7* <strong>\-e</strong> *ICC\_PROFIL* [<strong>\-z</strong>] [<strong>\-v</strong>]

* <strong>\-s</strong>|<strong>\-\-sun\-white\-point</strong> *0|1|2|3|4|5|6|7*	Setze den Tagesmodus
   * <strong>\-s 0</strong>		# Nein
   * <strong>\-s 1</strong>		# Automatisch
   * <strong>\-s 2</strong>		# Lichtart D50
   * <strong>\-s 3</strong>		# Lichtart D55
   * <strong>\-s 4</strong>		# Lichtart D65
   * <strong>\-s 5</strong>		# Lichtart D75
   * <strong>\-s 6</strong>		# Lichtart D93
* <strong>\-e</strong>|<strong>\-\-sunlight\-effect</strong> *ICC\_PROFIL*	Setze den Tages Effect: Ein ICC Farbprofil der Klasse Abstrakter Farbraum. Günstig wäre ein Effektprofil, welches nur eindimensionale RGB Kurven benutzt und mit meta:EFFECT\_linear=yes gekennzeichnet ist.
   * <strong>\-e \-</strong>		# [kein]
   * <strong>\-e Effect 1</strong>		# Example Effect 1
   * <strong>\-e Effect 2</strong>		# Example Effect 2

### Ort und Dämmerung
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-l</strong> | <strong>\-i</strong> *WINKEL\_IN\_GRAD* <strong>\-o</strong> *WINKEL\_IN\_GRAD* [<strong>\-t</strong> *WINKEL\_IN\_GRAD|0:Auf\-/Untergang|\-6:zivil|\-12:nautisch|\-18:astronomisch*] [<strong>\-z</strong>] [<strong>\-v</strong>]

* <strong>\-l</strong>|<strong>\-\-location</strong>	Erhalte Position von IP Adresse
* <strong>\-i</strong>|<strong>\-\-latitude</strong> *WINKEL\_IN\_GRAD*	Setze Geographische Breite (WINKEL\_IN\_GRAD:0 [≥\-90 ≤90])
* <strong>\-o</strong>|<strong>\-\-longitude</strong> *WINKEL\_IN\_GRAD*	Setze Geographische Länge (WINKEL\_IN\_GRAD:0 [≥\-180 ≤180])
* <strong>\-t</strong>|<strong>\-\-twilight</strong> *WINKEL\_IN\_GRAD|0:Auf\-/Untergang|\-6:zivil|\-12:nautisch|\-18:astronomisch*	Setze Dämmerungswinkel (WINKEL\_IN\_GRAD|0:Auf\-/Untergang|\-6:zivil|\-12:nautisch|\-18:astronomisch:0 [≥18 ≤\-18])

### Setze Sonnenuntergangsdienst
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-d</strong> *0|1|2* [<strong>\-v</strong>]

* <strong>\-d</strong>|<strong>\-\-daemon</strong> *0|1|2*	Setze Sonnenuntergangsdienst
   * <strong>\-d 0</strong>		# Deaktiviere
   * <strong>\-d 1</strong>		# Automatischer Start
   * <strong>\-d 2</strong>		# Aktiviere

### Allgemeine Optionen
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-m</strong> | <strong>\-r</strong> | <strong>\-X</strong> *json|json+command|man|markdown* | <strong>\-h</strong> [<strong>\-v</strong>]

* <strong>\-h</strong>|<strong>\-\-help</strong>	Hilfe
* <strong>\-m</strong>|<strong>\-\-modes</strong>	Zeige Weißpunktmodus
* <strong>\-r</strong>|<strong>\-\-sunrise</strong>	Zeige lokale Zeit, benutzte geografische Position, Dämmerungswinkel, Sonnenauf\-und untergangszeiten
* <strong>\-X</strong>|<strong>\-\-export</strong> *json|json+command|man|markdown*	Exportiere formatierten Text: Hole Benutzerschnittstelle als Text
   * <strong>\-X man</strong>		# Handbuch
   * <strong>\-X markdown</strong>		# Markdown
   * <strong>\-X json</strong>		# Json
   * <strong>\-X json+command</strong>		# Json + Kommando
   * <strong>\-X export</strong>		# Export
* <strong>\-z</strong>|<strong>\-\-system\-wide</strong>	Einstellung in der systemweiten DB
* <strong>\-v</strong>|<strong>\-\-verbose</strong>	plaudernd

## UMGEBUNGSVARIABLEN
### OY\_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
  <br />
&nbsp;&nbsp;Die \-v Option kann alternativ benutzt werden.
  <br />
&nbsp;&nbsp;Der gültige Bereich ist 1\-20.
### OY\_MODULE\_PATH
&nbsp;&nbsp;zeige Oyranos zusätzliche Verzeichnisse mit Modulen.
## BEISPIELE
### Starte den Wächter, setze nächtlichen Weißpunkt zu 3000 Kelvin und benutze diesen im Nachtmodus
&nbsp;&nbsp;oyranos\-monitor\-white\-point \-d 2 \-a 3000 \-n 1
### Schalte alle Tagesbeeinflussung aus, wie Weißpunkt und Effekt
&nbsp;&nbsp;oyranos\-monitor\-white\-point \-s 0 \-e 0
## AUTOR
Kai\-Uwe Behrmann http://www.oyranos.org
## KOPIERRECHT
*Copyright 2018 Kai\-Uwe Behrmann*


### Lizenz
newBSD
## FEHLER
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

