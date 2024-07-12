# oyranos-monitor v0.9.7 {#oyranosmonitorde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [ENVIRONMENT](#environment) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyranos-monitor"</strong> *1* <em>"22. Februar 2018"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-monitor v0.9.7 - Monitorkonfiguration mit dem Oyranos Farbmanagementsystem

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-monitor</strong> | <strong>-s</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-monitor</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--system-wide</strong>] [<strong>-v</strong>] ICC_DATEINAME
<br />
<strong>oyranos-monitor</strong> <a href="#erase"><strong>-e</strong></a> <strong>-u</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--system-wide</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#list"><strong>-l</strong></a> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--short</strong>] [<strong>--path</strong>] [<strong>-c</strong>] [<strong>-v</strong>] [<strong>--module</strong>=<em>MODULE_NAME</em>] [<strong>-2</strong>] [<strong>-4</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#format"><strong>-f</strong>=<em>edid|icc|edid_icc|vcgt...</em></a> <strong>-a</strong> <strong>--modules</strong> <strong>-L</strong> [<strong>-o</strong>=<em>AUSGABE_DATEINAME|voreingestellt:stdout</em>] [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--display</strong>=<em>NAME</em>] [<strong>-m</strong>] [<strong>-v</strong>] [<strong>-2</strong>] [<strong>-4</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#daemon"><strong>--daemon</strong></a> [<strong>-v</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#add-edid"><strong>--add-edid</strong>=<em>DATEINAME</em></a> <strong>--add-vcgt</strong>=<em>DATEINAME</em> <strong>--profile</strong>=<em>ICC_DATEINAME</em> [<strong>--name</strong>=<em>NAME</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-monitor</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-R</strong>=<em>gui|cli|web|...</em> <strong>-V</strong> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das oyranos-monitor Program erlaubt das Erfragen und setzen von Bildschirmprofilen innerhalb des Oyranos Farbmanagementsystems (CMS). Die Profile werden in einer Datenbank gespeichert, der Oyranos DB.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3>Setze grundlegende Parameter</h3>


<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-x</strong>=<em>PIXEL</em></td> <td>Wähle die Anzeige an der x Position. Die oyranos Ausgaben, sind wenn möglich Geräte. Überlappende Positionen werden nicht unterstützt.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>=<em>PIXEL</em></td> <td>Wähle die Anzeige an der y Position.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--device-pos</strong>=<em>NUMMER</em></td> <td>Position in Geräteliste.<br />Die Nummerierung der Bildschirme startet mit Null. Um die Zahl aller verfügbaren Geräte zu erhalten benutze: "oyranos-monitor -l | wc -l".  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--display</strong>=<em>NAME</em></td> <td>Nutze DISPLAY  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--module</strong>=<em>MODULE_NAME</em></td> <td>Wähle Modul<br />Siehe auch die --modules Option.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--x-color-region-target</strong></td> <td>Zeige X Color Management (XCM) Geräteprofil</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--system-wide</strong></td> <td>Tue es systemweit, benötigt eventuell Adminrechte</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Wähle ICC V2 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Wähle ICC V4 Profile</td> </tr>
</table>

<h3>Aktiviere Profile</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> | <strong>-s</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>-v</strong>]

&nbsp;&nbsp;Initialisiere den X11 Server mit dem ICC Geräteprofl aus der Oyranos DB.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'>|</td> <td>Keine Argumente<br />Starte Kommando ohne Argumente</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--setup</strong></td> <td>Stelle ICC Profil für alle oder einen gewählten Bildschirm</td> </tr>
</table>

<h3>Setze neues Profil</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--system-wide</strong>] [<strong>-v</strong>] ICC_DATEINAME

&nbsp;&nbsp;Füge ein ICC Bildschirmprofil der gewählten Anzeige zu und speichere in der Oyranos DB

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><em>ICC_DATEINAME</em></td> <td>ICC Farbprofil für einen Bildschirm  </td>
 </tr>
</table>

<h3 id="erase">Melde Profil ab</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>-e</strong> <strong>-u</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--system-wide</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--erase</strong></td> <td>Löse ein ICC Profil von einem Bildschirmgerät<br />Setze die Gerätegammatabelle zu den Vorgabewerten und lösche von der Oyranos DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--unset</strong></td> <td>Löse ein ICC Profil von einem Bildschirmgerät<br />Setze die Gerätegammatabelle zu den Vorgabewerten</td> </tr>
</table>

<h3 id="list">Geräteliste</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>-l</strong> [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--short</strong>] [<strong>--path</strong>] [<strong>-c</strong>] [<strong>-v</strong>] [<strong>--module</strong>=<em>MODULE_NAME</em>] [<strong>-2</strong>] [<strong>-4</strong>]

&nbsp;&nbsp;Liste Namen auf und zeige Details.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list</strong></td> <td>Zeige Geräteliste</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--short</strong></td> <td>Zeige nur den Profilnamen</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--path</strong></td> <td>kompletter Pfad- und Dateiname</td> </tr>
</table>

<h3 id="format">Zeige Informationen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>-f</strong>=<em>edid|icc|edid_icc|vcgt...</em> <strong>-a</strong> <strong>--modules</strong> <strong>-L</strong> [<strong>-o</strong>=<em>AUSGABE_DATEINAME|voreingestellt:stdout</em>] [<strong>-x</strong>=<em>PIXEL</em>] [<strong>-y</strong>=<em>PIXEL</em>|<strong>-d</strong>=<em>NUMMER</em>] [<strong>--display</strong>=<em>NAME</em>] [<strong>-m</strong>] [<strong>-v</strong>] [<strong>-2</strong>] [<strong>-4</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--format</strong>=<em>edid|icc|edid_icc|vcgt...</em></td> <td>Wähle Datenformat<br />Wähle ein Datenformat. Möglich sind edid für EDID Daten, icc für ein ICC Profil edid_icc für ein ICC Profil welches aus EDID Daten erzeugt wurde und vcgt für Grafikkartengammatabelle.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong>=<em>AUSGABE_DATEINAME|voreingestellt:stdout</em></td> <td>Schreibe Daten passend zur -f/--format Option heraus.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--device-meta-tag</strong></td> <td>(--device-meta-tag) Schalter wird von -f=icc und -f=edid_icc Optionen akzeptiert.<br />Bette Geräte Informations in ein Geräteprofil ein. Dies erleichter die passende Profileauswahl sowohl lokal als auch beim Teilen mit anderen Nutzern und Systemen.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--xcm-active</strong></td> <td><br />Erfrage X Color Management Farbserver Status.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--modules</strong></td> <td><br />Bildschirmmodulliste</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-L</strong>|<strong>--list-taxi</strong></td> <td>Liste Taxi Profile</td> </tr>
</table>

<h3 id="daemon">Starte Dienst</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>--daemon</strong> [<strong>-v</strong>]

&nbsp;&nbsp;Halte die Sitzung aktuell.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--daemon</strong></td> <td><br />Starte Dienst, um laufend neue Bildschirme zu erkennen und einzustellen.</td> </tr>
</table>

<h3 id="add-edid">Füge Geräte Meta Element hinzu</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>--add-edid</strong>=<em>DATEINAME</em> <strong>--add-vcgt</strong>=<em>DATEINAME</em> <strong>--profile</strong>=<em>ICC_DATEINAME</em> [<strong>--name</strong>=<em>NAME</em>] [<strong>-v</strong>]

&nbsp;&nbsp;Füge Geräte Informationen zu einen Profile hinzu für automatisierte Auswahl.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--add-edid</strong>=<em>DATEINAME</em></td> <td>EDID Dateiname<br />Bette EDID Schlüssen in ein ICC Profle als meta Element. Benötigt --profile Option.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--add-vcgt</strong>=<em>DATEINAME</em></td> <td>VCGT Dateiname<br />Bette eine VCGT Kalibration in ein ICC Profil als vcgt Element. DATEINAME kann die Ausgabe von xcalib -p sein. Die Option benötigt --profile.  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--profile</strong>=<em>ICC_DATEINAME</em></td> <td>ICC Profil  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--name</strong>=<em>NAME</em></td> <td>Bezeichnung für den Internen Namen eines ICC Profiles  </td>
 </tr>
</table>

<h3 id="help">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-monitor</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-R</strong>=<em>gui|cli|web|...</em> <strong>-V</strong> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> -</td><td># Vollständige Hilfe : Zeige Hilfe für alle Gruppen</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h</strong> synopsis</td><td># Übersicht : Liste Gruppen - Zeige alle Gruppen mit Syntax</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-X</strong>|<strong>--export</strong>=<em>json|json+command|man|markdown</em></td> <td>Exportiere formatierten Text<br />Hole Benutzerschnittstelle als Text
  <table>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> man</td><td># Handbuch : Unix Handbuchseite - Hole Unix Handbuchseite</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> markdown</td><td># Markdown : Formatierter Text - Hole formatierten Text</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json</td><td># Json : GUI - Hole Oyjl Json Benutzerschnittstelle</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> json+command</td><td># Json + Kommando : GUI + Kommando - Hole Oyjl Json Benutzerschnittstelle mit Kommando</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : Alle verfügbaren Daten - Erhalte Daten für Entwickler. Das Format kann mit dem oyjl-args Werkzeug umgewandelt werden.</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Darstellung
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> gui</td><td># Gui : Zeige UI - Zeige eine interaktive grafische Benutzerschnittstelle.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> cli</td><td># Cli : Zeige UI - Zeige Hilfstext für Benutzerschnittstelle auf der Kommandozeile.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Starte Web Server - Starte lokalen Web Service für die Darstellung in einem Webbrowser. Die -R=web:help Unteroption zeigt weitere Informationen an.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudere mehr</td> </tr>
</table>


<h2>ENVIRONMENT <a href="#toc" name="environment">&uarr;</a></h2>

#### DISPLAY
&nbsp;&nbsp;Auf X11 Systemen wird die Anzeige mit dieser Variable ausgewählt.
#### OY_DEBUG
&nbsp;&nbsp;setze den Oyranos Fehlersuchniveau. Die -v Option kann alternativ benutzt werden.
#### XDG_DATA_HOME XDG_DATA_DIRS
&nbsp;&nbsp;richte Oyranos auf obere Verzeichnisse, welche Resourcen enthalten. An die Pfade für ICC Profile wird "color/icc" angehangen. http://www.openicc.org/index.php%3Ftitle=OpenIccDirectoryProposal.html
  <br />
&nbsp;&nbsp;http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
#### OY_MODULE_PATH
&nbsp;&nbsp;zeige Oyranos zusätzliche Verzeichnisse mit Modulen.

<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Schreibe folgende Zeile in ein Startskript wie .xinitrc
&nbsp;&nbsp;oyranos-monitor
  <br />
&nbsp;&nbsp;Wähle ein Bildschirmprofl und lade es in X und fülle die Grafikkartengammatabelle falls Daten dafür enthalten sind.
#### Weise Farbprofil einem Gerät zu
&nbsp;&nbsp;oyranos-monitor -x pos -y pos profilename
#### Setze eine Gerätekurve zurück,um z.B. eine Kalibration durchzuführen.
&nbsp;&nbsp;oyranos-monitor -e -x pos -y pos profilename
#### Erfrage das serverseitige, netzwerkzugängliche Profil
&nbsp;&nbsp;oyranos-monitor -x pos -y pos
#### Liste alle Oyranos Bildschirm Farbgerät auf
&nbsp;&nbsp;oyranos-monitor -l
#### Zeige das erste Oyranos Bildschirmgerät plaudernd an
&nbsp;&nbsp;oyranos-monitor -l -v -d 0
#### Sende das Profil einem externen Werkzeug.
&nbsp;&nbsp;iccDumpProfile -v "`oyranos-monitor -l -d 0 -c --path`"
#### Liste alle Bildschirmgeräte durch das oyX1 Modul
&nbsp;&nbsp;oyranos-monitor -l --module oyX1
#### Drucke Daten aus passend zur -f Option
&nbsp;&nbsp;oyranos-monitor -f=[edid|icc|edid_icc] -o=edid.bin -x=pos -y=pos -m
#### Bette Geräteinformationen in ICC meta Block ein
&nbsp;&nbsp;oyranos-monitor --add-edid=edid_filename --profile=profilename.icc --name=profilename

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-config-fltk](oyranosconfigfltk.html)<a href="oyranosconfigfltk.md">(1)</a>&nbsp;&nbsp;[oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos-profiles](oyranosprofiles.html)<a href="oyranosprofiles.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 https://gitlab.com/oyranos/oyranos


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann https://gitlab.com/beku

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2024 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Lizenz
newBSD <a href="https://gitlab.com/oyranos/oyranos">https://gitlab.com/oyranos/oyranos</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

