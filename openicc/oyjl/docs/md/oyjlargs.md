# oyjl\-args v1.0.0 {#oyjlargs}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [EXAMPLES](#examples) [SEE ALSO](#seealso) [FORMAT](#format) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyjl-args"</strong> *1* <em>"March 10, 2023"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl-args v1.0.0 - Ui to source code

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyjl-args</strong> <a href="#input"><strong>-i</strong>=<em>FILENAME</em></a> [<strong>--c-stand-alone</strong><em>[=base]</em>] [<strong>--completion-bash</strong>] [<strong>-v</strong>]
<br />
<strong>oyjl-args</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-R</strong>=<em>gui|cli|web|...</em> <strong>-V</strong> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

Tool to convert UI JSON description from *-X export* into source code.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="input">Generate source code</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-args</strong></a> <strong>-i</strong>=<em>FILENAME</em> [<strong>--c-stand-alone</strong><em>[=base]</em>] [<strong>--completion-bash</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong>=<em>FILENAME</em></td> <td>Set Input<br />For C code output (default) and --completion-bash output use -X=export JSON. For --render=XXX use -X=json JSON. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--c-stand-alone</strong><em>[=base]</em></td> <td>Generate C code for oyjl_args.c inclusion.<br />Omit libOyjlCore reference.
  <table>
   <tr><td style='padding-left:0.5em'><strong>--c-stand-alone</strong> base</td><td># Base : Use OAJL_ARGS_BASE API only.</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--completion-bash</strong></td> <td>Generate bash completion code</td> </tr>
</table>

<h3 id="help">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-args</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-R</strong>=<em>gui|cli|web|...</em> <strong>-V</strong> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>-v</strong>]

&nbsp;&nbsp;The -R option can in parts accept additional sub arguments to the base argument. For an overview use the help subargument oyjl-args -R=web:help.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Print help text<br />Show usage information and hints for the tool.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> -</td><td># Full Help : Print help for all groups</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> synopsis</td><td># Synopsis : List groups - Show all groups including syntax</td></tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Select Renderer<br />Select and possibly configure Renderer. -R="gui" will just launch a graphical UI. -R="web:port=port_number:https_key=TLS_private_key_filename:https_cert=TLS_CA_certificate_filename:css=layout_filename.css" will launch a local Web Server, which listens on local port.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> gui</td><td># Gui : Show UI - Display a interactive graphical User Interface.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> cli</td><td># Cli : Show UI - Print on Command Line Interface.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Start Web Server - Start a local Web Service to connect a Webbrowser with. Use the -R=web:help sub option to see more information.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>verbose</td> </tr>
</table>


<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

#### Convert eXported developer JSON to C source
&nbsp;&nbsp;oyjl-args -X export | oyjl-args -i -

<h2>SEE ALSO <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl](oyjl.html)<a href="oyjl.md">(1)</a>&nbsp;&nbsp;[oyjl-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>&nbsp;&nbsp;[oyjl-args-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

&nbsp;&nbsp;<a href="https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html">https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html</a>

<h2>FORMAT <a href="#toc" name="format">&uarr;</a></h2>

#### Synopsis
&nbsp;&nbsp;Tools follows syntax rules.
  <br />
&nbsp;&nbsp;OyjlArgs requires, checks and enforces rules. These rules are expressed in each synopsis line.
  <br />
&nbsp;&nbsp;A tool can provide different rule sets, expressed in different synopsis lines.
#### prog -o
&nbsp;&nbsp;Simple tool with only one option.
  <br />
&nbsp;&nbsp;The option consists of one single letter and thus starts with a single dash.
  <br />
&nbsp;&nbsp;The command line tool is in the following examples called "prog" for simplicity.
#### prog --option
&nbsp;&nbsp;Simple tool with only one option.
  <br />
&nbsp;&nbsp;The option consists of more than one letter starting with two dashs.
  <br />
&nbsp;&nbsp;This is called a long option name. The same option can be triggered by the single letter name or the long option name. Inside the Synopsis line only one form is noticed.
#### prog -o=ARG --name=one|two|...
&nbsp;&nbsp;Simple tool with two options, which both accept arguments.
  <br />
&nbsp;&nbsp;The argument can be representet by a big letter content hint, like FILE, NUMBER etc. Or it is a collection of pipe separated choices.
  <br />
&nbsp;&nbsp;The later --name option names a few choices and shows with the immediately following three dots, that the choices are not exclusive and might be edited. OyjlArgs checks for args following the option name even without the equal sign '='.
#### prog -o [-v]
&nbsp;&nbsp;Tool with two differently required options.
  <br />
&nbsp;&nbsp;By default all options are required like the -o one and is mandatory. The second option is enclosed in squared brackets is not required but might be used and thus is optional. 
#### prog -h[=synopsis|...] [--option[=NUMBER]]
&nbsp;&nbsp;Tool options, which might be follwed by an argument.
#### prog -f=FILE ... [-i=FILE ...]
&nbsp;&nbsp;Tool options with three dots after empty space ' ...' can occure multiple times.
  <br />
&nbsp;&nbsp;Command line example: prog -f=file1.ext -f=file2.ext -f file3.ext
#### prog | [-v]
&nbsp;&nbsp;Tool without option requirement.
  <br />
&nbsp;&nbsp;The tool can be called without any option. But one optional option might occure.
#### prog sub -o [-i] [-v]
&nbsp;&nbsp;Tool with sub tool option syntax.
  <br />
&nbsp;&nbsp;The tool has one long mandatory option name without leading dashes.
  <br />
&nbsp;&nbsp;This style is used sometimes for one complex tool for multiple connected tasks. The sub tool sections help in separating the different tool areas.
#### prog [-v] FILE ...
&nbsp;&nbsp;Tool with default option free style arguments.
  <br />
&nbsp;&nbsp;The @ option argument(s) are mentioned as last in order to not confuse with sub tool options or with option arguments.
#### Option syntax
&nbsp;&nbsp;The options are described each individually in more detail.
  <br />
&nbsp;&nbsp;One letter option name and long name forms are show separated by the pipe symbol '|'.
  <br />
&nbsp;&nbsp;E.g. -o|--option
#### -k|--kelvin=NUMBER        Lambert (NUMBER:0 [≥0 ≤25000 Δ100])
&nbsp;&nbsp;Line for a number argument.
  <br />
&nbsp;&nbsp;The single letter and long option names are noticed and followed by the number symbolic name. After that the short name of the option is printed. After the opening brace is the symbolic name repated, followed by the default value. In square brackets follow boundaries ≥ minimal value, ≤ maximal value and Δ the step or tick.
#### Command line parser
&nbsp;&nbsp;The OyjlArgs command line parser follows the above rules.
#### prog -hvi=file.ext
&nbsp;&nbsp;Options can be concatenated on the command line.
  <br />
&nbsp;&nbsp;The OyjlArgs parser takes each letter after a single dash as a separated option.
  <br />
&nbsp;&nbsp;The last option can have a argument.
#### prog -i=file-in.ext -o file-out.ext
&nbsp;&nbsp;Arguments for options can be written with equal sign or with empty space.
#### prog -i=file1.ext -i file2.ext -i file3.ext
&nbsp;&nbsp;Multiple arguments for one option need each one option in front.

<h2>AUTHOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>COPYRIGHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright © 2017-2022 Kai-Uwe Behrmann*


<a name="license"></a>
### License
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>BUGS <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

