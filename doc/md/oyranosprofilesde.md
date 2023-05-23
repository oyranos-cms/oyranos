# oyranos-profiles v0.9.7 {#oyranosprofilesde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [ALLGEMEINE OPTIONEN](#general_options) [UMGEBUNGSVARIABLEN](#environmentvariables) [BEISPIELE](#examples) [SIEHE AUCH](#seeaswell) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyranos-profiles"</strong> *1* <em>"October 11, 2018"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-profiles v0.9.7 - Oyranos Profile

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-profiles</strong> <a href="#list-profiles"><strong>-l</strong></a> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-a</strong>] [<strong>-c</strong>] [<strong>-d</strong>] [<strong>-k</strong>] [<strong>-n</strong>] [<strong>-o</strong>] [<strong>-i</strong>] [<strong>-2</strong>] [<strong>-4</strong>] [<strong>-P</strong>=<em>PFAD_WORT_TEIL</em>] [<strong>-T</strong>=<em>SCHLÜSSEL:WERT</em>] [<strong>-v</strong>]
<br />
<strong>oyranos-profiles</strong> <a href="#list-paths"><strong>-p</strong></a> [<strong>-u</strong>|<strong>-s</strong>|<strong>-y</strong>|<strong>-m</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-profiles</strong> <a href="#install"><strong>-I</strong>=<em>ICC_PROFIL</em></a> | <strong>-t</strong>=<em>TAXI_ID</em> [<strong>-u</strong>|<strong>-s</strong>|<strong>-y</strong>|<strong>-m</strong>] [<strong>-g</strong>] [<strong>-v</strong>]
<br />
<strong>oyranos-profiles</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong>

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das Werkzeug kann installierte Profile und Suchpfade anzeigen. Es kann ein Profil in einen Suchpfad installieren.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="list-profiles">Liste der verfügbaren ICC Farbprofile</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profiles</strong></a> <strong>-l</strong> [<strong>-f</strong>] [<strong>-e</strong>] [<strong>-a</strong>] [<strong>-c</strong>] [<strong>-d</strong>] [<strong>-k</strong>] [<strong>-n</strong>] [<strong>-o</strong>] [<strong>-i</strong>] [<strong>-2</strong>] [<strong>-4</strong>] [<strong>-P</strong>=<em>PFAD_WORT_TEIL</em>] [<strong>-T</strong>=<em>SCHLÜSSEL:WERT</em>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-profiles</strong></td> <td>Liste Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-f</strong>|<strong>--full-names</strong></td> <td>Profildateiname<br />Zeige kompletten Pfad- und Dateinamen</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-e</strong>|<strong>--internal-names</strong></td> <td>Interne Namen<br />Der Text kommt vom 'desc' Profilelement.</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-a</strong>|<strong>--abstract</strong></td> <td>Wähle abstrakte Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-c</strong>|<strong>--color-space</strong></td> <td>Wähle Farbraumprofile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--display</strong></td> <td>Wähle Monitorprofile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-k</strong>|<strong>--device-link</strong></td> <td>Wähle Verknüpfungsprofil</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-n</strong>|<strong>--named-color</strong></td> <td>Wähle Einzelfarbprofile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-o</strong>|<strong>--output</strong></td> <td>Wähle Ausgabeprofil</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong></td> <td>Wähle Eingangsprofile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-2</strong>|<strong>--icc-version-2</strong></td> <td>Wähle ICC V2 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-4</strong>|<strong>--icc-version-4</strong></td> <td>Wähle ICC V4 Profile</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-P</strong>|<strong>--path</strong>=<em>PFAD_WORT_TEIL</em></td> <td>Zeige Profile, welche ein Wort in ihrem Pfadnamen enthalten.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-P</strong> basICColor</td>
   <tr><td style='padding-left:0.5em'><strong>-P</strong> colord</td>
   <tr><td style='padding-left:0.5em'><strong>-P</strong> edid</td>
   <tr><td style='padding-left:0.5em'><strong>-P</strong> OpenICC</td>
   <tr><td style='padding-left:0.5em'><strong>-P</strong> oyra</td>
   <tr><td style='padding-left:0.5em'><strong>-P</strong> xorg</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-T</strong>|<strong>--meta</strong>=<em>SCHLÜSSEL:WERT</em></td> <td>Suche nach Schlüssel/Werte Paar in Meta Element<br />Zeige Profile welche ein bestimmtes Schlüssel/Werte Paar im meta Tag enthalten. WERT kann '*' enthalten für das Filtern von Wortteilen.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-T</strong> EFFECT_class:sepia</td>
   <tr><td style='padding-left:0.5em'><strong>-T</strong> EFFECT_class:bw</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-D</strong>|<strong>--duplicates</strong></td> <td>Zeige identische mehrfach installierte Profile an</td> </tr>
</table>

<h3 id="list-paths">Zeige Suchpfade</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profiles</strong></a> <strong>-p</strong> [<strong>-u</strong>|<strong>-s</strong>|<strong>-y</strong>|<strong>-m</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--list-paths</strong></td> <td>Liste der ICC Farbprofilpfade</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--user</strong></td> <td>Benutzerpfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--system</strong></td> <td>Systempfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--oyranos</strong></td> <td>Oyranospfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--machine</strong></td> <td>Maschinenpfad</td> </tr>
</table>

<h3 id="install">Installiere Profil</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profiles</strong></a> <strong>-I</strong>=<em>ICC_PROFIL</em> | <strong>-t</strong>=<em>TAXI_ID</em> [<strong>-u</strong>|<strong>-s</strong>|<strong>-y</strong>|<strong>-m</strong>] [<strong>-g</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-I</strong>|<strong>--install</strong>=<em>ICC_PROFIL</em></td> <td>Installiere Profil  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--taxi</strong>=<em>TAXI_ID</em></td> <td>ICC Taxi Profile DatenBank  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--user</strong></td> <td>Benutzerpfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--system</strong></td> <td>Systempfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--oyranos</strong></td> <td>Oyranospfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--machine</strong></td> <td>Maschinenpfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--gui</strong></td> <td>Zeige Grafische Benutzerschnittstelle</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--test</strong></td> <td>Keine Ausführung</td> </tr>
</table>


<h2>ALLGEMEINE OPTIONEN <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="help">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-profiles</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-V</strong>

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.</td> </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Wähle Darstellung<br />Wähle und konfiguriere eine Ausgabeform. -R=gui wird eine grafische Ausgabe starten. -R="web:port=port_nummer:https_key=TLS_privater_Schlüssel_Dateiname:https_cert=TLS_CA_Zertifikat_Dateiname:css=Stil_Dateiname.css" wird einen Web Service starten.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> gui</td><td># Gui : Zeige UI - Zeige eine interaktive grafische Benutzerschnittstelle.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> cli</td><td># Cli : Zeige UI - Zeige Hilfstext für Benutzerschnittstelle auf der Kommandozeile.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Starte Web Server - Starte lokalen Web Service für die Darstellung in einem Webbrowser. Die -R=web:help Unteroption zeigt weitere Informationen an.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>


<h2>UMGEBUNGSVARIABLEN <a href="#toc" name="environmentvariables">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
  <br />
&nbsp;&nbsp;Die -v Option kann alternativ benutzt werden.
  <br />
&nbsp;&nbsp;Der gültige Bereich ist 1-20.
#### XDG_DATA_HOME XDG_DATA_DIRS
&nbsp;&nbsp;richte Oyranos auf obere Verzeichnisse, welche Resourcen enthalten. An die Pfade für ICC Farbprofile wird "color/icc" angehangen. http://www.openicc.org/index.php%3Ftitle=OpenIccDirectoryProposal.html

<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Liste alle installierten Profile mit internem Namen
&nbsp;&nbsp;oyranos-profiles -le
#### Liste alle installierten Profile mit Monitor-und Ausgabeklasse
&nbsp;&nbsp;oyranos-profiles -l -od
#### Liste alle installierten Profile im Benutzerpfad
&nbsp;&nbsp;oyranos-profiles -lfu
#### Installiere ein Profile für den aktuellen Benutzer und zeige eine Fehlermeldung grafische an
&nbsp;&nbsp;oyranos-profiles --install profilename -u --gui
#### Installiere ein Profile für den aktuellen Benutzer und zeige eine Fehlermeldung grafische an
&nbsp;&nbsp;oyranos-profiles --install --taxi=taxi_id/0 --gui -d -u
#### Zeige Dateiinfos
&nbsp;&nbsp;SAVEIFS=$IFS ; IFS=$'\n\b'; profiles=(`oyranos-profiles -ldf`); IFS=$SAVEIFS; for file in "${profiles[@]}"; do ls "$file"; done

<h2>SIEHE AUCH <a href="#toc" name="seeaswell">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-profile-graph](oyranosprofilegraph.html)<a href="oyranosprofilegraph.md">(1)</a>&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(1)</a>&nbsp;&nbsp;[oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2021 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

