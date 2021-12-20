# oyjl\-args\-qml v1.0 {#oyjlargsqmlde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) 

<strong>"oyjl-args-qml"</strong> *1* <em>"15. November 2020"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl-args-qml v1.0 - Oyjl Args QML Betrachter

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyjl-args-qml</strong> <a href="#input"><strong>-i</strong>=<em>TEXT</em></a> [<strong>-c</strong>=<em>TEXT</em>] [<strong>-o</strong>=<em>TEXT</em>] [<strong>-v</strong>]
<br />
<strong>oyjl-args-qml</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-V</strong> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das Werkzeug zeigt Oyjl JSON UI Dateien grafisch an in QML.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="input">QML Benutzeroberfläche</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-args-qml</strong></a> <strong>-i</strong>=<em>TEXT</em> [<strong>-c</strong>=<em>TEXT</em>] [<strong>-o</strong>=<em>TEXT</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong>=<em>TEXT</em></td> <td>JSON Benutzerschnittstellen Beschreibung<br />Das Dateiformat ist ein JSON UI von einem Werkzeug mittels libOyjlCore mit der -X export Option.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--command</strong>=<em>TEXT</em></td> <td>JSON Kommando<br />Unterstützt wird 1) ein Wert von '+' als Hinweis, daß ein Kommando bereits in UI JSON entahlten ist. Das Kommando kann 2) als JSON Text eingegeben werden. Z.B. "{"command_set":"my-cli-program"}". Oder 3) wird einfach ein Programmname angegeben.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>TEXT</em></td> <td>Ergebnis JSON  </td>
 </tr>
</table>

<h3 id="help">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-args-qml</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-V</strong> [<strong>-v</strong>]

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
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : Alle verfügbaren Daten - Erhalte Daten für Entwickler</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>


<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Lese UI JSON Beschreibung von Datei
&nbsp;&nbsp;oyjl-args-qml -i oyjl-ui-text.json -c oyjl-command.json
#### Lese UI JSON Beschreibung von Werkzeug
&nbsp;&nbsp;oyjl -X json+command | oyjl-args-qml -i - -c +

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl](oyjl.html)<a href="oyjl.md">(1)</a>&nbsp;&nbsp;[oyjl-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>


