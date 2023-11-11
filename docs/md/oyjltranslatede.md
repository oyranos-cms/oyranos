# oyjl\-translate v1.0.0 {#oyjltranslatede}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [BEISPIELE](#examples) [UMGEBUNGSVARIABLEN](#environmentvariables) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyjl-translate"</strong> *1* <em>"2. Januar 2020"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl-translate v1.0.0 - Oyjl Übersetzung

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyjl-translate</strong> <a href="#extract"><strong>-e</strong></a> <strong>-k</strong>=<em>name,description,help...</em> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-o</strong>=<em>DATEINAME</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>] [<strong>--function-name</strong>=<em>NAME</em>] [<strong>--function-name-out</strong>=<em>NAME</em>]
<br />
<strong>oyjl-translate</strong> <a href="#add"><strong>-a</strong></a> <strong>-d</strong>=<em>TEXTKATALOG1,TEXTKATALOG2</em> <strong>-k</strong>=<em>name,description,help...</em> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-o</strong>=<em>DATEINAME</em>] [<strong>-l</strong>=<em>de_DE,es_ES</em>] [<strong>-p</strong>=<em>LOCALEPFAD</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-t</strong>] [<strong>-n</strong>] [<strong>-v</strong>]
<br />
<strong>oyjl-translate</strong> <a href="#copy"><strong>-c</strong></a> <strong>--locale</strong>=<em>de_DE</em> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-o</strong>=<em>DATEINAME</em>] [<strong>-n</strong>] [<strong>-v</strong>]
<br />
<strong>oyjl-translate</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-V</strong> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Wandle Oyjl UI JSON nach C übersetzbare Texte für gettext Werkzeuge und übersetze die UI eines Werkzeuges mit der programmeigenen ID. Das resultierende Oyjl UI JSON kann für eine übersetzte Darstellung benutzt werden.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3>Setze grundlegende Parameter</h3>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong>=<em>DATEINAME</em></td> <td>Datei oder Datenstrom<br />Ein JSON-Dateiname oder Eingangsdatenstrom wie "stdin"  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>DATEINAME</em></td> <td>Datei oder Datenstrom<br />Ein JSON-Dateiname oder Ausgangsdatenstrom wie "stdout"  </td>
 </tr>
</table>

<h3 id="extract">Wandle JSON/C in gettext lesbare C Texte</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-translate</strong></a> <strong>-e</strong> <strong>-k</strong>=<em>name,description,help...</em> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-o</strong>=<em>DATEINAME</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>] [<strong>--function-name</strong>=<em>NAME</em>] [<strong>--function-name-out</strong>=<em>NAME</em>]

&nbsp;&nbsp;Zwei Eingabearten werden unterstützt. Lese Oyjl UI -X=export Json. Oder suche in C Quelltext nach --function-name definierten Name und ersetze ihn in --output durch --function-name-out. Die letztere Art ist hilfreich für oyjlTranslation_s, Qt Stil oder andere Übersetzungsmethoden.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--extract</strong></td> <td>Auszug übersetzbarer Nachrichten<br />Wandle JSON in gettext lesbare C Texte</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong>=<em>FORMAT</em></td> <td>Formatierungstext<br />Ausgabeformat  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--key-list</strong>=<em>name,description,help...</em></td> <td>Schlüsselnamenliste<br />zu benutzende Schlüsselwörter in einer kommagetrennten Liste  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--function-name</strong>=<em>NAME</em></td> <td>Funktion Name<br />Ein eingabeseitiger Funktionsname. z.B.: "i18n(\""
  <table>
   <tr><td style='padding-left:0.5em'><strong>--function-name</strong> _(\"</td>
   <tr><td style='padding-left:0.5em'><strong>--function-name</strong> i18n(\"</td>
   <tr><td style='padding-left:0.5em'><strong>--function-name</strong> QObject::tr(\"</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--function-name-out</strong>=<em>NAME</em></td> <td>Funktion Name<br />Ein ausgabeseitiger Funktionsname. z.B.: "_"
  <table>
   <tr><td style='padding-left:0.5em'><strong>--function-name-out</strong> _(\"</td>
   <tr><td style='padding-left:0.5em'><strong>--function-name-out</strong> i18n(\"</td>
   <tr><td style='padding-left:0.5em'><strong>--function-name-out</strong> QObject::tr(\"</td>
  </table>
  </td>
 </tr>
</table>

<h3 id="add">Füge mit gettext übersetzte Schlüssel zu JSON hinzu</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-translate</strong></a> <strong>-a</strong> <strong>-d</strong>=<em>TEXTKATALOG1,TEXTKATALOG2</em> <strong>-k</strong>=<em>name,description,help...</em> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-o</strong>=<em>DATEINAME</em>] [<strong>-l</strong>=<em>de_DE,es_ES</em>] [<strong>-p</strong>=<em>LOCALEPFAD</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-t</strong>] [<strong>-n</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--add</strong></td> <td>Füge Übersetzung hinzu<br />Füge mit gettext übersetzte Schlüssel zu JSON hinzu</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--domain</strong>=<em>TEXTKATALOG1,TEXTKATALOG2</em></td> <td>Textkatalogliste<br />Text Hausadressliste für das Projekt  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--locales</strong>=<em>de_DE,es_ES</em></td> <td>Sprachliste<br />Sprachen in einer kommagetrennten Liste  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--localedir</strong>=<em>LOCALEPFAD</em></td> <td>Übersetzungsverzeichnis<br />locale Verzeichnis mit deine-locale/LC_MESSAGES/deine-textdomain.mo gettext Übersetzungen  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--key-list</strong>=<em>name,description,help...</em></td> <td>Schlüsselnamenliste<br />zu benutzende Schlüsselwörter in einer kommagetrennten Liste  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--wrap</strong>=<em>TYP</em></td> <td>sprachspezifische Formatierung
  <table>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> C</td><td># C statischer Text</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--translations-only</strong></td> <td>Nur Übersetzungen<br />gebe nur Übersetzungen aus</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--list-empty</strong></td> <td>Nicht übersetzte Liste<br />liste auch leere Übersetzungen auf</td> </tr>
</table>

<h3 id="copy">Kopiere Schlüssel nach JSON</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-translate</strong></a> <strong>-c</strong> <strong>--locale</strong>=<em>de_DE</em> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-o</strong>=<em>DATEINAME</em>] [<strong>-n</strong>] [<strong>-v</strong>]

&nbsp;&nbsp;Importiere Übersetzungen aus anderen Formaten ohne gettext. Unterstützt ist --input=Qt-xml-format.tr

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--copy</strong></td> <td>Kopiere Übersetzungen<br />Kopiere übersetzte Schlüssel nach JSON. Lasse gettext aus.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--locale</strong>=<em>de_DE</em></td> <td>Einzelne Sprache  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--list-empty</strong></td> <td>Nicht übersetzte Liste<br />liste auch leere Übersetzungen auf</td> </tr>
</table>

<h3 id="help">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-translate</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-V</strong> [<strong>-v</strong>]

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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>mehr Infos</td> </tr>
</table>


<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Wandle JSON in gettext lesbare C Texte
&nbsp;&nbsp;oyjl-translate -e [-v] -i oyjl-ui.json -o ergebnis.json -f '_("%s"); ' -k name,description,help
#### Füge mit gettext übersetzte Schlüssel zu JSON hinzu
&nbsp;&nbsp;oyjl-translate -a -i oyjl-ui.json -o ergebnis.json -k name,description,help -d TEXTKATALOG -p SPRACHPFAD -l de_DE,es_ES
#### Kopiere übersetzte Schlüssel nach JSON. Lasse gettext aus.
&nbsp;&nbsp;oyjl-translate -c -i sprache.tr -o ergebnis.json --locale de_DE
#### Betrachte Unix Handbuchseite
&nbsp;&nbsp;oyjl-translate -X man | groff -T utf8 -man -

<h2>UMGEBUNGSVARIABLEN <a href="#toc" name="environmentvariables">&uarr;</a></h2>

#### OUTPUT_CHARSET
&nbsp;&nbsp;Setze GNU gettext Textformatierung.
  <br />
&nbsp;&nbsp;Alternativ kann die -l=de_DE.UTF-8 Option genutzt werden.
  <br />
&nbsp;&nbsp;Ein typischer Wert ist UTF-8.

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl](oyjl.html)<a href="oyjl.md">(1)</a>&nbsp;&nbsp;[oyjl-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl-args-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

&nbsp;&nbsp;<a href="https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html">https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html</a>

<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright © 2017-2022 Kai-Uwe Behrmann*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

