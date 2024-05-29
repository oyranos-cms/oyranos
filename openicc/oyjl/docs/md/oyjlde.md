# oyjl v1.0.0 {#oyjlde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyjl"</strong> *1* <em>"12. November 2017"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl v1.0.0 - Oyjl Json Bearbeitung

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyjl</strong> <a href="#json"><strong>json</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#yaml"><strong>yaml</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#xml"><strong>xml</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#csv"><strong>csv</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#csv-semicolon"><strong>csv-semicolon</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#count"><strong>count</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#key"><strong>key</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#type"><strong>type</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#paths"><strong>paths</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#format"><strong>format</strong></a> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das oyjl Programm unterstützt Einlesen, Filtern von Untermengen, Wertauswahl und Wertveränderung in JSON, YAML, XML und CSV. Es übersetzt zwischen Datenformaten.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3>Setze Eingangsdateiname und Pfad</h3>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong>=<em>DATEINAME</em> ...</td> <td>Datei oder Datenstrom<br />Ein JSON-Dateiname oder Eingangsdatenstrom wie "stdin" </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xpath</strong>=<em>PFAD</em></td> <td>Pfadangabe<br />Der Pfad besteht aus Namen, welche durch Schrägstrich '/' getrennt sind. Jeder Name kann ein Schlüsselname sein oder ein Index in eckigen Klammern. Ein leerer Name wird für die Suche innerhalb eines Baumes benutzt.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--set</strong>=<em>TEXT</em></td> <td>Setze Wert des "xpath" Schlüssels.<br />Benutze "oyjl-remove" zum entfernen des Schlüssels.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--plain</strong></td> <td>Keine Textformatierung</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--try-format</strong>=<em>FORMAT</em></td> <td>Versuche ein Datenformat zu finden, auch mit Versatz.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> JSON</td><td># JSON</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> XML</td><td># XML</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> CSV</td><td># CSV</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> CSV-semicolon</td><td># CSV-semicolon</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--detect-numbers</strong><em>[=TRENNER]</em></td> <td>Versuche Zahlen in nicht typensicheren Formaten zu erkennen.<br />Voreingestellt ist der Punkt '.' als Ganzzahlentrenner.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> ,</td><td># Komma : Ganzzahlentrenner</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> .</td><td># Punkt : Ganzzahlentrenner</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--wrap</strong>=<em>TYP</em></td> <td>sprachspezifische Formatierung
  <table>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> C</td><td># C statischer Text</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-W</strong>|<strong>--wrap-name</strong>=<em>NAME</em></td> <td>Ein Name für das zu definierende Symbol.<br />Benutze nur Buchstaben aus den Alphabet [A-Z,a-z] einschließlich möglichem Understrich '_'.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-W</strong> wrap</td><td># wrap</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="json">Zeige JSON auf stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>json</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;JSON - JavaScript Object Notation

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>json</strong></td> <td>Zeige JSON auf stdout</td> </tr>
</table>

<h3 id="yaml">Zeige YAML auf stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>yaml</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;YAML - Yet Another Markup Language

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>yaml</strong></td> <td>Zeige YAML auf stdout</td> </tr>
</table>

<h3 id="xml">Zeige XML auf stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>xml</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;XML - eXtended Markup Language

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>xml</strong></td> <td>Zeige XML auf stdout</td> </tr>
</table>

<h3 id="csv">Zeige CSV auf stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>csv</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;CSV - Comma Separated Values

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>csv</strong></td> <td>Zeige CSV auf stdout</td> </tr>
</table>

<h3 id="csv-semicolon">Zeige CSV-Semikolon auf stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>csv-semicolon</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=TRENNER]</em>] [<strong>-w</strong>=<em>TYP</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;CSV - Comma Separated Values

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>csv-semicolon</strong></td> <td>Zeige CSV mit Semikolon auf stdout</td> </tr>
</table>

<h3 id="count">Zeige Knotenanzahl</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>count</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>count</strong></td> <td>Zeige Anzahl der Knotenzweige</td> </tr>
</table>

<h3 id="key">Zeige Schlüsselname</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>key</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>key</strong></td> <td>Zeige Schlüsselname des Knotens</td> </tr>
</table>

<h3 id="type">Zeige Typ</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>type</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>type</strong></td> <td>Hole Knotentyp</td> </tr>
</table>

<h3 id="paths">Zeige alle zutreffenden Pfade</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>paths</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>paths</strong></td> <td>Zeige alle zutreffenden Pfade</td> </tr>
</table>

<h3 id="format">Zeige Datenformat.</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>format</strong> [<strong>-i</strong>=<em>DATEINAME</em> ...] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>format</strong></td> <td>Zeige Datenformat</td> </tr>
</table>

<h3 id="help">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> 1</td><td># Vollständige Hilfe : Zeige Hilfe für alle Gruppen</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> synopsis</td><td># Übersicht : Liste Gruppen - Zeige alle Gruppen mit Syntax</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Eingabe</td><td># Setze Eingangsdateiname und Pfad</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Zeige JSON</td><td># Zeige JSON auf stdout : JSON - JavaScript Object Notation</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Zeige YAML</td><td># Zeige YAML auf stdout : YAML - Yet Another Markup Language</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Zeige XML</td><td># Zeige XML auf stdout : XML - eXtended Markup Language</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Zeige CSV</td><td># Zeige CSV auf stdout : CSV - Comma Separated Values</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Zeige CSV-Semikolon</td><td># Zeige CSV-Semikolon auf stdout : CSV - Comma Separated Values</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Anzahl</td><td># Zeige Knotenanzahl</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Schlüsselname</td><td># Zeige Schlüsselname</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Typ</td><td># Zeige Typ</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Pfade</td><td># Zeige alle zutreffenden Pfade</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Format</td><td># Zeige Datenformat.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Verschiedenes</td><td># Allgemeine Optionen</td></tr>
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

#### Zeige JSON auf stdout
&nbsp;&nbsp;oyjl -i text.json -x ///[0]
#### Zeige Anzahl der Knotenzweige
&nbsp;&nbsp;oyjl -c -i text.json -x mein/Pfad/
#### Zeige Schlüsselname des Knotens
&nbsp;&nbsp;oyjl -k -i text.json -x ///[0]
#### Zeige alle zutreffenden Pfade
&nbsp;&nbsp;oyjl -p -i text.json -x //
#### Setze Wert des Schlüssels
&nbsp;&nbsp;oyjl -i text.json -x mein/Pfad/zum/Schlüssel -s Wert

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>&nbsp;&nbsp;[oyjl-args-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

&nbsp;&nbsp;<a href="https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html">https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html</a>

<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann 

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright © 2017-2024 Kai-Uwe Behrmann*


<a name="license"></a>
### Lizenz
MIT <a href="https://opensource.org/license/mit">https://opensource.org/license/mit</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/beku/oyjl/issues">https://www.gitlab.com/beku/oyjl/issues</a>

