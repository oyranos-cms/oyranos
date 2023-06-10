# openicc\-device v0.1.1 {#openiccdevicede}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"openicc-device"</strong> *1* <em>""</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

openicc-device v0.1.1 - OpenICC Device

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>openicc-device</strong> <a href="#list-devices"><strong>-l</strong></a> [<strong>-d</strong>=<em>NUMMER</em>] [<strong>-j</strong>] [<strong>-n</strong>] [<strong>-b</strong>=<em>DATEINAME</em>] [<strong>-v</strong>]
<br />
<strong>openicc-device</strong> <a href="#add"><strong>-a</strong></a> <strong>-f</strong>=<em>DATEINAME</em> [<strong>-b</strong>=<em>DATEINAME</em>] [<strong>-v</strong>]
<br />
<strong>openicc-device</strong> <a href="#erase-device"><strong>-e</strong></a> <strong>-d</strong>=<em>NUMMER</em> [<strong>-b</strong>=<em>DATEINAME</em>] [<strong>-v</strong>]
<br />
<strong>openicc-device</strong> <a href="#show-path"><strong>-p</strong></a> [<strong>-s</strong>] [<strong>-v</strong>]
<br />
<strong>openicc-device</strong> <a href="#help"><strong>-h</strong></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Bearbeitung von Geräteeinträgen in der Datenbank für OpenICC Farbmanagement

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="list-devices">Zeige die Geräte aus der DB</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-l</strong> [<strong>-d</strong>=<em>NUMMER</em>] [<strong>-j</strong>] [<strong>-n</strong>] [<strong>-b</strong>=<em>DATEINAME</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-devices</strong></td> <td>Zeige Geräte</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device</strong>=<em>NUMMER</em></td> <td>Gerätebeposition (NUMMER:0 [≥0 ≤100 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--dump-json</strong></td> <td>Schreibe OpenICC Json</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--long</strong></td> <td>Zeige alle Schlüssel/Wertepaare</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong>=<em>DATEINAME</em></td> <td>DB Dateiname<br />Dateiname der OpenICC Json Gerätedatenbank  </td>
 </tr>
</table>

<h3 id="add">Trage Gerät in die DB ein</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-a</strong> <strong>-f</strong>=<em>DATEINAME</em> [<strong>-b</strong>=<em>DATEINAME</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--add</strong></td> <td>Trage Gerät in die DB ein</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--file-name</strong>=<em>DATEINAME</em></td> <td>Dateiname<br />Der Dateiname des OpenICC Gerätes im Json Format.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong>=<em>DATEINAME</em></td> <td>DB Dateiname<br />Dateiname der OpenICC Json Gerätedatenbank  </td>
 </tr>
</table>

<h3 id="erase-device">Lösche Gerät von der DB</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-e</strong> <strong>-d</strong>=<em>NUMMER</em> [<strong>-b</strong>=<em>DATEINAME</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--erase-device</strong></td> <td>Lösche Geräte</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device</strong>=<em>NUMMER</em></td> <td>Gerätebeposition (NUMMER:0 [≥0 ≤100 Δ1])</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong>=<em>DATEINAME</em></td> <td>DB Dateiname<br />Dateiname der OpenICC Json Gerätedatenbank  </td>
 </tr>
</table>

<h3 id="show-path">Zeige den Dateipfad zur DB</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-p</strong> [<strong>-s</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--show-path</strong></td> <td>Zeige Pfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--system</strong></td> <td>Lokales System</td> </tr>
</table>

<h3 id="help">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>openicc-device</strong></a> <strong>-h</strong> <strong>-X</strong>=<em>json|json+command|man|markdown</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-b</strong>|<strong>--db-file</strong>=<em>DATEINAME</em></td> <td>DB Dateiname<br />Dateiname der OpenICC Json Gerätedatenbank  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Exportiere formatierten Text<br />Hole Benutzerschnittstelle als Text
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> man</td><td># Handbuch : Unix Handbuchseite - Hole Unix Handbuchseite</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> markdown</td><td># Markdown : Formatierter Text - Hole formatierten Text</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json</td><td># Json : GUI - Hole Oyjl Json Benutzerschnittstelle</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json+command</td><td># Json + Kommando : GUI + Kommando - Hole Oyjl Json Benutzerschnittstelle mit Kommando</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : Alle verfügbaren Daten - Get UI data for developers. The format can be converted by the oyjl-args tool.</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Hilfe</td> </tr>
</table>


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.openicc.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright 2018 Kai-Uwe Behrmann*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.openicc.org">http://www.openicc.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/OpenICC/config/issues">https://www.github.com/OpenICC/config/issues</a>

