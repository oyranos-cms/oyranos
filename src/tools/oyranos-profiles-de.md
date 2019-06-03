# oyranos\-profiles v0.9.7 {#oyranosprofilesde}
*"oyranos\-profiles"* *1* *"October 11, 2018"* "User Commands"
## NAME
oyranos\-profiles v0.9.7 \- Oyranos Profile
## ÜBERSICHT
**oyranos\-profiles** \-l [\-f] [\-e] [\-a] [\-c] [\-d] [\-k] [\-n] [\-o] [\-i] [\-2] [\-4] [\-P] [\-T *SCHLÜSSEL;WERT*] [\-v]
<br />
**oyranos\-profiles** \-p [\-u|\-s|\-y|\-m] [\-v]
<br />
**oyranos\-profiles** \-I *ICC\_PROFIL*|  \-t *TAXI\_ID* [\-u|\-s|\-y|\-m] [\-g] [\-v]
<br />
**oyranos\-profiles** \-h \-X *json|json+command|man|markdown*
## BESCHREIBUNG
Das Werkzeug kann installierte Profile und Suchpfade anzeigen. Es kann ein Profil in einen Suchpfad installieren.
## OPTIONEN
### Liste der verfügbaren ICC Farbprofile
**oyranos\-profiles** \-l [\-f] [\-e] [\-a] [\-c] [\-d] [\-k] [\-n] [\-o] [\-i] [\-2] [\-4] [\-P] [\-T *SCHLÜSSEL;WERT*] [\-v]

* \-l|\-\-list\-profiles	Liste Profile
* \-f|\-\-full\-names	Profildateiname: Zeige kompletten Pfad\- und Dateinamen
* \-e|\-\-internal\-names	Interne Namen: Der Text kommt vom 'desc' Profilelement.
* \-a|\-\-abstract	Wähle abstrakte Profile
* \-c|\-\-color\-space	Wähle Farbraumprofile
* \-d|\-\-display	Wähle Monitorprofile
* \-k|\-\-device\-link	Wähle Verknüpfungsprofil
* \-n|\-\-named\-color	Wähle Einzelfarbprofile
* \-o|\-\-output	Wähle Ausgabeprofil
* \-i|\-\-input	Wähle Eingangsprofile
* \-2|\-\-icc\-version\-2	Wähle ICC V2 Profile
* \-4|\-\-icc\-version\-4	Wähle ICC V4 Profile
* \-P|\-\-path	Zeige Profile, welche ein Wort in ihrem Pfadnamen enthalten.: PFAD\_WORT\_TEIL
* \-T|\-\-meta *SCHLÜSSEL;WERT*	Suche nach Schlüssel/Werte Paar in Meta Element: Zeige Profile welche ein bestimmtes Schlüssel/Werte Paar im meta Tag enthalten. WERT kann '*' enthalten für das Filtern von Wortteilen.
* \-D|\-\-duplicates	Zeige identische mehrfach installierte Profile an

### Zeige Suchpfade
**oyranos\-profiles** \-p [\-u|\-s|\-y|\-m] [\-v]

* \-p|\-\-list\-paths	Liste der ICC Farbprofilpfade
* \-u|\-\-user	Benutzerpfad
* \-s|\-\-system	Systempfad
* \-y|\-\-oyranos	Oyranospfad
* \-m|\-\-machine	Maschinenpfad

### Installiere Profil
**oyranos\-profiles** \-I *ICC\_PROFIL*|  \-t *TAXI\_ID* [\-u|\-s|\-y|\-m] [\-g] [\-v]

* \-I|\-\-install *ICC\_PROFIL*	Installiere Profil
* \-t|\-\-taxi *TAXI\_ID*	ICC Taxi Profile DB
* \-u|\-\-user	Benutzerpfad
* \-s|\-\-system	Systempfad
* \-y|\-\-oyranos	Oyranospfad
* \-m|\-\-machine	Maschinenpfad
* \-g|\-\-gui	Zeige Grafische Benutzerschnittstelle

### Allgemeine Optionen
**oyranos\-profiles** \-h \-X *json|json+command|man|markdown*

* \-h|\-\-help	Hilfe
* \-X|\-\-export *json|json+command|man|markdown*	Exportiere formatierten Text: Hole Benutzerschnittstelle als Text
   * \-X json		# Json
   * \-X json+command		# Json + Kommando
   * \-X man		# Handbuch
   * \-X markdown		# Markdown
* \-r|\-\-no\-repair	Lasse Reparatur der Profil ID weg
* \-v|\-\-verbose	plaudernd

## UMGEBUNGSVARIABLEN
### OY\_DEBUG
Setze das Oyranos Fehlersuchniveau. Die \-v Option kann alternativ benutzt werden. Der gültige Bereich ist 1\-20.
### XDG\_DATA\_HOME XDG\_DATA\_DIRS
route Oyranos to top directories containing resources. The derived paths for ICC profiles have a "color/icc" appended. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal  
## BEISPIELE
### Liste alle installierten Profile mit internem Namen
oyranos\-profiles \-le 
### Liste alle installierten Profile mit Monitor\-und Ausgabeklasse
oyranos\-profiles \-l \-od 
### Liste alle installierten Profile im Benutzerpfad
oyranos\-profiles \-lfu 
### Installiere ein Profile für den aktuellen Benutzer und zeige eine Fehlermeldung grafische an
oyranos\-profiles \-\-install profilename \-u \-\-gui 
### Installiere ein Profile für den aktuellen Benutzer und zeige eine Fehlermeldung grafische an
oyranos\-profiles \-\-install \- \-\-taxi=taxi\_id/0 \-\-gui \-d \-u 
### Zeige Dateiinfos
SAVEIFS=$IFS ; IFS=$'\n\b'; profiles=(\`oyranos\-profiles \-ldf\`); IFS=$SAVEIFS; for file in "${profiles[@]}"; do ls "$file"; done  
## AUTOR
Kai\-Uwe Behrmann http://www.oyranos.org
## KOPIERRECHT
*Copyright 2018 Kai\-Uwe Behrmann*


### Lizenz
newBSD
## FEHLER
[https://www.github.com/oyranos\-cms/oyranos/issues](https://www.github.com/oyranos\-cms/oyranos/issues)

