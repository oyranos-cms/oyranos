# oyranos\-device v0.9.7 {#oyranosdevicede}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [ALLGEMEINE OPTIONEN](#general_options) [ENVIRONMENT](#environment) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyranos-device"</strong> *1* <em>"May 13, 2021"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-device v0.9.7 - Oyranos Farbgerät

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-device</strong> <a href="#assign"><strong>-a</strong></a> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em> <strong>-p</strong>=<em>ICC_DATEINAME</em> [<strong>--system-wide</strong>]
<br />
<strong>oyranos-device</strong> <a href="#erase"><strong>-e</strong></a> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em> [<strong>--system-wide</strong>]
<br />
<strong>oyranos-device</strong> <a href="#setup"><strong>-s</strong></a> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em>
<br />
<strong>oyranos-device</strong> <a href="#unset"><strong>-u</strong></a> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em>
<br />
<strong>oyranos-device</strong> <a href="#list"><strong>-l</strong></a> [<strong>-v</strong>] [<strong>--short</strong>] [<strong>--print-class</strong>]
<br />
<strong>oyranos-device</strong> <a href="#list"><strong>-l</strong></a> [<strong>-c</strong>=<em>KLASSE</em>] [<strong>-d</strong>=<em>NUMMER</em>|<strong>--device-name</strong>=<em>NAME</em>] [<strong>--short</strong>] [<strong>--path</strong>] [<strong>-r</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-device</strong> <a href="#list-profiles"><strong>--list-profiles</strong></a> [<strong>-c</strong>=<em>KLASSE</em>] [<strong>-d</strong>=<em>NUMMER</em>] [<strong>--show-non-device-related</strong>]
<br />
<strong>oyranos-device</strong> <a href="#list-taxi-profiles"><strong>--list-taxi-profiles</strong></a> [<strong>-c</strong>=<em>KLASSE</em>] [<strong>-d</strong>=<em>NUMMER</em>] [<strong>--show-non-device-related</strong>]
<br />
<strong>oyranos-device</strong> <a href="#format"><strong>-f</strong>=<em>icc|openicc+rank-map|openicc|openicc-rank-map</em></a> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em> <strong>-j</strong>=<em>DATEINAME</em> <strong>-k</strong>=<em>DATEINAME</em> [<strong>-o</strong>=<em>DATEINAME</em>] [<strong>--only-db</strong>] [<strong>-m</strong>]
<br />
<strong>oyranos-device</strong> <a href="#help"><strong>-h</strong>=<em>synopsis|...</em></a> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das oyranos-device Program zeigt und ändert Einstellungen zu ICC Geräteprofilen.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3>Setze grundlegende Parameter</h3>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--device-class</strong>=<em>KLASSE</em></td> <td>nutze Geräteklasse. Nützliche Klassen sind monitor, scanner, printer, camera
  <table>
   <tr><td style='padding-left:0.5em'><strong>-c monitor</strong></td><td># Bildschirm</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c printer</strong></td><td># Drucker</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c camera</strong></td><td># Kamera</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c scanner</strong></td><td># Scanner</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device-pos</strong>=<em>NUMMER</em></td> <td>Geräteposition started von Null </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--device-name</strong>=<em>NAME</em></td> <td>geben Sie alternativ den Namen des Gerätes an </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Wähle ICC V2 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Wähle ICC V4 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--skip-x-color-region-target</strong></td> <td>ignoriere X Color Management Geräteprofil</td> </tr>
</table>

<h3 id="assign">Weise Farbprofil einem Gerät zu</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-a</strong> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em> <strong>-p</strong>=<em>ICC_DATEINAME</em> [<strong>--system-wide</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--assign</strong></td> <td>Füge Konfiguration zu OpenICC DB hinzu</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--profile-name</strong>=<em>ICC_DATEINAME</em></td> <td>Profildateiname<br />Kann ein lehrer Text "" oder "automatic" oder der Name einer existierenden Profiles sein. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--system-wide</strong></td> <td>Füge systemweit hinzu</td> </tr>
</table>

<h3 id="erase">Löse Profil vom Gerät</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-e</strong> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em> [<strong>--system-wide</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--erase</strong></td> <td>Lösche Geräteinformationen von OpenICC DB</td> </tr>
</table>

<h3 id="setup">Initialisiere Gerät</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-s</strong> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em>

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--setup</strong></td> <td>stelle Sitzung ein mit OpenICC DB Daten</td> </tr>
</table>

<h3 id="unset">Setze Gerät zurück</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-u</strong> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em>

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--unset</strong></td> <td>Setze Sitzungseinstellung zurück zu keinen Einstellungen</td> </tr>
</table>

<h3 id="list">Zeige Geräteklassen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-l</strong> [<strong>-v</strong>] [<strong>--short</strong>] [<strong>--print-class</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list</strong></td> <td>Zeige Geräteklassen</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--short</strong></td> <td>zeige Modul ID oder Profilnamen</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--print-class</strong></td> <td>zeige die Geräteklasse des Modules</td> </tr>
</table>

<h3 id="list">Geräteliste</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-l</strong> [<strong>-c</strong>=<em>KLASSE</em>] [<strong>-d</strong>=<em>NUMMER</em>|<strong>--device-name</strong>=<em>NAME</em>] [<strong>--short</strong>] [<strong>--path</strong>] [<strong>-r</strong>] [<strong>-v</strong>]

&nbsp;&nbsp;Benötigt die -c Option

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list</strong></td> <td>Zeige Geräteklassen</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--device-class</strong>=<em>KLASSE</em></td> <td>nutze Geräteklasse. Nützliche Klassen sind monitor, scanner, printer, camera
  <table>
   <tr><td style='padding-left:0.5em'><strong>-c monitor</strong></td><td># Bildschirm</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c printer</strong></td><td># Drucker</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c camera</strong></td><td># Kamera</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-c scanner</strong></td><td># Scanner</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--short</strong></td> <td>zeige Modul ID oder Profilnamen</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--path</strong></td> <td>zeige den vollständigen Dateiname</td> </tr>
</table>

<h3 id="list-profiles">Liste lokale DB Profile für gewähltes Gerät</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>--list-profiles</strong> [<strong>-c</strong>=<em>KLASSE</em>] [<strong>-d</strong>=<em>NUMMER</em>] [<strong>--show-non-device-related</strong>]

&nbsp;&nbsp;Benötigt -c und -d Optionen.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--list-profiles</strong></td> <td>Liste lokale DB Profile für gewähltes Gerät</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--show-non-device-related</strong></td> <td>zeige auch nicht passende Profile</td> </tr>
</table>

<h3 id="list-taxi-profiles">Liste Profile von Taxi für gewähltes Gerät</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>--list-taxi-profiles</strong> [<strong>-c</strong>=<em>KLASSE</em>] [<strong>-d</strong>=<em>NUMMER</em>] [<strong>--show-non-device-related</strong>]

&nbsp;&nbsp;Benötigt -c und -d Optionen.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--list-taxi-profiles</strong></td> <td>Liste Profile von Taxi für gewähltes Gerät</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--show-non-device-related</strong></td> <td>zeige auch nicht passende Profile</td> </tr>
</table>

<h3 id="format">Schreibe Gerätefarbeinstellungen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-f</strong>=<em>icc|openicc+rank-map|openicc|openicc-rank-map</em> <strong>-c</strong>=<em>KLASSE</em> <strong>-d</strong>=<em>NUMMER</em> <strong>-j</strong>=<em>DATEINAME</em> <strong>-k</strong>=<em>DATEINAME</em> [<strong>-o</strong>=<em>DATEINAME</em>] [<strong>--only-db</strong>] [<strong>-m</strong>]

&nbsp;&nbsp;Benötigt -c und -d Optionen.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong>=<em>icc|openicc+rank-map|openicc|openicc-rank-map</em></td> <td>Schreibe OpenICC Konfigurationdaten
  <table>
   <tr><td style='padding-left:0.5em'><strong>-f icc</strong></td><td># Schreibe zugewiesenes ICC Profil</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f fallback-icc</strong></td><td># erzeuge alternatives ICC Profil</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f openicc+rank-map</strong></td><td># erzeuge OpenICC Gerätefarbeinstellungs JSON mit Wichtungstabelle</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f openicc</strong></td><td># erzeuge OpenICC Gerätefarbeinstellungs JSON</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-f openicc-rank-map</strong></td><td># erzeuge OpenICC Wichtungstabelle JSON zu Gerätefarbeinstellungen</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>DATEINAME</em></td> <td>schreibe nach Datei  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--device-json</strong>=<em>DATEINAME</em></td> <td>benutze Geräte JSON alternativ zu den -c und -d Optionen </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--rank-json</strong>=<em>DATEINAME</em></td> <td>benutze Wichtungstabelle im JSON-Format alternativ zu den -c und -d Optionen </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--only-db</strong></td> <td>benutze nur DB Schlüssel/Wertepaare für -f=openicc</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--device-meta-tag</strong></td> <td>bette Geräte-und Treiberinformationen in ICC meta Block ein</td> </tr>
</table>


<h2>ALLGEMEINE OPTIONEN <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="help">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-device</strong></a> <strong>-h</strong>=<em>synopsis|...</em> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong>=<em>synopsis|...</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Exportiere formatierten Text<br />Hole Benutzerschnittstelle als Text
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X man</strong></td><td># Handbuch</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X markdown</strong></td><td># Markdown</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json</strong></td><td># Json</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X json+command</strong></td><td># Json + Kommando</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X export</strong></td><td># Export</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Wähle Darstellung<br />Wähle und konfiguriere eine Ausgabeform. -R=gui wird eine grafische Ausgabe starten.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R gui</strong></td><td># Gui</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R cli</strong></td><td># Cli</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R web</strong></td><td># Web</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R -</strong></td><td># </td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>


<h2>ENVIRONMENT <a href="#toc" name="environment">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;setze den Oyranos Fehlersuchniveau. Die -v Option kann alternativ benutzt werden.
#### XDG_DATA_HOME XDG_DATA_DIRS
&nbsp;&nbsp;richte Oyranos auf obere Verzeichnisse, welche Resourcen enthalten. An die Pfade für ICC Profile wird "color/icc" angehangen. http://www.openicc.org/index.php%3Ftitle=OpenIccDirectoryProposal.html
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos-profiles](oyranosprofiles.html)<a href="oyranosprofiles.md">(1)</a>&nbsp;&nbsp;[oyranos-profile](oyranosprofile.html)<a href="oyranosprofile.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2021 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

