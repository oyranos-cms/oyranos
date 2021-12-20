# oyranos-profile v0.9.7 {#oyranosprofilede}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [ALLGEMEINE OPTIONEN](#general_options) [ENVIRONMENT](#environment) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyranos-profile"</strong> *1* <em>"March 06, 2015"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-profile v0.9.7 - Oyranos Profil

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-profile</strong> [<strong>--path</strong>] [<strong>--short</strong>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#list-tags"><strong>-l</strong></a> [<strong>-p</strong>=<em>NUMMER</em>] [<strong>-n</strong>=<em>NAME</em>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#remove-tag"><strong>-r</strong>=<em>NUMMER</em></a> l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#list-hash"><strong>-m</strong></a> [<strong>-w</strong>=<em>ICC_DATEINAME</em>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#ppmcie"><strong>--ppmcie</strong></a> [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#output"><strong>-o</strong>=<em>DATEINAME</em></a> [<strong>-c</strong>=<em>KLASSE</em>] [<strong>-f</strong>=<em>xml...</em>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#profile-name"><strong>-w</strong>=<em>ICC_DATEINAME</em></a> [<strong>-j</strong>=<em>DATEINAME</em>|<strong>-m</strong>] [<strong>-s</strong>=<em>NAME</em>] [<strong>-2</strong>] [<strong>-4</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE
<br />
<strong>oyranos-profile</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das oyranos-profile Program zeigt die Informationen eines ICC Profiles an und erlaubt Änderungen.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3>Zeige ICC Profil</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> [<strong>--path</strong>] [<strong>--short</strong>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><em>l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE</em></td> <td>ICC Profil<br />kann ein Dateiname, interner Name, ICC Profil ID oder ein Namenkürzel sein "rgb", "cmyk", "gray", "lab", "xyz", "web", "rgbi", "cmyki", "grayi", "labi", "xyzi". Kürzel endend auf "i" sind vermutete Profile. "web" ist ein sRGB Profile. Die anderen Kürzel sind Editierprofile. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--path</strong></td> <td>kompletter Pfad- und Dateiname</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--short</strong></td> <td>Zeige nur den Profilnamen</td> </tr>
</table>

<h3 id="list-tags">Liste der enthaltenen ICC Elemente</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-l</strong> [<strong>-p</strong>=<em>NUMMER</em>] [<strong>-n</strong>=<em>NAME</em>] [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-tags</strong></td> <td>Liste die enthaltenen Elemente zusätzlich zur Übersicht und Dateikopf.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--tag-pos</strong>=<em>NUMMER</em></td> <td>wähle Eintrag </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--tag-name</strong>=<em>NAME</em></td> <td>wähle Eintrag </tr>
</table>

<h3 id="remove-tag">Lösche enthaltenes ICC Element</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-r</strong>=<em>NUMMER</em> l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--remove-tag</strong>=<em>NUMMER</em></td> <td><br />lösche nummeriertes Element  </td>
 </tr>
</table>

<h3 id="list-hash">Zeige Profil ID</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-m</strong> [<strong>-w</strong>=<em>ICC_DATEINAME</em>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--list-hash</strong></td> <td>Zeige internen Hashwert</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--profile-name</strong>=<em>ICC_DATEINAME</em></td> <td>schreibe Profil mit korrekter ID<br />Die -w Option gibt den inneren und äußeren Profilename an.PROFILNAME gibt das Quellprofile an.  </td>
 </tr>
</table>

<h3 id="ppmcie">Zeige CIE*xy Primärfarben</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>--ppmcie</strong> [<strong>-v</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--ppmcie</strong></td> <td>zeige CIE*xy Farbwerte, falls vorhanden. Zur Benutzung mit ppmcie.</td> </tr>
</table>

<h3 id="output">Schreibe Geräteinformationen nach OpenICC JSON</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-o</strong>=<em>DATEINAME</em> [<strong>-c</strong>=<em>KLASSE</em>] [<strong>-f</strong>=<em>xml...</em>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>DATEINAME</em></td> <td><br />Schreibe Geräteinformationen nach OpenICC JSON.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--device-class</strong>=<em>KLASSE</em></td> <td>nutze Geräteklasse. Nützliche Klassen sind monitor, scanner, printer, camera  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong>=<em>xml...</em></td> <td>benutze IccXML Format  </td>
 </tr>
</table>

<h3 id="profile-name">Schreibe ICC Profil</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-w</strong>=<em>ICC_DATEINAME</em> [<strong>-j</strong>=<em>DATEINAME</em>|<strong>-m</strong>] [<strong>-s</strong>=<em>NAME</em>] [<strong>-2</strong>] [<strong>-4</strong>] l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-w</strong>|<strong>--profile-name</strong>=<em>ICC_DATEINAME</em></td> <td>schreibe Profil mit korrekter ID<br />Die -w Option gibt den inneren und äußeren Profilename an.PROFILNAME gibt das Quellprofile an.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-j</strong>|<strong>--json-name</strong>=<em>DATEINAME</em></td> <td>bette OpenICC JSON Gerät von Datei ein  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--list-hash</strong></td> <td>Zeige internen Hashwert</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--name-space</strong>=<em>NAME</em></td> <td>füge Vorsilbe hinzu  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Wähle ICC V2 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Wähle ICC V4 Profile</td> </tr>
</table>


<h2>ALLGEMEINE OPTIONEN <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="help">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profile</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|cli|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Exportiere formatierten Text<br />Hole Benutzerschnittstelle als Text
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> man</td><td># Handbuch : Unix Handbuchseite - Hole Unix Handbuchseite</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> markdown</td><td># Markdown : Formatierter Text - Hole formatierten Text</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json</td><td># Json : GUI - Hole Oyjl Json Benutzerschnittstelle</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json+command</td><td># Json + Kommando : GUI + Kommando - Hole Oyjl Json Benutzerschnittstelle mit Kommando</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : Alle verfügbaren Daten - Erhalte Daten für Entwickler</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Wähle Darstellung<br />Wähle und konfiguriere eine Ausgabeform. -R=gui wird eine grafische Ausgabe starten.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> gui</td><td># Gui : Zeige UI - Zeige eine interaktive grafische Benutzerschnittstelle.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> cli</td><td># Cli : Zeige UI - Zeige Hilfstext für Benutzerschnittstelle auf der Kommandozeile.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Starte Web Server - Starte lokalen Web Service für die Darstellung in einem Webbrowser</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
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

<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Zeige Übersicht und Dateikopf des ICC Profiles
&nbsp;&nbsp;oyranos-profile sRGB.icc
#### Zeige Inhalt des ersten Elementes eines Profiles
&nbsp;&nbsp;oyranos-profile -lv -p=1 sRGB.icc
#### Zeige nur die Profilprüfsumme
&nbsp;&nbsp;oyranos-profile -m sRGB.icc
#### Zeige die RGB Primärfarben eines Matrixprofiles innerhalb eines CIE*xy Diagrammes
&nbsp;&nbsp;ppmcie `oyranos-profile --ppmcie sRGB.icc` > sRGB_cie-xy.ppm
#### Füge Kalibrierungsdaten zum meta Element eines Profiles
&nbsp;&nbsp;oyranos-profile -w Mein_Profil -j Mein_Gerät.json Mein_Profile.icc
#### Benene das Profil einem externen Werkzeug.
&nbsp;&nbsp;iccdump "`oyranos-profile --path cmyk`"

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-profiles](oyranosprofiles.html)<a href="oyranosprofiles.md">(1)</a>&nbsp;&nbsp;[oyranos-profile-graph](oyranosprofilegraph.html)<a href="oyranosprofilegraph.md">(1)</a>&nbsp;&nbsp;[oyranos-config-fltk](oyranosconfigfltk.html)<a href="oyranosconfigfltk.md">(1)</a>&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>&nbsp;&nbsp;[ppmcie](ppmcie.html)<a href="ppmcie.md">(1)</a>

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

