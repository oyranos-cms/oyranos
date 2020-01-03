# oyjl v1.0 {#oyjl}
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

*"oyjl"* *1* *"November 12, 2017"* "User Commands"

<a name="name"></a>
## NAME
oyjl v1.0 \- Oyjl Json Manipulation

<a name="synopsis"></a>
## SYNOPSIS
**oyjl** <strong>\-j</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]
<br />
**oyjl** <strong>\-y</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]
<br />
**oyjl** <strong>\-m</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]
<br />
**oyjl** <strong>\-c</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]
<br />
**oyjl** <strong>\-k</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]
<br />
**oyjl** <strong>\-t</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]
<br />
**oyjl** <strong>\-p</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]
<br />
**oyjl** <strong>\-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<a name="description"></a>
## DESCRIPTION
The oyjl program can be used to parse, filter sub trees, select values and modify values in JSON texts.

<a name="options"></a>
## OPTIONS
### Set input file and pah

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong> <em>FILENAME</em></td> <td>File or Stream<br />A JSON file name or a input stream like "stdin". </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>|<strong>--xpath</strong> <em>PATH</em></td> <td>Path specifier<br />The path consists of slash '/' separated terms. Each term can be a key name or a square bracketed index. A empty term is used for a search inside a tree. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--set</strong> <em>STRING</em></td> <td>Set a key name to a value </tr>
</table>

### Print JSON to stdout
&nbsp;&nbsp;**oyjl** <strong>-j</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--json</strong></td> <td>Print JSON to stdout</td> </tr>
</table>

### Print YAML to stdout
&nbsp;&nbsp;**oyjl** <strong>-y</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--yaml</strong></td> <td>Print YAML to stdout</td> </tr>
</table>

### Print XML to stdout
&nbsp;&nbsp;**oyjl** <strong>-m</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>] [<strong>\-s</strong>=<em>STRING</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--xml</strong></td> <td>Print XML to stdout</td> </tr>
</table>

### Print node count
&nbsp;&nbsp;**oyjl** <strong>-c</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--count</strong></td> <td>Print count of leafs in node</td> </tr>
</table>

### Print key name
&nbsp;&nbsp;**oyjl** <strong>-k</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--key</strong></td> <td>Print key name of node</td> </tr>
</table>

### Print type
&nbsp;&nbsp;**oyjl** <strong>-t</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--type</strong></td> <td>Get node type</td> </tr>
</table>

### Print all matching paths.
&nbsp;&nbsp;**oyjl** <strong>-p</strong> [<strong>\-i</strong>=<em>FILENAME</em>] [<strong>\-x</strong>=<em>PATH</em>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--paths</strong></td> <td>Print all matching paths</td> </tr>
</table>

### General options
&nbsp;&nbsp;**oyjl** <strong>-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Help</td> </tr>
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


<a name="examples"></a>
## EXAMPLES
### Print JSON to stdout
&nbsp;&nbsp;oyjl -i text.json \-x ///[0]
### Print count of leafs in node
&nbsp;&nbsp;oyjl \-c \-i text.json \-x my/path/
### Print key name of node
&nbsp;&nbsp;oyjl \-k \-i text.json \-x ///[0]
### Print all matching paths
&nbsp;&nbsp;oyjl \-p \-i text.json \-x //
### Set a key name to a value
&nbsp;&nbsp;oyjl \-i text.json \-x my/path/to/key \-s value

<a name="seealso"></a>
## SEE ALSO
&nbsp;&nbsp;[oyjl\-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl\-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>&nbsp;&nbsp;[oyjl\-args\-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

&nbsp;&nbsp;<a href="https://codedocs.xyz/oyranos\-cms/oyranos/group\_\_oyjl.html">https://codedocs.xyz/oyranos\-cms/oyranos/group\_\_oyjl.html</a>

<a name="author"></a>
## AUTHOR
Kai\-Uwe Behrmann (ku.b (at) gmx.de) 

<a name="copyright"></a>
## COPYRIGHT
*(c) 2017, Kai\-Uwe Behrmann and others*


<a name="license"></a>
### License
MIT <a href="http://www.opensource.org/licenses/MIT">http://www.opensource.org/licenses/MIT</a>

<a name="bugs"></a>
## BUGS
 [https://github.com/oyranos\-cms/oyranos/issues](https://github.com/oyranos\-cms/oyranos/issues)


<a href="#name">Top</a>
