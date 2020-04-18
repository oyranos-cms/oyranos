# oyjl\-args v1.0.0 {#oyjlargs}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

*"oyjl\-args"* *1* *"June 26, 2019"* "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl\-args v1.0.0 \- Ui to source code

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyjl\-args</strong> <a href="#input"><strong>\-i</strong>=<em>FILENAME</em></a> [<strong>\-v</strong>]
<br />
<strong>oyjl\-args</strong> <a href="#help"><strong>\-h</strong><em>[=synopsis|...]</em></a> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> <strong>\-V</strong> [<strong>\-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

Tool to convert UI JSON description from *\-X export* into source code.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

 <h4 id="input">Generate source code</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl\-args</strong></a> <strong>\-i</strong>=<em>FILENAME</em> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong> <em>FILENAME</em></td> <td>Set Input </tr>
</table>

 <h4 id="help">General options</h4>
&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-args</strong></a> <strong>\-h</strong><em>[=synopsis|...]</em> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> <strong>\-V</strong> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[ synopsis|...]</em></td> <td>Print help text<br />Show usage information and hints for the tool.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h 1</strong></td><td># Full Help</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h synopsis</strong></td><td># Synopsis</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h input</strong></td><td># input</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h export</strong></td><td># export</td></tr>
  </table>
  </td>
 </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>verbose</td> </tr>
</table>


<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

#### Convert eXported developer JSON to C source
&nbsp;&nbsp;oyjl-args \-X export | oyjl\-args \-i \-

<h2>SEE ALSO <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl](oyjl.html)<a href="oyjl.md">(1)</a>&nbsp;&nbsp;[oyjl\-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>&nbsp;&nbsp;[oyjl\-args\-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

&nbsp;&nbsp;<a href="https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html">https://codedocs.xyz/oyranos\-cms/oyranos/group\_\_oyjl.html</a>

<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai\-Uwe Behrmann http://www.oyranos.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright © 2017\-2020 Kai\-Uwe Behrmann*


<a name="license"></a>
### License
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos\-cms/oyranos/issues</a>

