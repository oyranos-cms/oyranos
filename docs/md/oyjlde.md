# oyjl v1.0 {#oyjlde}
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

*"oyjl"* *1* *"12. November 2017"* "User Commands"

<a name="name"></a>
## NAME
oyjl v1.0 \- Oyjl Json Bearbeitung

<a name="synopsis"></a>
## ÜBERSICHT
**oyjl** <strong>\-j</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>] [<strong>\-s</strong>=<em>TEXT</em>]
<br />
**oyjl** <strong>\-y</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>] [<strong>\-s</strong>=<em>TEXT</em>]
<br />
**oyjl** <strong>\-m</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>] [<strong>\-s</strong>=<em>TEXT</em>]
<br />
**oyjl** <strong>\-c</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>]
<br />
**oyjl** <strong>\-k</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>]
<br />
**oyjl** <strong>\-t</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>]
<br />
**oyjl** <strong>\-p</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>]
<br />
**oyjl** <strong>\-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<a name="description"></a>
## BESCHREIBUNG
Das oyjl Programm unterstütz Einlesen, Filtern von Untermengen, Wertauswahl und Wertveränderung in einem JSON Text.

<a name="options"></a>
## OPTIONEN
### Setze Eingangsdateiname und Pfad

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong> <em>DATEINAME</em></td> <td>Datei oder Datenstrom<br />Ein JSON-Dateiname oder Eingangsdatenstrom wie "stdin" </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xpath</strong> <em>PFAD</em></td> <td>Pfadangabe<br />Der Pfad besteht aus Namen, welche durch Schrägstrich '/' getrennt sind. Jeder Name kann ein Schlüsselname sein oder ein Index in eckigen Klammern. Ein leerer Name wird für die Suche innerhalb eines Baumes benutzt. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--set</strong> <em>TEXT</em></td> <td>Setze Wert des Schlüssels </tr>
</table>

### Zeige JSON auf stdout
&nbsp;&nbsp;**oyjl** <strong>-j</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>] [<strong>\-s</strong>=<em>TEXT</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--json</strong></td> <td>Zeige JSON auf stdout</td> </tr>
</table>

### Zeige YAML auf stdout
&nbsp;&nbsp;**oyjl** <strong>-y</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>] [<strong>\-s</strong>=<em>TEXT</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--yaml</strong></td> <td>Zeige YAML auf stdout</td> </tr>
</table>

### Zeige XML auf stdout
&nbsp;&nbsp;**oyjl** <strong>-m</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>] [<strong>\-s</strong>=<em>TEXT</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--xml</strong></td> <td>Zeige XML auf stdout</td> </tr>
</table>

### Zeige Knotenanzahl
&nbsp;&nbsp;**oyjl** <strong>-c</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--count</strong></td> <td>Zeige Anzahl der Knotenzweige</td> </tr>
</table>

### Zeige Schlüsselname
&nbsp;&nbsp;**oyjl** <strong>-k</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--key</strong></td> <td>Zeige Schlüsselname des Knotens</td> </tr>
</table>

### Zeige Typ
&nbsp;&nbsp;**oyjl** <strong>-t</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--type</strong></td> <td>Hole Knotentyp</td> </tr>
</table>

### Zeige alle zutreffenden Pfade
&nbsp;&nbsp;**oyjl** <strong>-p</strong> [<strong>\-i</strong>=<em>DATEINAME</em>] [<strong>\-x</strong>=<em>PFAD</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--paths</strong></td> <td>Zeige alle zutreffenden Pfade</td> </tr>
</table>

### Allgemeine Optionen
&nbsp;&nbsp;**oyjl** <strong>-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Hilfe</td> </tr>
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


<a name="examples"></a>
## BEISPIELE
### Zeige JSON auf stdout
&nbsp;&nbsp;oyjl -i text.json \-x ///[0]
### Zeige Anzahl der Knotenzweige
&nbsp;&nbsp;oyjl \-c \-i text.json \-x mein/Pfad/
### Zeige Schlüsselname des Knotens
&nbsp;&nbsp;oyjl \-k \-i text.json \-x ///[0]
### Zeige alle zutreffenden Pfade
&nbsp;&nbsp;oyjl \-p \-i text.json \-x //
### Setze Wert des Schlüssels
&nbsp;&nbsp;oyjl \-i text.json \-x mein/Pfad/zum/Schlüssel \-s Wert

<a name="seealso"></a>
## SIEHE AUCH
&nbsp;&nbsp;[oyjl\-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl\-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>&nbsp;&nbsp;[oyjl\-args\-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

&nbsp;&nbsp;[https://codedocs.xyz/oyranos\-cms/oyranos/group\_\_oyjl.html](https://codedocs.xyz/oyranos\-cms/oyranos/group\_\_oyjl.html)

<a name="author"></a>
## AUTOR
Kai\-Uwe Behrmann (ku.b (at) gmx.de) 

<a name="copyright"></a>
## KOPIERRECHT
*(c) 2017, Kai\-Uwe Behrmann und Andere*


<a name="license"></a>
### Lizenz
MIT <http://www.opensource.org/licenses/MIT>

<a name="bugs"></a>
## FEHLER
 [https://github.com/oyranos\-cms/oyranos/issues](https://github.com/oyranos\-cms/oyranos/issues)

