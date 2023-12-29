# oyjl\-translate v1.0.0 {#oyjltranslate}
<a name="toc"></a>
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [EXAMPLES](#examples) [ENVIRONMENT VARIABLES](#environmentvariables) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

<strong>"oyjl-translate"</strong> *1* <em>"January 2, 2020"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl-translate v1.0.0 - Oyjl Translation

<h2>SYNOPSIS <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyjl-translate</strong> <a href="#extract"><strong>-e</strong></a> <strong>-k</strong>=<em>name,description,help...</em> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>-o</strong>=<em>FILENAME</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>] [<strong>--function-name</strong>=<em>NAME</em>] [<strong>--function-name-out</strong>=<em>NAME</em>]
<br />
<strong>oyjl-translate</strong> <a href="#add"><strong>-a</strong></a> <strong>-d</strong>=<em>TEXTDOMAIN1,TEXTDOMAIN2</em> <strong>-k</strong>=<em>name,description,help...</em> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>-o</strong>=<em>FILENAME</em>] [<strong>-l</strong>=<em>de_DE,es_ES</em>] [<strong>-p</strong>=<em>LOCALEDIR</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-t</strong>] [<strong>-n</strong>] [<strong>-v</strong>]
<br />
<strong>oyjl-translate</strong> <a href="#copy"><strong>-c</strong></a> <strong>--locale</strong>=<em>de_DE</em> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>-o</strong>=<em>FILENAME</em>] [<strong>-n</strong>] [<strong>-v</strong>]
<br />
<strong>oyjl-translate</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-V</strong> [<strong>-v</strong>]

<h2>DESCRIPTION <a href="#toc" name="description">&uarr;</a></h2>

Convert Oyjl UI JSON to C translatable strings for use with gettext tools and translate a tools UI using the programs own text domain. The resulting Oyjl UI JSON can be used for translated rendering.

<h2>OPTIONS <a href="#toc" name="options">&uarr;</a></h2>

<h3>Set basic parameters</h3>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong>=<em>FILENAME</em></td> <td>File or Stream<br />A JSON file name or a input stream like "stdin".  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>FILENAME</em></td> <td>File or Stream<br />A JSON file name or a output stream like "stdout".  </td>
 </tr>
</table>

<h3 id="extract">Convert JSON/C to gettext ready C strings</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-translate</strong></a> <strong>-e</strong> <strong>-k</strong>=<em>name,description,help...</em> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>-o</strong>=<em>FILENAME</em>] [<strong>-f</strong>=<em>FORMAT</em>] [<strong>-v</strong>] [<strong>--function-name</strong>=<em>NAME</em>] [<strong>--function-name-out</strong>=<em>NAME</em>]

&nbsp;&nbsp;Two input modes are supported. Read Oyjl UI -X=export JSON. Or parse C sources to --function-name defined strings and replace them in --output by --function-name-out. The later is useful for oyjlTranslation_s, Qt style or other translations.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--extract</strong></td> <td>Extract translatable Messages<br />Convert JSON to gettext ready C strings</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong>=<em>FORMAT</em></td> <td>Format string<br />A output format string containing a %s for replacement.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-f</strong> json</td><td># I18N JSON : Create translation Json with -af=json. - This option is useful only for smaller projects as a initial start.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f</strong> i18n(\"%s\");</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--key-list</strong>=<em>name,description,help...</em></td> <td>Key Name List<br />to be used key names in a comma separated list  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--function-name</strong>=<em>NAME</em></td> <td>Function Name<br />A input function name string. e.g.: "i18n(\""
  <table>
   <tr><td style='padding-left:0.5em'><strong>--function-name</strong> _(\"</td>
   <tr><td style='padding-left:0.5em'><strong>--function-name</strong> i18n(\"</td>
   <tr><td style='padding-left:0.5em'><strong>--function-name</strong> QObject::tr(\"</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--function-name-out</strong>=<em>NAME</em></td> <td>Function Name<br />A output funtion name string. e.g.: "_"
  <table>
   <tr><td style='padding-left:0.5em'><strong>--function-name-out</strong> _(\"</td>
   <tr><td style='padding-left:0.5em'><strong>--function-name-out</strong> i18n(\"</td>
   <tr><td style='padding-left:0.5em'><strong>--function-name-out</strong> QObject::tr(\"</td>
  </table>
  </td>
 </tr>
</table>

<h3 id="add">Add gettext translated keys to JSON</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-translate</strong></a> <strong>-a</strong> <strong>-d</strong>=<em>TEXTDOMAIN1,TEXTDOMAIN2</em> <strong>-k</strong>=<em>name,description,help...</em> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>-o</strong>=<em>FILENAME</em>] [<strong>-l</strong>=<em>de_DE,es_ES</em>] [<strong>-p</strong>=<em>LOCALEDIR</em>] [<strong>-w</strong>=<em>TYPE</em>] [<strong>-t</strong>] [<strong>-n</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--add</strong></td> <td>Add Translation<br />Add gettext translated keys to JSON</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--domain</strong>=<em>TEXTDOMAIN1,TEXTDOMAIN2</em></td> <td>Text Domain List<br />text domain list of your project  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--locales</strong>=<em>de_DE,es_ES</em></td> <td>Locales List<br />locales in a comma separated list  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--localedir</strong>=<em>LOCALEDIR</em></td> <td>Locale Directory<br />locale directory containing the your-locale/LC_MESSAGES/your-textdomain.mo gettext translations  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--key-list</strong>=<em>name,description,help...</em></td> <td>Key Name List<br />to be used key names in a comma separated list  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--wrap</strong>=<em>TYPE</em></td> <td>language specific wrap
  <table>
   <tr><td style='padding-left:0.5em'><strong>-w</strong> C</td><td># C static char</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--translations-only</strong></td> <td>Only Translations<br />output only translations</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--list-empty</strong></td> <td>List not translated<br />list empty translations too</td> </tr>
</table>

<h3 id="copy">Copy keys to JSON</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-translate</strong></a> <strong>-c</strong> <strong>--locale</strong>=<em>de_DE</em> [<strong>-i</strong>=<em>FILENAME</em>] [<strong>-o</strong>=<em>FILENAME</em>] [<strong>-n</strong>] [<strong>-v</strong>]

&nbsp;&nbsp;Import translations from other formats without gettext. Supported --input=Qt-xml-format.tr

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--copy</strong></td> <td>Copy Translations<br />Copy translated keys to JSON. Skip gettext.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--locale</strong>=<em>de_DE</em></td> <td>Single Locale  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--list-empty</strong></td> <td>List not translated<br />list empty translations too</td> </tr>
</table>

<h3 id="help">General options</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-translate</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-V</strong> [<strong>-v</strong>]

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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>increase verbosity</td> </tr>
</table>


<h2>EXAMPLES <a href="#toc" name="examples">&uarr;</a></h2>

#### Convert JSON to gettext ready C strings
&nbsp;&nbsp;oyjl-translate -e [-v] -i oyjl-ui.json -o result.json -f '_("%s"); ' -k name,description,help
#### Convert C source to I18N JSON
&nbsp;&nbsp;oyjl-translate -e -f=json -i oyjl-ui.c -o result.json
#### Add gettext translated keys to JSON
&nbsp;&nbsp;oyjl-translate -a -i oyjl-ui.json -o result.json -k name,description,help -d TEXTDOMAIN -p LOCALEDIR -l de_DE,es_ES
#### Copy translated keys to JSON. Skip gettext.
&nbsp;&nbsp;oyjl-translate -c -i lang.tr -o result.json --locale de_DE
#### View MAN page
&nbsp;&nbsp;oyjl-translate -X man | groff -T utf8 -man -

<h2>ENVIRONMENT VARIABLES <a href="#toc" name="environmentvariables">&uarr;</a></h2>

#### OUTPUT_CHARSET
&nbsp;&nbsp;Set the GNU gettext output encoding.
  <br />
&nbsp;&nbsp;Alternatively use the -l=de_DE.UTF-8 option.
  <br />
&nbsp;&nbsp;Typical value is UTF-8.

<h2>SEE ALSO <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl](oyjl.html)<a href="oyjl.md">(1)</a>&nbsp;&nbsp;[oyjl-args](oyjlargs.html)<a href="oyjlargs.md">(1)</a>&nbsp;&nbsp;[oyjl-args-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

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

