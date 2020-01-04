# oyjl\-args\-qml v1.0 {#oyjlargsqml}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [EXAMPLES](#examples) [SEE ALSO](#seealso) 

*"oyjl\-args\-qml"* *1* *"May 23, 2019"* "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl\-args\-qml v1.0 \- Oyjl Args QML Viewer

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

**oyjl\-args\-qml** <strong>\-i</strong>=<em>STRING</em> [<strong>\-c</strong>=<em>STRING</em>] [<strong>\-o</strong>=<em>STRING</em>] [<strong>\-v</strong>]
<br />
**oyjl\-args\-qml** <strong>\-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The tool graphicaly renders Oyjl JSON UI files using QML.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

### QML UI
&nbsp;&nbsp;**oyjl\-args\-qml** <strong>\-i</strong>=<em>STRING</em> [<strong>\-c</strong>=<em>STRING</em>] [<strong>\-o</strong>=<em>STRING</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong> <em>STRING</em></td> <td>JSON UI Description </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--command</strong> <em>STRING</em></td> <td>JSON Command </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong> <em>STRING</em></td> <td>Results JSON </tr>
</table>

### General options
&nbsp;&nbsp;**oyjl-args\-qml** <strong>\-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Help</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>verbose</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong> <em>json|json+command|man|markdown</em></td> <td>Export formated text<br />Get UI converted into text formats
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X man</strong></td><td># Man</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X markdown</strong></td><td># Markdown</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json</strong></td><td># Json</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json+command</strong></td><td># Json + Command</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X export</strong></td><td># Export</td></tr>
  </table>
  </td>
 </tr>
</table>


<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

### Load a UI JSON declaration from file
&nbsp;&nbsp;oyjl-args\-qml \-i oyjl\-ui\-text.json \-c oyjl\-command.json
### Load a UI JSON declaration from tool
&nbsp;&nbsp;oyjl \-X json+command | oyjl\-args\-qml \-i \- \-c +

<h2>SEE ALSO <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl](oyjl.html)<a href="oyjl.md">(1)</a>&nbsp;&nbsp;[oyjl\-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl\-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>

