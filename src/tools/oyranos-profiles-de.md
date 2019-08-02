# oyranos\-profiles v0.9.7 {#oyranosprofilesde}
*"oyranos\-profiles"* *1* *"October 11, 2018"* "User Commands"
## NAME
oyranos\-profiles v0.9.7 \- Oyranos Profile
## ÜBERSICHT
**oyranos\-profiles** <strong>\-l</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-a</strong>] [<strong>\-c</strong>] [<strong>\-d</strong>] [<strong>\-k</strong>] [<strong>\-n</strong>] [<strong>\-o</strong>] [<strong>\-i</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-P</strong>] [<strong>\-T</strong> *SCHLÜSSEL;WERT*] [<strong>\-v</strong>]
<br />
**oyranos\-profiles** <strong>\-p</strong> [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-profiles** <strong>\-I</strong> *ICC\_PROFIL* | <strong>\-t</strong> *TAXI\_ID* [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-g</strong>] [<strong>\-v</strong>]
<br />
**oyranos\-profiles** <strong>\-h</strong> <strong>\-X</strong> *json|json+command|man|markdown*
## BESCHREIBUNG
Das Werkzeug kann installierte Profile und Suchpfade anzeigen. Es kann ein Profil in einen Suchpfad installieren.
## OPTIONEN
### Liste der verfügbaren ICC Farbprofile
&nbsp;&nbsp;**oyranos\-profiles** <strong>\-l</strong> [<strong>\-f</strong>] [<strong>\-e</strong>] [<strong>\-a</strong>] [<strong>\-c</strong>] [<strong>\-d</strong>] [<strong>\-k</strong>] [<strong>\-n</strong>] [<strong>\-o</strong>] [<strong>\-i</strong>] [<strong>\-2</strong>] [<strong>\-4</strong>] [<strong>\-P</strong>] [<strong>\-T</strong> *SCHLÜSSEL;WERT*] [<strong>\-v</strong>]

* <strong>\-l</strong>|<strong>\-\-list\-profiles</strong>	Liste Profile
* <strong>\-f</strong>|<strong>\-\-full\-names</strong>	Profildateiname: Zeige kompletten Pfad\- und Dateinamen
* <strong>\-e</strong>|<strong>\-\-internal\-names</strong>	Interne Namen: Der Text kommt vom 'desc' Profilelement.
* <strong>\-a</strong>|<strong>\-\-abstract</strong>	Wähle abstrakte Profile
* <strong>\-c</strong>|<strong>\-\-color\-space</strong>	Wähle Farbraumprofile
* <strong>\-d</strong>|<strong>\-\-display</strong>	Wähle Monitorprofile
* <strong>\-k</strong>|<strong>\-\-device\-link</strong>	Wähle Verknüpfungsprofil
* <strong>\-n</strong>|<strong>\-\-named\-color</strong>	Wähle Einzelfarbprofile
* <strong>\-o</strong>|<strong>\-\-output</strong>	Wähle Ausgabeprofil
* <strong>\-i</strong>|<strong>\-\-input</strong>	Wähle Eingangsprofile
* <strong>\-2</strong>|<strong>\-\-icc\-version\-2</strong>	Wähle ICC V2 Profile
* <strong>\-4</strong>|<strong>\-\-icc\-version\-4</strong>	Wähle ICC V4 Profile
* <strong>\-P</strong>|<strong>\-\-path</strong>	Zeige Profile, welche ein Wort in ihrem Pfadnamen enthalten.: PFAD\_WORT\_TEIL
* <strong>\-T</strong>|<strong>\-\-meta</strong> *SCHLÜSSEL;WERT*	Suche nach Schlüssel/Werte Paar in Meta Element: Zeige Profile welche ein bestimmtes Schlüssel/Werte Paar im meta Tag enthalten. WERT kann '*' enthalten für das Filtern von Wortteilen.
* <strong>\-D</strong>|<strong>\-\-duplicates</strong>	Zeige identische mehrfach installierte Profile an

### Zeige Suchpfade
&nbsp;&nbsp;**oyranos\-profiles** <strong>\-p</strong> [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-v</strong>]

* <strong>\-p</strong>|<strong>\-\-list\-paths</strong>	Liste der ICC Farbprofilpfade
* <strong>\-u</strong>|<strong>\-\-user</strong>	Benutzerpfad
* <strong>\-s</strong>|<strong>\-\-system</strong>	Systempfad
* <strong>\-y</strong>|<strong>\-\-oyranos</strong>	Oyranospfad
* <strong>\-m</strong>|<strong>\-\-machine</strong>	Maschinenpfad

### Installiere Profil
&nbsp;&nbsp;**oyranos\-profiles** <strong>\-I</strong> *ICC\_PROFIL* | <strong>\-t</strong> *TAXI\_ID* [<strong>\-u</strong>|<strong>\-s</strong>|<strong>\-y</strong>|<strong>\-m</strong>] [<strong>\-g</strong>] [<strong>\-v</strong>]

* <strong>\-I</strong>|<strong>\-\-install</strong> *ICC\_PROFIL*	Installiere Profil
* <strong>\-t</strong>|<strong>\-\-taxi</strong> *TAXI\_ID*	ICC Taxi Profile DB
* <strong>\-u</strong>|<strong>\-\-user</strong>	Benutzerpfad
* <strong>\-s</strong>|<strong>\-\-system</strong>	Systempfad
* <strong>\-y</strong>|<strong>\-\-oyranos</strong>	Oyranospfad
* <strong>\-m</strong>|<strong>\-\-machine</strong>	Maschinenpfad
* <strong>\-g</strong>|<strong>\-\-gui</strong>	Zeige Grafische Benutzerschnittstelle

### Allgemeine Optionen
&nbsp;&nbsp;**oyranos\-profiles** <strong>\-h</strong> <strong>\-X</strong> *json|json+command|man|markdown*

* <strong>\-h</strong>|<strong>\-\-help</strong>	Hilfe
* <strong>\-X</strong>|<strong>\-\-export</strong> *json|json+command|man|markdown*	Exportiere formatierten Text: Hole Benutzerschnittstelle als Text
   * <strong>\-X man</strong>		# Handbuch
   * <strong>\-X markdown</strong>		# Markdown
   * <strong>\-X json</strong>		# Json
   * <strong>\-X json+command</strong>		# Json + Kommando
   * <strong>\-X export</strong>		# Export
* <strong>\-r</strong>|<strong>\-\-no\-repair</strong>	Lasse Reparatur der Profil ID weg
* <strong>\-v</strong>|<strong>\-\-verbose</strong>	plaudernd

## UMGEBUNGSVARIABLEN
### OY\_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
  <br />
&nbsp;&nbsp;Die \-v Option kann alternativ benutzt werden.
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
## AUTOR
Kai\-Uwe Behrmann http://www.oyranos.org
## KOPIERRECHT
*Copyright 2018 Kai\-Uwe Behrmann*


### Lizenz
newBSD
## FEHLER
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

