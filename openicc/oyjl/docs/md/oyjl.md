# oyjl v1.0.0 {#oyjl}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyjl"</strong> *1* <em>"November 12, 2017"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl v1.0.0 - Oyjl Json Manipulation

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyjl</strong> <a href="#json"><strong>json</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#yaml"><strong>yaml</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#xml"><strong>xml</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#csv"><strong>csv</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#csv-semicolon"><strong>csv-semicolon</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]
<br />
<strong>oyjl</strong> <a href="#count"><strong>count</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#key"><strong>key</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#type"><strong>type</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#paths"><strong>paths</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#format"><strong>format</strong></a> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]
<br />
<strong>oyjl</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

The oyjl program can be used to parse, filter sub trees, select values and modify values in JSON texts.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3>Set input file and path</h3>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong>=<em>FILENAME</em> ...</td> <td>File or Stream<br />A JSON file name or a input stream like "stdin". </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xpath</strong>=<em>PATH</em></td> <td>Path specifier<br />The path consists of slash '/' separated terms. Each term can be a key name or a square bracketed index. A empty term is used for a search inside a tree.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--set</strong>=<em>STRING</em></td> <td>Set a key name to a value  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--plain</strong></td> <td>No Markup</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--try-format</strong>=<em>FORMAT</em></td> <td>Try to find data format, even with offset.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> JSON</td><td># JSON</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> CSV</td><td># CSV</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-r</strong> CSV-semicolon</td><td># CSV-semicolon</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--detect-numbers</strong><em>[=SEPARATOR]</em></td> <td>Try to detect numbers from non typesafe formats.<br />Uses by default dot '.' as decimal separator.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> ,</td><td># Comma : Decimal Separator</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d</strong> .</td><td># Dot : Decimal Separator</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--wrap</strong>=<em>TYPE</em></td> <td>language specific wrap
  <table>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> C</td><td># C static char</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-W</strong>|<strong>--wrap-name</strong>=<em>NAME</em></td> <td>A name for the symbol to be defined.<br />Use only letters from alphabet [A-Z,a-z] including optional underscore '_'.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-W</strong> wrap</td><td># wrap</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="json">Print JSON to stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>json</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;JSON - JavaScript Object Notation

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>json</strong></td> <td>Print JSON to stdout</td> </tr>
</table>

<h3 id="yaml">Print YAML to stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>yaml</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;YAML - Yet Another Markup Language

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>yaml</strong></td> <td>Print YAML to stdout</td> </tr>
</table>

<h3 id="xml">Print XML to stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>xml</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;XML - eXtended Markup Language

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>xml</strong></td> <td>Print XML to stdout</td> </tr>
</table>

<h3 id="csv">Print CSV to stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>csv</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;CSV - Comma Separated Values

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>csv</strong></td> <td>Print CSV to stdout</td> </tr>
</table>

<h3 id="csv-semicolon">Print CSV-semicolon to stdout</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>csv-semicolon</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-s</strong>=<em>STRING</em>] [<strong>-r</strong>=<em>FORMAT</em>] [<strong>-p</strong>] [<strong>-d</strong><em>[=SEPARATOR]</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-W</strong>=<em>NAME</em>]

&nbsp;&nbsp;CSV - Comma Separated Values

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>csv-semicolon</strong></td> <td>Print CSV with semicolon to stdout</td> </tr>
</table>

<h3 id="count">Print node count</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>count</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>count</strong></td> <td>Print count of leafs in node</td> </tr>
</table>

<h3 id="key">Print key name</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>key</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>key</strong></td> <td>Print key name of node</td> </tr>
</table>

<h3 id="type">Print type</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>type</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>type</strong></td> <td>Get node type</td> </tr>
</table>

<h3 id="paths">Print all matching paths.</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>paths</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>paths</strong></td> <td>Print all matching paths</td> </tr>
</table>

<h3 id="format">Print Data Format.</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>format</strong> [<strong>-i</strong>=<em>FILENAME</em> ...] [<strong>-x</strong>=<em>PATH</em>] [<strong>-r</strong>=<em>FORMAT</em>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>format</strong></td> <td>Print Data Format</td> </tr>
</table>

<h3 id="help">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Print help text<br />Show usage information and hints for the tool.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> 1</td><td># Full Help : Print help for all groups</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> synopsis</td><td># Synopsis : List groups - Show all groups including syntax</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Input</td><td># Set input file and path</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Print JSON</td><td># Print JSON to stdout : JSON - JavaScript Object Notation</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Print YAML</td><td># Print YAML to stdout : YAML - Yet Another Markup Language</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Print XML</td><td># Print XML to stdout : XML - eXtended Markup Language</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Print CSV</td><td># Print CSV to stdout : CSV - Comma Separated Values</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Print CSV-semicolon</td><td># Print CSV-semicolon to stdout : CSV - Comma Separated Values</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Count</td><td># Print node count</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Key Name</td><td># Print key name</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Type</td><td># Print type</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Paths</td><td># Print all matching paths.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Format</td><td># Print Data Format.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> Misc</td><td># General options</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Export formated text<br />Get UI converted into text formats
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> man</td><td># Man : Unix Man page - Get a unix man page</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> markdown</td><td># Markdown : Formated text - Get formated text</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json</td><td># Json : GUI - Get a Oyjl Json UI declaration</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json+command</td><td># Json + Command : GUI + Command - Get Oyjl Json UI declaration incuding command</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : All available data - Get UI data for developers. The format can be converted by the oyjl-args tool.</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
</table>


<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

#### Print JSON to stdout
&nbsp;&nbsp;oyjl -i text.json -x ///[0]
#### Print count of leafs in node
&nbsp;&nbsp;oyjl -c -i text.json -x my/path/
#### Print key name of node
&nbsp;&nbsp;oyjl -k -i text.json -x ///[0]
#### Print all matching paths
&nbsp;&nbsp;oyjl -p -i text.json -x //
#### Set a key name to a value
&nbsp;&nbsp;oyjl -i text.json -x my/path/to/key -s value

<h2>SEE ALSO <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>&nbsp;&nbsp;[oyjl-args-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

&nbsp;&nbsp;<a href="https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html">https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html</a>

<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright © 2017-2022 Kai-Uwe Behrmann*


<a name="license"></a>
### License
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

