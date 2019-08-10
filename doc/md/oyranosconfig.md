# oyranos\-config  {#oyranosconfig}
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [BUGS](#bugs) 

*"oyranos\-config"* *1* *"Noveber 14, 2017"* "User Commands"
## NAME <a name="name"></a>
oyranos\-config  \- Oyranos CMS configuration DB access
## SYNOPSIS <a name="synopsis"></a>
**oyranos\-config** [<strong>\-g</strong>=<em>KEY</em>] [<strong>\-s</strong>=<em>KEY:VALUE</em>] [<strong>\-l</strong>=<em></em>] [<strong>\-p</strong>=<em></em>] [<strong>\-\-version</strong>=<em></em>] [<strong>\-\-api\-version</strong>=<em></em>] [<strong>\-\-num\-version</strong>=<em></em>] [<strong>\-\-cflags</strong>=<em></em>] [<strong>\-\-ldflags</strong>=<em></em>] [<strong>\-\-ldstaticflags</strong>=<em></em>]
## DESCRIPTION <a name="description"></a>
The command line tool allows to access the persistent settings. Additionally it reports some basic static informations about the Oyranos color management system (CMS), like version numbers, compile flags and directories.
## OPTIONS <a name="options"></a>

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong> <em>KEY</em></td> <td>get a persistent DB value </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong> <em>KEY:VALUE</em></td> <td><br />set a persistent DB value </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong> <em></em></td> <td>list existing persistent DB keys </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong> <em></em></td> <td>show the file path to the DB </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--version</strong> <em></em></td> <td>--api-version<br />get the release version of Oyranos CMS </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--api-version</strong> <em></em></td> <td>--num-version<br />get the actual API version </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--num-version</strong> <em></em></td> <td>--cflags<br />get the release version of Oyranos CMS in numerical form for convenience </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--cflags</strong> <em></em></td> <td>compile flags for a C programm<br />compile flags for a C programm </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--ldflags</strong> <em></em></td> <td>--ldstaticflags<br />linker flags for a C programm to link against the Oyranos library </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--ldstaticflags</strong> <em></em></td> <td>linker flags for a C programm to link statically against the Oyranos library<br />linker flags for a C programm to link statically against the Oyranos library </tr>
</table>

## EXAMPLES <a name="examples"></a>
### Show a settings value
&nbsp;&nbsp;oyranos-config \-g org/freedesktop/openicc/behaviour/effect\_switch
### Change a setting
&nbsp;&nbsp;oyranos\-config \-s org/freedesktop/openicc/behaviour/effect\_switch:1
### Log setting changes
&nbsp;&nbsp;oyranos\-config \-\-watch \-v > log\-file.txt
### Compile a simple programm
&nbsp;&nbsp;cc \`oyranos\-config \-\-cflags\` myFile.c \`oyranos\-config \-\-ldflags\` \-o myProg
### Show system wide visible profiles from the Oyranos installation path
&nbsp;&nbsp;ls \`oyranos\-config \-\-syscolordir\`/\`oyranos\-config \-\-iccdirname\`
## SEE ALSO <a name="seealso"></a>
###  [oyranos\-config\-fltk](oyranosconfigfltk.html)<a href="oyranosconfigfltk.md">(1)</a>  [oyranos\-config\-synnefo](oyranosconfigsynnefo.html)<a href="oyranosconfigsynnefo.md">(1)</a>  [oyranos\-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>  [oyranos\-monitor](oyranosmonitor.html)<a href="oyranosmonitor.md">(1)</a>  [oyranos](oyranos.html)<a href="oyranos.md">(3)</a>
## AUTHOR <a name="author"></a>
Kai\-Uwe Behrmann (ku.b (at) gmx.de) 
## BUGS <a name="bugs"></a>
at: [https://github.com/oyranos\-cms/oyranos/issues](https://github.com/oyranos\-cms/oyranos/issues)

