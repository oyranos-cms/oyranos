# oyjl\-args v1.0.0 {#oyjlargsde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

*"oyjl\-args"* *1* *"26. Juni 2019"* "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl\-args v1.0.0 \- Benutzerschnittstellen nach Quelltext

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

**oyjl\-args** <strong>\-i</strong>=<em>DATEINAME</em> [<strong>\-v</strong>]
<br />
**oyjl\-args** <strong>\-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> <strong>\-V</strong> [<strong>\-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Werkzeug um Benutzerschnittstellen in JSON von *\-X export* nach Quelltext zu übertragen.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

#### Erzeuge Quelltext
&nbsp;&nbsp;**oyjl\-args** <strong>\-i</strong>=<em>DATEINAME</em> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong> <em>DATEINAME</em></td> <td>Setze Eingabe </tr>
</table>

#### Allgemeine Optionen
&nbsp;&nbsp;**oyjl-args** <strong>\-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> <strong>\-V</strong> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Hilfe</td> </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>


<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Wandle EntwicklerJSON nach C Quelltext
&nbsp;&nbsp;oyjl-args \-X export | oyjl\-args \-i \-

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl](oyjl.html)<a href="oyjl.md">(1)</a>&nbsp;&nbsp;[oyjl\-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>&nbsp;&nbsp;[oyjl\-args\-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

&nbsp;&nbsp;<a href="https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html">https://codedocs.xyz/oyranos\-cms/oyranos/group\_\_oyjl.html</a>

<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai\-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright © 2017\-2020 Kai\-Uwe Behrmann*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos\-cms/oyranos/issues</a>

