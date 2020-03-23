# oyranos\-monitor\-white\-point v0.9.7 {#oyranosmonitorwhitepointde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [UMGEBUNGSVARIABLEN](#environmentvariables) [BEISPIELE](#examples) [SIEHE AUCH](#seeaswell) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

*"oyranos\-monitor\-white\-point"* *1* *"October 11, 2018"* "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos\-monitor\-white\-point v0.9.7 \- Nacht Manager

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

**oyranos\-monitor\-white\-point** <strong>\-n</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>\-g</strong>=<em>ICC_PROFIL</em> [<strong>\-b</strong>=<em>PROZENT</em>] [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-w</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>\-a</strong>=<em>KELVIN</em> [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-s</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>\-e</strong>=<em>ICC_PROFIL</em> [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-l</strong> | <strong>\-i</strong>=<em>WINKEL_IN_GRAD</em> <strong>\-o</strong>=<em>WINKEL_IN_GRAD</em> [<strong>\-t</strong>=<em>WINKEL_IN_GRAD|0:Auf\-/Untergang|\-6:zivil|\-12:nautisch|\-18:astronomisch</em>] [<strong>\-z</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-d</strong>=<em>0|1|2</em> [<strong>\-v</strong>]
<br />
**oyranos\-monitor\-white\-point** <strong>\-m</strong> | <strong>\-r</strong> | <strong>\-X</strong>=<em>json|json+command|man|markdown</em> | <strong>\-h</strong> [<strong>\-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das Werkzeug kann den momentanen Weißpunkt setzen oder ihn abhängig von der Tages\- und Nachzeit festlegen. Ein zusätzliches Effektprofil kann eingestellt werden.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

#### Nächtliche Anmutung
&nbsp;&nbsp;**oyranos\-monitor\-white\-point** <strong>\-n</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>\-g</strong>=<em>ICC_PROFIL</em> [<strong>\-b</strong>=<em>PROZENT</em>] [<strong>\-z</strong>] [<strong>\-v</strong>]

&nbsp;&nbsp;Der Nachtweißpunkmodus soll den Einfluß von blauen Licht auf das menschliche Hormonsystem reduzieren. Ein Weißpunkt mit einer Strahlungstemperatur von 4000 Kelvin und weniger erleichtert den natürlichen Schlafrhytmus zu finden. Dieser wird zusammen mit einem warmen Lichtton für die allgemeine Zimmerbeleuchtung wärend der Abend\-und Nachtstunden empfohlen.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--night-white-point</strong> <em>0|1|2|3|4|5|6|7</em></td> <td>Setze den Nachtmodus<br />Ein Weißpunkt mit einer Strahlungstemperatur von 4000 Kelvin und weniger erleichtert den natürlichen Schlafrhytmus zu finden. Dazu wird diese Einstellung auf Automatisch (-n=1) und die Temperatur auf 3000 (-a=3000) gestellt.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-n 0</strong></td><td># Nein</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 1</strong></td><td># Automatisch</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 2</strong></td><td># Lichtart D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 3</strong></td><td># Lichtart D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 4</strong></td><td># Lichtart D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 5</strong></td><td># Lichtart D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n 6</strong></td><td># Lichtart D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--night-effect</strong> <em>ICC_PROFIL</em></td> <td>Setze den Nacht Effect<br />Ein ICC Farbprofil der Klasse Abstrakter Farbraum. Günstig wäre ein Effektprofil, welches nur eindimensionale RGB Kurven benutzt und mit meta:EFFECT_linear=yes gekennzeichnet ist.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-g -</strong></td><td># [kein]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-g Effect 1</strong></td><td># Example Effect 1</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-g Effect 2</strong></td><td># Example Effect 2</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--night-backlight</strong> <em>PROZENT</em></td> <td>Setze Nächtiches Monitorlicht: Die Einstellung benötigt xbacklight für das Abdunkeln der Bildschirmlampe. (PROZENT:4 [≥0 ≤100])</td> </tr>
</table>

#### Aktueller Modus
&nbsp;&nbsp;**oyranos-monitor-white-point** <strong>-w</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-a</strong>=<em>KELVIN</em> [<strong>-z</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--white-point</strong> <em>0|1|2|3|4|5|6|7</em></td> <td>Setze Weißpunktmodus
  <table>
   <tr><td style='padding-left:0.5em'><strong>-w 0</strong></td><td># Nein</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 1</strong></td><td># Automatisch</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 2</strong></td><td># Lichtart D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 3</strong></td><td># Lichtart D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 4</strong></td><td># Lichtart D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 5</strong></td><td># Lichtart D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w 6</strong></td><td># Lichtart D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--automatic</strong> <em>KELVIN</em></td> <td>Werte zwischen 2700 bis 8000 Kelvin sollten keine Darstellungsfehler hervorrufen (KELVIN:2800 [≥1100 ≤10100])</td> </tr>
</table>

#### Setze Tagesmodus
&nbsp;&nbsp;**oyranos-monitor-white-point** <strong>-s</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-e</strong>=<em>ICC_PROFIL</em> [<strong>-z</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--sun-white-point</strong> <em>0|1|2|3|4|5|6|7</em></td> <td>Setze den Tagesmodus
  <table>
   <tr><td style='padding-left:0.5em'><strong>-s 0</strong></td><td># Nein</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 1</strong></td><td># Automatisch</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 2</strong></td><td># Lichtart D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 3</strong></td><td># Lichtart D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 4</strong></td><td># Lichtart D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 5</strong></td><td># Lichtart D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s 6</strong></td><td># Lichtart D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--sunlight-effect</strong> <em>ICC_PROFIL</em></td> <td>Setze den Tages Effect<br />Ein ICC Farbprofil der Klasse Abstrakter Farbraum. Günstig wäre ein Effektprofil, welches nur eindimensionale RGB Kurven benutzt und mit meta:EFFECT_linear=yes gekennzeichnet ist.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-e -</strong></td><td># [kein]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-e Effect 1</strong></td><td># Example Effect 1</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-e Effect 2</strong></td><td># Example Effect 2</td></tr>
  </table>
  </td>
 </tr>
</table>

#### Ort und Dämmerung
&nbsp;&nbsp;**oyranos-monitor-white-point** <strong>-l</strong> | <strong>-i</strong>=<em>WINKEL_IN_GRAD</em> <strong>-o</strong>=<em>WINKEL_IN_GRAD</em> [<strong>-t</strong>=<em>WINKEL_IN_GRAD|0:Auf-/Untergang|-6:zivil|-12:nautisch|-18:astronomisch</em>] [<strong>-z</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--location</strong></td> <td>Erhalte Position von IP Adresse</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--latitude</strong> <em>WINKEL_IN_GRAD</em></td> <td>Setze Geographische Breite (WINKEL_IN_GRAD:0 [≥-90 ≤90])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--longitude</strong> <em>WINKEL_IN_GRAD</em></td> <td>Setze Geographische Länge (WINKEL_IN_GRAD:0 [≥-180 ≤180])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--twilight</strong> <em>WINKEL_IN_GRAD|0:Auf-/Untergang|-6:zivil|-12:nautisch|-18:astronomisch</em></td> <td>Setze Dämmerungswinkel (WINKEL_IN_GRAD|0:Auf-/Untergang|-6:zivil|-12:nautisch|-18:astronomisch:0 [≥18 ≤-18])</td> </tr>
</table>

#### Setze Sonnenuntergangsdienst
&nbsp;&nbsp;**oyranos-monitor-white-point** <strong>-d</strong>=<em>0|1|2</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--daemon</strong> <em>0|1|2</em></td> <td>Setze Sonnenuntergangsdienst
  <table>
   <tr><td style='padding-left:0.5em'><strong>-d 0</strong></td><td># Deaktiviere</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d 1</strong></td><td># Automatischer Start</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d 2</strong></td><td># Aktiviere</td></tr>
  </table>
  </td>
 </tr>
</table>

#### Allgemeine Optionen
&nbsp;&nbsp;**oyranos-monitor-white-point** <strong>-m</strong> | <strong>-r</strong> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Hilfe</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--modes</strong></td> <td>Zeige Weißpunktmodus</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--sunrise</strong></td> <td>Zeige lokale Zeit, benutzte geografische Position, Dämmerungswinkel, Sonnenauf-und untergangszeiten</td> </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-z</strong>|<strong>--system-wide</strong></td> <td>Einstellung in der systemweiten DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>


<h2>UMGEBUNGSVARIABLEN <a href="#toc" name="environmentvariables">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
  <br />
&nbsp;&nbsp;Die -v Option kann alternativ benutzt werden.
  <br />
&nbsp;&nbsp;Der gültige Bereich ist 1-20.
#### OY_MODULE_PATH
&nbsp;&nbsp;zeige Oyranos zusätzliche Verzeichnisse mit Modulen.

<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Starte den Wächter, setze nächtlichen Weißpunkt zu 3000 Kelvin und benutze diesen im Nachtmodus
&nbsp;&nbsp;oyranos-monitor-white-point -d 2 -a 3000 -n 1
#### Schalte alle Tagesbeeinflussung aus, wie Weißpunkt und Effekt
&nbsp;&nbsp;oyranos-monitor-white-point -s 0 -e 0

<h2>SIEHE AUCH <a href="#toc" name="seeaswell">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-monitor](oyranosmonitor.html)<a href="oyranosmonitor.md">(1)</a>&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2020 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

