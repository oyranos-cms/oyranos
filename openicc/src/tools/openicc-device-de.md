# openicc\-device v0.1.1 {#openiccdevicede}
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

*"openicc\-device"* *1* *""* "User Commands"
## NAME <a name="name"></a>
openicc\-device v0.1.1 \- OpenICC Device
## ÜBERSICHT <a name="synopsis"></a>
**openicc\-device** <strong>\-l</strong> [<strong>\-d</strong>=<em>NUMMER</em>] [<strong>\-j</strong>] [<strong>\-n</strong>] [<strong>\-b</strong>=<em>DATEINAME</em>] [<strong>\-v</strong>]
<br />
**openicc\-device** <strong>\-a</strong> <strong>\-f</strong>=<em>DATEINAME</em> [<strong>\-b</strong>=<em>DATEINAME</em>] [<strong>\-v</strong>]
<br />
**openicc\-device** <strong>\-e</strong> <strong>\-d</strong>=<em>NUMMER</em> [<strong>\-b</strong>=<em>DATEINAME</em>] [<strong>\-v</strong>]
<br />
**openicc\-device** <strong>\-p</strong> [<strong>\-s</strong>] [<strong>\-v</strong>]
<br />
## BESCHREIBUNG <a name="description"></a>
Bearbeitung von Geräteeinträgen in der Datenbank für OpenICC Farbmanagement
## OPTIONEN <a name="options"></a>
### Zeige die Geräte aus der DB
&nbsp;&nbsp;**openicc\-device** <strong>\-l</strong> [<strong>\-d</strong>=<em>NUMMER</em>] [<strong>\-j</strong>] [<strong>\-n</strong>] [<strong>\-b</strong>=<em>DATEINAME</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-devices</strong></td> <td>Zeige Geräte</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device</strong> <em>NUMMER</em></td> <td>Gerätebeposition (NUMMER:0 [≥0 ≤100])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--dump-json</strong></td> <td>Schreibe OpenICC Json</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--long</strong></td> <td>Zeige alle Schlüssel/Wertepaare</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong> <em>DATEINAME</em></td> <td>DB Dateiname<br />Dateiname der OpenICC Json Gerätedatenbank </tr>
</table>

### Trage Gerät in die DB ein
&nbsp;&nbsp;**openicc-device** <strong>\-a</strong> <strong>\-f</strong>=<em>DATEINAME</em> [<strong>\-b</strong>=<em>DATEINAME</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--add</strong></td> <td>Trage Gerät in die DB ein</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--file-name</strong> <em>DATEINAME</em></td> <td>Dateiname<br />Der Dateiname des OpenICC Gerätes im Json Format. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong> <em>DATEINAME</em></td> <td>DB Dateiname<br />Dateiname der OpenICC Json Gerätedatenbank </tr>
</table>

### Lösche Gerät von der DB
&nbsp;&nbsp;**openicc-device** <strong>\-e</strong> <strong>\-d</strong>=<em>NUMMER</em> [<strong>\-b</strong>=<em>DATEINAME</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--erase-device</strong></td> <td>Lösche Geräte</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device</strong> <em>NUMMER</em></td> <td>Gerätebeposition (NUMMER:0 [≥0 ≤100])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong> <em>DATEINAME</em></td> <td>DB Dateiname<br />Dateiname der OpenICC Json Gerätedatenbank </tr>
</table>

### Zeige den Dateipfad zur DB
&nbsp;&nbsp;**openicc-device** <strong>\-p</strong> [<strong>\-s</strong>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--show-path</strong></td> <td>Zeige Pfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--scope</strong></td> <td>System</td> </tr>
</table>

### Allgemeine Optionen

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong> <em>DATEINAME</em></td> <td>DB Dateiname<br />Dateiname der OpenICC Json Gerätedatenbank </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Hilfe</td> </tr>
</table>

## AUTOR <a name="author"></a>
Kai-Uwe Behrmann http://www.openicc.org
## KOPIERRECHT <a name="copyright"></a>
*Copyright 2018 Kai\-Uwe Behrmann*


### Lizenz <a name="license"></a>
newBSD
## FEHLER <a name="bugs"></a>
[https://www.github.com/OpenICC/config/issues](https://www.github.com/OpenICC/config/issues)

