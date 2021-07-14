# oyranos\-policy v0.9.7 {#oyranospolicyde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [ALLGEMEINE OPTIONEN](#general_options) [ENVIRONMENT](#environment) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyranos-policy"</strong> *1* <em>"February 13, 2015"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-policy v0.9.7 - Oyranos ICC Richtlinien Werkzeug

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-policy</strong> <a href="#current-policy"><strong>-c</strong></a> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#list-policies"><strong>-l</strong></a> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#dump"><strong>-d</strong></a> [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#import-policy"><strong>-i</strong>=<em>DATEINAME</em></a> [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#list-paths"><strong>-p</strong></a> [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#save-policy"><strong>-s</strong>=<em>DATEINAME</em></a> [<strong>--system-wide</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-policy</strong> <a href="#long-help"><strong>--long-help</strong></a> | <strong>--docbook</strong> [<strong>--doc-title</strong>=<em>TEXT</em>] [<strong>--doc-version</strong>=<em>TEXT</em>]
<br />
<strong>oyranos-policy</strong> <a href="#export"><strong>-X</strong>=<em>json|json+command|man|markdown</em></a> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das Werkzeug zeigt und liest eine Richtline des Oyranos Farbmanagementsystems.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="current-policy">Aktive Richtlinie</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-c</strong> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--current-policy</strong></td> <td>Zeige den Name der zur Zeit aktiven Richtlinie</td> </tr>
</table>

<h3 id="list-policies">Liste der verfügbaren Richtlinien</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-l</strong> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-policies</strong></td> <td>Liste der verfügbaren Richtlinien</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--file-name</strong></td> <td>Zeige vollen Dateiname mit Pfad.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--internal-name</strong></td> <td>Zeige Anzeigename</td> </tr>
</table>

<h3 id="dump">Die aktuellen Einstellungen ausgeben</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-d</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--dump</strong></td> <td>Die aktuellen Einstellungen als Oyranos XML ausgeben</td> </tr>
</table>

<h3 id="import-policy">Wählen der Richtlinie</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-i</strong>=<em>DATEINAME</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--import-policy</strong>=<em>DATEINAME</em></td> <td>Muss in Oyranos eigenem XML Textformat sein  </td>
 </tr>
</table>

<h3 id="list-paths">Zeige Suchpfade</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-p</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--list-paths</strong></td> <td>Zeige Suchpfade</td> </tr>
</table>

<h3 id="save-policy">Speichern in eine neue Richtlinie</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-s</strong>=<em>DATEINAME</em> [<strong>--system-wide</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--save-policy</strong>=<em>DATEINAME</em></td> <td>Speichere und Installiere in eine neue Richtlinie  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--system-wide</strong></td> <td>Tue es systemweit, benötigt eventuell Adminrechte</td> </tr>
</table>

<h3 id="long-help">Dokumente</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>--long-help</strong> | <strong>--docbook</strong> [<strong>--doc-title</strong>=<em>TEXT</em>] [<strong>--doc-version</strong>=<em>TEXT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--long-help</strong></td> <td>Erzeuge Oyranos Dokumentation<br />HTML Format</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--docbook</strong></td> <td>Erzeuge Oyranos Dokumentation<br />Docbook Format</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--doc-title</strong>=<em>TEXT</em></td> <td>  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--doc-version</strong>=<em>TEXT</em></td> <td>  </td>
 </tr>
</table>


<h2>ALLGEMEINE OPTIONEN <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="export">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-policy</strong></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h -</strong></td><td># Vollständige Hilfe</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h synopsis</strong></td><td># Übersicht</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Exportiere formatierten Text<br />Hole Benutzerschnittstelle als Text
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X man</strong></td><td># Handbuch</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X markdown</strong></td><td># Markdown</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json</strong></td><td># Json</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json+command</strong></td><td># Json + Kommando</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X export</strong></td><td># Export</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Wähle Darstellung<br />Wähle und konfiguriere eine Ausgabeform. -R=gui wird eine grafische Ausgabe starten.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R gui</strong></td><td># Gui</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R cli</strong></td><td># Cli</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R web</strong></td><td># Web</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R -</strong></td><td># </td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>


<h2>ENVIRONMENT <a href="#toc" name="environment">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
#### XDG_CONFIG_HOME
&nbsp;&nbsp;richte Oyranos auf obere Verzeichnisse, welche Resourcen enthalten. An die Pfade für Richtlinien wird "color/settings" angehangen. http://www.openicc.org/index.php%3Ftitle=OpenIccDirectoryProposal.html
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal

<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Die aktuellen Einstellungen ausgeben
&nbsp;&nbsp;oyranos-policy -d
#### Setze in eine neue Richtlinie
&nbsp;&nbsp;oyranos-policy -i policy_dateiname
#### Liste der verfügbaren Richtlinien
&nbsp;&nbsp;oyranos-policy -l
#### Zur Zeit aktive Richtlinie mit Dateiname
&nbsp;&nbsp;oyranos-policy -cfe
#### Speichere und Installiere in eine neue Richtlinie
&nbsp;&nbsp;oyranos-policy -s policy_name
#### Zeige einen Hilfetext an
&nbsp;&nbsp;oyranos-policy -h

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(3)</a>&nbsp;&nbsp;[oyranos-monitor](oyranosmonitor.html)<a href="oyranosmonitor.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2021 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

