# oyjl v1.0.0 {#oyjl}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

*"oyjl"* *1* *"November 12, 2017"* "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl v1.0.0 \- Oyjl Json Manipulation

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

**oyjl** <strong>json</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]
<br />
**oyjl** <strong>yaml</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]
<br />
**oyjl** <strong>xml</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]
<br />
**oyjl** <strong>count</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]
<br />
**oyjl** <strong>key</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]
<br />
**oyjl** <strong>type</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]
<br />
**oyjl** <strong>paths</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]
<br />
**oyjl** <strong>\-h</strong><em>[=synopsis|...]</em> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The oyjl program can be used to parse, filter sub trees, select values and modify values in JSON texts.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

#### Set input file and path

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong> <em>FILENAME</em></td> <td>File or Stream<br />A JSON file name or a input stream like "stdin". </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xpath</strong> <em>PATH</em></td> <td>Path specifier<br />The path consists of slash '/' separated terms. Each term can be a key name or a square bracketed index. A empty term is used for a search inside a tree. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--set</strong> <em>STRING</em></td> <td>Set a key name to a value </tr>
</table>

#### Print JSON to stdout
&nbsp;&nbsp;**oyjl** <strong>json</strong> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>json</strong></td> <td>Print JSON to stdout</td> </tr>
</table>

#### Print YAML to stdout
&nbsp;&nbsp;**oyjl** <strong>yaml</strong> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>yaml</strong></td> <td>Print YAML to stdout</td> </tr>
</table>

#### Print XML to stdout
&nbsp;&nbsp;**oyjl** <strong>xml</strong> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>xml</strong></td> <td>Print XML to stdout</td> </tr>
</table>

#### Print node count
&nbsp;&nbsp;**oyjl** <strong>count</strong> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>count</strong></td> <td>Print count of leafs in node</td> </tr>
</table>

#### Print key name
&nbsp;&nbsp;**oyjl** <strong>key</strong> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>key</strong></td> <td>Print key name of node</td> </tr>
</table>

#### Print type
&nbsp;&nbsp;**oyjl** <strong>type</strong> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>type</strong></td> <td>Get node type</td> </tr>
</table>

#### Print all matching paths.
&nbsp;&nbsp;**oyjl** <strong>paths</strong> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>paths</strong></td> <td>Print all matching paths</td> </tr>
</table>

#### General options
&nbsp;&nbsp;**oyjl** <strong>-h</strong><em>[=synopsis|...]</em> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[ synopsis|...]</em></td> <td>Print help text<br />Show usage information and hints for the tool.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h 1</strong></td><td># Full Help</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h synopsis</strong></td><td># Synopsis</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h json</strong></td><td># JSON</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h yaml</strong></td><td># YAML</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h xml</strong></td><td># XML</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h count</strong></td><td># Count</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h key</strong></td><td># Key Name</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h type</strong></td><td># Type</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h export</strong></td><td># export</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
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

#### Print JSON to stdout
&nbsp;&nbsp;oyjl -i text.json \-x ///[0]
#### Print count of leafs in node
&nbsp;&nbsp;oyjl \-c \-i text.json \-x my/path/
#### Print key name of node
&nbsp;&nbsp;oyjl \-k \-i text.json \-x ///[0]
#### Print all matching paths
&nbsp;&nbsp;oyjl \-p \-i text.json \-x //
#### Set a key name to a value
&nbsp;&nbsp;oyjl \-i text.json \-x my/path/to/key \-s value

<h2>SEE ALSO <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl\-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl\-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>&nbsp;&nbsp;[oyjl\-args\-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

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

