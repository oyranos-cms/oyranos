# oyjl v1.0.0 {#oyjlde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyjl"</strong> *1* <em>"12. November 2017"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl v1.0.0 - Oyjl Json Bearbeitung

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyjl</strong> <a href="#json"><strong>json</strong></a> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>]
<br />
<strong>oyjl</strong> <a href="#yaml"><strong>yaml</strong></a> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>]
<br />
<strong>oyjl</strong> <a href="#xml"><strong>xml</strong></a> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>]
<br />
<strong>oyjl</strong> <a href="#count"><strong>count</strong></a> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>]
<br />
<strong>oyjl</strong> <a href="#key"><strong>key</strong></a> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>]
<br />
<strong>oyjl</strong> <a href="#type"><strong>type</strong></a> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>]
<br />
<strong>oyjl</strong> <a href="#paths"><strong>paths</strong></a> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>]
<br />
<strong>oyjl</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das oyjl Programm unterstütz Einlesen, Filtern von Untermengen, Wertauswahl und Wertveränderung in einem JSON Text.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

 <h4>Setze Eingangsdateiname und Pfad</h4>

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong> <em>DATEINAME</em></td> <td>Datei oder Datenstrom<br />Ein JSON-Dateiname oder Eingangsdatenstrom wie "stdin" </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xpath</strong> <em>PFAD</em></td> <td>Pfadangabe<br />Der Pfad besteht aus Namen, welche durch Schrägstrich '/' getrennt sind. Jeder Name kann ein Schlüsselname sein oder ein Index in eckigen Klammern. Ein leerer Name wird für die Suche innerhalb eines Baumes benutzt. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--set</strong> <em>TEXT</em></td> <td>Setze Wert des Schlüssels </tr>
</table>

 <h4 id="json">Zeige JSON auf stdout</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>json</strong> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>json</strong></td> <td>Zeige JSON auf stdout</td> </tr>
</table>

 <h4 id="yaml">Zeige YAML auf stdout</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>yaml</strong> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>yaml</strong></td> <td>Zeige YAML auf stdout</td> </tr>
</table>

 <h4 id="xml">Zeige XML auf stdout</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>xml</strong> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>] [<strong>-s</strong>=<em>TEXT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>xml</strong></td> <td>Zeige XML auf stdout</td> </tr>
</table>

 <h4 id="count">Zeige Knotenanzahl</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>count</strong> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>count</strong></td> <td>Zeige Anzahl der Knotenzweige</td> </tr>
</table>

 <h4 id="key">Zeige Schlüsselname</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>key</strong> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>key</strong></td> <td>Zeige Schlüsselname des Knotens</td> </tr>
</table>

 <h4 id="type">Zeige Typ</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>type</strong> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>type</strong></td> <td>Hole Knotentyp</td> </tr>
</table>

 <h4 id="paths">Zeige alle zutreffenden Pfade</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>paths</strong> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-x</strong>=<em>PFAD</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>paths</strong></td> <td>Zeige alle zutreffenden Pfade</td> </tr>
</table>

 <h4 id="help">Allgemeine Optionen</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> <strong>-X</strong>=<em>json|json+command|man|markdown</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[ synopsis|...]</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h 1</strong></td><td># Vollständige Hilfe</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h synopsis</strong></td><td># Übersicht</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h json</strong></td><td># JSON</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h yaml</strong></td><td># YAML</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h xml</strong></td><td># XML</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h count</strong></td><td># Anzahl</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h key</strong></td><td># Schlüsselname</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h type</strong></td><td># Typ</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h export</strong></td><td># exportiere</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>mehr Infos</td> </tr>
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

Kai-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright © 2017-2020 Kai-Uwe Behrmann*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

