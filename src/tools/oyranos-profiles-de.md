# oyranos\-profiles v0.9.7 {#oyranosprofilesde}
*"oyranos\-profiles"* *1* *"October 11, 2018"* "User Commands"
## NAME
oyranos\-profiles v0.9.7 \- Oyranos Profile
## ÜBERSICHT
**oyranos\-profiles** <strong>\-l</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-a</strong>] [<strong>\-c</strong>] [<strong>\-d</strong>] [<strong>\-k</strong>] [<strong>\-n</strong>] [<strong>\-o</strong>] [<strong>\-i</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-P</strong>] [<strong>\-T</strong>=<em>SCHLÜSSEL;WERT</em>] [<strong>\-v</strong>]
<br />
**oyranos\-profiles** <strong>\-p</strong> [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-profiles** <strong>\-I</strong>=<em>ICC\_PROFIL</em> | <strong>\-t</strong>=<em>TAXI\_ID</em> [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-g</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-profiles** <strong>\-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em>
## BESCHREIBUNG
Das Werkzeug kann installierte Profile und Suchpfade anzeigen. Es kann ein Profil in einen Suchpfad installieren.
## OPTIONEN
### Liste der verfügbaren ICC Farbprofile
&nbsp;&nbsp;**oyranos\-profiles** <strong>\-l</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-a</strong>] [<strong>\-c</strong>] [<strong>\-d</strong>] [<strong>\-k</strong>] [<strong>\-n</strong>] [<strong>\-o</strong>] [<strong>\-i</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-P</strong>] [<strong>\-T</strong>=<em>SCHLÜSSEL;WERT</em>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list-profiles</strong></td> <td>Liste Profile</td> </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-P</strong>|<strong>--path</strong></td> <td>Zeige Profile, welche ein Wort in ihrem Pfadnamen enthalten.<br />PFAD\_WORT_TEIL </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-T</strong>|<strong>--meta</strong> <em>SCHLÜSSEL;WERT</em></td> <td>Suche nach Schlüssel/Werte Paar in Meta Element<br />Zeige Profile welche ein bestimmtes Schlüssel/Werte Paar im meta Tag enthalten. WERT kann '*' enthalten für das Filtern von Wortteilen. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-D</strong>|<strong>--duplicates</strong></td> <td>Zeige identische mehrfach installierte Profile an</td> </tr>
</table>

### Zeige Suchpfade
&nbsp;&nbsp;**oyranos-profiles** <strong>\-p</strong> [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--list-paths</strong></td> <td>Liste der ICC Farbprofilpfade</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--user</strong></td> <td>Benutzerpfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--system</strong></td> <td>Systempfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--oyranos</strong></td> <td>Oyranospfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--machine</strong></td> <td>Maschinenpfad</td> </tr>
</table>

### Installiere Profil
&nbsp;&nbsp;**oyranos-profiles** <strong>\-I</strong>=<em>ICC_PROFIL</em> | <strong>\-t</strong>=<em>TAXI\_ID</em> [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-g</strong>] [<strong>\-v</strong>]

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-I</strong>|<strong>--install</strong> <em>ICC\_PROFIL</em></td> <td>Installiere Profil </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-t</strong>|<strong>--taxi</strong> <em>TAXI_ID</em></td> <td>ICC Taxi Profile DB </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-u</strong>|<strong>--user</strong></td> <td>Benutzerpfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--system</strong></td> <td>Systempfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-y</strong>|<strong>--oyranos</strong></td> <td>Oyranospfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-m</strong>|<strong>--machine</strong></td> <td>Maschinenpfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--gui</strong></td> <td>Zeige Grafische Benutzerschnittstelle</td> </tr>
</table>

### Allgemeine Optionen
&nbsp;&nbsp;**oyranos-profiles** <strong>\-h</strong> <strong>\-X</strong>=<em>json|json+command|man|markdown</em>

<table style='width:100%'>
 <tr><td style='padding\-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong></td> <td>Hilfe</td> </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-r</strong>|<strong>--no-repair</strong></td> <td>Lasse Reparatur der Profil ID weg</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>

## UMGEBUNGSVARIABLEN
### OY_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
  <br />
&nbsp;&nbsp;Die -v Option kann alternativ benutzt werden.
  <br />
&nbsp;&nbsp;Der gültige Bereich ist 1\-20.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
&nbsp;&nbsp;route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
## BEISPIELE
### Liste alle installierten Profile mit internem Namen
&nbsp;&nbsp;oyranos\-profiles \-le
### Liste alle installierten Profile mit Monitor\-und Ausgabeklasse
&nbsp;&nbsp;oyranos\-profiles \-l \-od
### Liste alle installierten Profile im Benutzerpfad
&nbsp;&nbsp;oyranos\-profiles \-lfu
### Installiere ein Profile für den aktuellen Benutzer und zeige eine Fehlermeldung grafische an
&nbsp;&nbsp;oyranos\-profiles \-\-install profilename \-u \-\-gui
### Installiere ein Profile für den aktuellen Benutzer und zeige eine Fehlermeldung grafische an
&nbsp;&nbsp;oyranos\-profiles \-\-install \-\-taxi=taxi\_id/0 \-\-gui \-d \-u
### Zeige Dateiinfos
&nbsp;&nbsp;SAVEIFS=$IFS ; IFS=$'\n\b'; profiles=(\`oyranos\-profiles \-ldf\`); IFS=$SAVEIFS; for file in "${profiles[@]}"; do ls "$file"; done
## SIEHE AUCH
### oyranos\-profile\-graph(1) oyranos\-config(1) oyranos\-policy(1) oyranos(3)
### http://www.oyranos.org
## AUTOR
Kai\-Uwe Behrmann http://www.oyranos.org
## KOPIERRECHT
*© 2005\-2019 Kai\-Uwe Behrmann and others*


### Lizenz
newBSD
## FEHLER
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

