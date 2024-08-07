# oyranos\-monitor\-white\-point v0.9.7 {#oyranosmonitorwhitepointde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [ALLGEMEINE OPTIONEN](#general_options) [UMGEBUNGSVARIABLEN](#environmentvariables) [BEISPIELE](#examples) [SIEHE AUCH](#seeaswell) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyranos-monitor-white-point"</strong> *1* <em>"October 11, 2018"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-monitor-white-point v0.9.7 - Nacht Manager

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-monitor-white-point</strong> <a href="#night-white-point"><strong>-n</strong>=<em>0|1|2|3|4|5|6|7</em></a> <strong>-g</strong>=<em>ICC_PROFIL</em> <strong>-b</strong>=<em>PROZENT</em> <strong>--night-color-scheme</strong>=<em>WORT</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#white-point"><strong>-w</strong>=<em>0|1|2|3|4|5|6|7</em></a> <strong>-a</strong>=<em>KELVIN</em> [<strong>-z</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#sun-white-point"><strong>-s</strong>=<em>0|1|2|3|4|5|6|7</em></a> <strong>-e</strong>=<em>ICC_PROFIL</em> <strong>--sunlight-color-scheme</strong>=<em>WORT</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#location"><strong>-l</strong></a> | <strong>-i</strong>=<em>WINKEL_IN_GRAD</em> <strong>-o</strong>=<em>WINKEL_IN_GRAD</em> [<strong>-t</strong>=<em>WINKEL_IN_GRAD</em>] [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#sunrise"><strong>-r</strong><em>[=FORMAT]</em></a> <strong>-m</strong> [<strong>-v</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#daemon"><strong>-d</strong><em>[=0|1|2]</em></a> [<strong>-v</strong>]
<br />
<strong>oyranos-monitor-white-point</strong> <a href="#export"><strong>-X</strong>=<em>json|json+command|man|markdown</em></a> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das Werkzeug kann den momentanen Weißpunkt setzen oder ihn abhängig von der Tages- und Nachzeit festlegen. Ein zusätzliches Effektprofil kann eingestellt werden.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="night-white-point">Nächtliche Anmutung</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-n</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-g</strong>=<em>ICC_PROFIL</em> <strong>-b</strong>=<em>PROZENT</em> <strong>--night-color-scheme</strong>=<em>WORT</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]

&nbsp;&nbsp;Der Nachtweißpunkmodus soll den Einfluß von blauen Licht auf das menschliche Hormonsystem reduzieren. Ein Weißpunkt mit einer Strahlungstemperatur von 4000 Kelvin und weniger erleichtert den natürlichen Schlafrhytmus zu finden. Dieser wird zusammen mit einem warmen Lichtton für die allgemeine Zimmerbeleuchtung wärend der Abend-und Nachtstunden empfohlen.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--night-white-point</strong>=<em>0|1|2|3|4|5|6|7</em></td> <td>Setze den Nachtmodus<br />Ein Weißpunkt mit einer Strahlungstemperatur von 4000 Kelvin und weniger erleichtert den natürlichen Schlafrhytmus zu finden. Dazu wird diese Einstellung auf Automatisch (-n=1) und die Temperatur auf 3000 (-a=3000) gestellt.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 0</td><td># Nein</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 1</td><td># Automatisch</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 2</td><td># Lichtart D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 3</td><td># Lichtart D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 4</td><td># Lichtart D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 5</td><td># Lichtart D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-n</strong> 6</td><td># Lichtart D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--night-effect</strong>=<em>ICC_PROFIL</em></td> <td>Setze den Nacht Effect<br />Ein ICC Farbprofil der Klasse Abstrakter Farbraum. Günstig wäre ein Effektprofil, welches nur eindimensionale RGB Kurven benutzt und mit meta:EFFECT_linear=yes gekennzeichnet ist.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-g</strong> -</td><td># [kein]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-g</strong> Effect-1</td><td># Example Effect 1 : ICC profile of class abstract, wich affects gamma</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-g</strong> Effect-2</td><td># Example Effect 2 : ICC profile of class abstract, wich affects gamma</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--night-backlight</strong>=<em>PROZENT</em></td> <td>Setze Nächtiches Monitorlicht: Die Einstellung benötigt xbacklight für das Abdunkeln der Bildschirmlampe. (PROZENT:4 [≥0 ≤100 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--night-color-scheme</strong>=<em>WORT</em></td> <td>Setze zur Nacht ein typischerweise dunkles Farbschema<br />Benutze dies um das Farbschema abhängig von Tag und Nacht umzustellen. plasma-apply-colorscheme --list-schemes
  <table>
   <tr><td style='padding-left:0.5em'><strong>--night-color-scheme</strong> -</td><td># [kein]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>--night-color-scheme</strong> Breeze</td><td># Breeze : Detected Color Scheme</td></tr>
   <tr><td style='padding-left:0.5em'><strong>--night-color-scheme</strong> BreezeDark</td><td># Breeze Dark : 2. detected Color Scheme</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="white-point">Aktueller Modus</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-w</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-a</strong>=<em>KELVIN</em> [<strong>-z</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--white-point</strong>=<em>0|1|2|3|4|5|6|7</em></td> <td>Setze Weißpunktmodus
  <table>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 0</td><td># Nein</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 1</td><td># Automatisch</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 2</td><td># Lichtart D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 3</td><td># Lichtart D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 4</td><td># Lichtart D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 5</td><td># Lichtart D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> 6</td><td># Lichtart D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--automatic</strong>=<em>KELVIN</em></td> <td>Werte zwischen 2700 bis 8000 Kelvin sollten keine Darstellungsfehler hervorrufen (KELVIN:2800 [≥1100 ≤10100 Δ100])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--test</strong></td> <td></td> </tr>
</table>

<h3 id="sun-white-point">Setze Tagesmodus</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-s</strong>=<em>0|1|2|3|4|5|6|7</em> <strong>-e</strong>=<em>ICC_PROFIL</em> <strong>--sunlight-color-scheme</strong>=<em>WORT</em> [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--sun-white-point</strong>=<em>0|1|2|3|4|5|6|7</em></td> <td>Setze den Tagesmodus
  <table>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 0</td><td># Nein</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 1</td><td># Automatisch</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 2</td><td># Lichtart D50</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 3</td><td># Lichtart D55</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 4</td><td># Lichtart D65</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 5</td><td># Lichtart D75</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-s</strong> 6</td><td># Lichtart D93</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--sunlight-effect</strong>=<em>ICC_PROFIL</em></td> <td>Setze den Tages Effect<br />Ein ICC Farbprofil der Klasse Abstrakter Farbraum. Günstig wäre ein Effektprofil, welches nur eindimensionale RGB Kurven benutzt und mit meta:EFFECT_linear=yes gekennzeichnet ist.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-e</strong> -</td><td># [kein]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-e</strong> Effect-1</td><td># Example Effect 1 : ICC profile of class abstract, wich affects gamma</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-e</strong> Effect-2</td><td># Example Effect 2 : ICC profile of class abstract, wich affects gamma</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--sunlight-color-scheme</strong>=<em>WORT</em></td> <td>Setze für Tageslight ein typischerweise helleres Farbschema<br />Benutze dies um das Farbschema abhängig von Tag und Nacht umzustellen. plasma-apply-colorscheme --list-schemes
  <table>
   <tr><td style='padding-left:0.5em'><strong>--sunlight-color-scheme</strong> -</td><td># [kein]</td></tr>
   <tr><td style='padding-left:0.5em'><strong>--sunlight-color-scheme</strong> Breeze</td><td># Breeze : Detected Color Scheme</td></tr>
   <tr><td style='padding-left:0.5em'><strong>--sunlight-color-scheme</strong> BreezeDark</td><td># Breeze Dark : 2. detected Color Scheme</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="location">Ort und Dämmerung</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-l</strong> | <strong>-i</strong>=<em>WINKEL_IN_GRAD</em> <strong>-o</strong>=<em>WINKEL_IN_GRAD</em> [<strong>-t</strong>=<em>WINKEL_IN_GRAD</em>] [<strong>-z</strong>] [<strong>-v</strong>] [<strong>-y</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--location</strong></td> <td>Erhalte Position von IP Adresse</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--latitude</strong>=<em>WINKEL_IN_GRAD</em></td> <td>Setze Geographische Breite (WINKEL_IN_GRAD:0 [≥-90 ≤90 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--longitude</strong>=<em>WINKEL_IN_GRAD</em></td> <td>Setze Geographische Länge (WINKEL_IN_GRAD:0 [≥-180 ≤180 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--twilight</strong>=<em>WINKEL_IN_GRAD</em></td> <td>Setze Dämmerungswinkel: 0:Auf-/Untergang|-6:zivil|-12:nautisch|-18:astronomisch (WINKEL_IN_GRAD:0 [≥-18 ≤18 Δ1])</td> </tr>
</table>

<h3 id="sunrise">Zeige Informationen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-r</strong><em>[=FORMAT]</em> <strong>-m</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--sunrise</strong><em>[=FORMAT]</em></td> <td>Zeige lokale Zeit, benutzte geografische Position, Dämmerungswinkel, Sonnenauf-und untergangszeiten
  <table>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> TEXT</td><td># TEXT</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> JSON</td><td># JSON</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--modes</strong></td> <td>Zeige Weißpunktmodus</td> </tr>
</table>

<h3 id="daemon">Setze Sonnenuntergangsdienst</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-d</strong><em>[=0|1|2]</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--daemon</strong><em>[=0|1|2]</em></td> <td>Setze Sonnenuntergangsdienst
  <table>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> 0</td><td># Ausschalten : Deaktiviere</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> 1</td><td># Automatischer Start : Automatischer Start</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> 2</td><td># Aktiviere : Aktiviere</td></tr>
  </table>
  </td>
 </tr>
</table>


<h2>ALLGEMEINE OPTIONEN <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="export">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor-white-point</strong></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> -</td><td># Vollständige Hilfe : Zeige Hilfe für alle Gruppen</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> synopsis</td><td># Übersicht : Liste Gruppen - Zeige alle Gruppen mit Syntax</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Exportiere formatierten Text<br />Hole Benutzerschnittstelle als Text
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> man</td><td># Handbuch : Unix Handbuchseite - Hole Unix Handbuchseite</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> markdown</td><td># Markdown : Formatierter Text - Hole formatierten Text</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json</td><td># Json : GUI - Hole Oyjl Json Benutzerschnittstelle</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json+command</td><td># Json + Kommando : GUI + Kommando - Hole Oyjl Json Benutzerschnittstelle mit Kommando</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : Alle verfügbaren Daten - Erhalte Daten für Entwickler. Das Format kann mit dem oyjl-args Werkzeug umgewandelt werden.</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Wähle Darstellung<br />Wähle und konfiguriere eine Ausgabeform. -R=gui wird eine grafische Ausgabe starten. -R="web:port=port_nummer:https_key=TLS_privater_Schlüssel_Dateiname:https_cert=TLS_CA_Zertifikat_Dateiname:css=Stil_Dateiname.css" wird einen Web Service starten.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> gui</td><td># Gui : Zeige UI - Zeige eine interaktive grafische Benutzerschnittstelle.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> cli</td><td># Cli : Zeige UI - Zeige Hilfstext für Benutzerschnittstelle auf der Kommandozeile.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Starte Web Server - Starte lokalen Web Service für die Darstellung in einem Webbrowser. Die -R=web:help Unteroption zeigt weitere Informationen an.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-z</strong>|<strong>--system-wide</strong></td> <td>Einstellung in der systemweiten DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--test</strong></td> <td></td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>Plaudernd</td> </tr>
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

 https://gitlab.com/oyranos/oyranos


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann https://gitlab.com/beku

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2024 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Lizenz
newBSD <a href="https://gitlab.com/oyranos/oyranos">https://gitlab.com/oyranos/oyranos</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

