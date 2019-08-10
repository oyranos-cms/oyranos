# oyranos\-policy  {#oyranospolicy}
[NAME](#name) [SYNOPSIS](#synopsis) [DESCRIPTION](#description) [OPTIONS](#options) [ENVIRONMENT](#environment) [EXAMPLES](#examples) [SEE ALSO](#seealso) [AUTHOR](#author) [COPYRIGHT](#copyright) [BUGS](#bugs) 

*"oyranos\-policy"* *1* *"February 13, 2015"* "User Commands"
## NAME <a name="name"></a>
oyranos\-policy  \- Oyranos CMS policy tool
## SYNOPSIS <a name="synopsis"></a>
**oyranos\-policy** <strong>\-c</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-policy** <strong>\-l</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-policy** <strong>\-d</strong> [<strong>\-v</strong>]
<br />
**oyranos\-policy** <strong>\-i</strong>=<em>FILENAME</em> [<strong>\-v</strong>]
<br />
**oyranos\-policy** <strong>\-p</strong> [<strong>\-v</strong>]
<br />
**oyranos\-policy** <strong>\-s</strong>=<em>FILENAME</em> [<strong>\-\-system\-wide</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-policy** <strong>\-h</strong>
## DESCRIPTION <a name="description"></a>
The tool dumps out / read a configuration of the Oyranos color management system (CMS).
## OPTIONS <a name="options"></a>
&nbsp;&nbsp;**oyranos\-policy** <strong>\-c</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong></td> <td>show current active policy name</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong></td> <td>show full filename including path.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong></td> <td>show display name.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong></td> <td></td> </tr>
</table>

&nbsp;&nbsp;**oyranos-policy** <strong>\-l</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong></td> <td>list available policies</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong></td> <td>show full filename including path.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong></td> <td>show display name.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong></td> <td></td> </tr>
</table>

&nbsp;&nbsp;**oyranos-policy** <strong>\-d</strong> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong></td> <td>dump out all settings in Oyranos' own xml-ish text format</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong></td> <td></td> </tr>
</table>

&nbsp;&nbsp;**oyranos-policy** <strong>\-i</strong>=<em>FILENAME</em> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong> <em>FILENAME</em></td> <td><br />import; FILENAME must be in Oyranos' own xml-ish text format </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong></td> <td></td> </tr>
</table>

&nbsp;&nbsp;**oyranos-policy** <strong>\-p</strong> [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong></td> <td>list search paths</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong></td> <td></td> </tr>
</table>

&nbsp;&nbsp;**oyranos-policy** <strong>\-s</strong>=<em>FILENAME</em> [<strong>\-\-system\-wide</strong>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong> <em>FILENAME</em></td> <td><br />Save and install to a new policy; FILENAME must be in Oyranos' own xml-ish text format </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--system-wide</strong></td> <td>do system wide, might need admin or root privileges</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong></td> <td></td> </tr>
</table>

&nbsp;&nbsp;**oyranos-policy** <strong>\-h</strong>

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong></td> <td>help text</td> </tr>
</table>

## ENVIRONMENT <a name="environment"></a>
### OY\_DEBUG
&nbsp;&nbsp;set the Oyranos debug level. Alternatively the -v option can be used.
### XDG\_CONFIG\_HOME
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for policies have a "color/settings" appended.
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
## EXAMPLES <a name="examples"></a>
### Dump out the actual settings:
&nbsp;&nbsp;oyranos\-policy \-d
### Set new policy
&nbsp;&nbsp;oyranos\-policy \-i policy\_filename
### List available policies
&nbsp;&nbsp;oyranos\-policy \-l
### Currently active policy including its file name
&nbsp;&nbsp;oyranos\-policy \-cfe
### Save and install to a new policy
&nbsp;&nbsp;oyranos\-policy \-s policy\_name
### Print a help text
&nbsp;&nbsp;oyranos\-policy \-h
## SEE ALSO <a name="seealso"></a>
###  [oyranos\-config](oyranosconfig.html)<a href="oyranosconfig.md">(3)</a>  [oyranos\-monitor](oyranosmonitor.html)<a href="oyranosmonitor.md">(1)</a>  [oyranos](oyranos.html)<a href="oyranos.md">(3)</a>
### http://www.oyranos.org
## AUTHOR <a name="author"></a>
Kai\-Uwe Behrmann (ku.b (at) gmx.de) 
## COPYRIGHT <a name="copyright"></a>
*(c) 2005\-2015, Kai\-Uwe Behrmann*


### License <a name="license"></a>
new BSD <http://www.opensource.org/licenses/BSD\-3\-Clause>
## BUGS <a name="bugs"></a>
at: [https://github.com/oyranos\-cms/oyranos/issues](https://github.com/oyranos\-cms/oyranos/issues)

