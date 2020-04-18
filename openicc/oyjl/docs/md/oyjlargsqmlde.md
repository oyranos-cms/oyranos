# oyjl\-args\-qml v1.0 {#oyjlargsqmlde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) 

*"oyjl\-args\-qml"* *1* *"May 23, 2019"* "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl\-args\-qml v1.0 \- Oyjl Args QML Viewer

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

**oyjl\-args\-qml** <strong>\-i</strong>=<em>STRING</em> [<strong>\-c</strong>=<em>STRING</em>] [<strong>\-o</strong>=<em>STRING</em>] [<strong>\-v</strong>]
<br />
**oyjl\-args\-qml** <strong>\-h</strong>=<em>|synopsis|...</em> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

The tool graphicaly renders Oyjl JSON UI files using QML.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

#### QML UI
&nbsp;&nbsp;**oyjl\-args\-qml** <strong>\-i</strong>=<em>STRING</em> [<strong>\-c</strong>=<em>STRING</em>] [<strong>\-o</strong>=<em>STRING</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong> <em>STRING</em></td> <td>JSON UI Description </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--command</strong> <em>STRING</em></td> <td>JSON Command </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong> <em>STRING</em></td> <td>Results JSON </tr>
</table>

#### General options
&nbsp;&nbsp;**oyjl-args\-qml** <strong>\-h</strong>=<em>|synopsis|...</em> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong> <em>|synopsis|...</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h 1</strong></td><td># Vollständige Hilfe</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h synopsis</strong></td><td># Übersicht</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h input</strong></td><td># JSON UI Description</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h export</strong></td><td># exportiere</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>verbose</td> </tr>
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

#### Load a UI JSON declaration from file
&nbsp;&nbsp;oyjl-args\-qml \-i oyjl\-ui\-text.json \-c oyjl\-command.json
#### Load a UI JSON declaration from tool
&nbsp;&nbsp;oyjl \-X json+command | oyjl\-args\-qml \-i \- \-c +

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl](oyjl.html)<a href="oyjl.md">(1)</a>&nbsp;&nbsp;[oyjl\-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl\-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>


