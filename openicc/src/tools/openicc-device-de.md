# openicc\-device v0.1.1 {#openiccdevicede}
*"openicc\-device"* *1* *""* "User Commands"
## NAME
openicc\-device v0.1.1 \- OpenICC Device
## ÜBERSICHT
**openicc\-device** \-l [\-d *NUMMER*] [\-j] [\-n] [\-b *DATEINAME*] [\-v]
<br />
**openicc\-device** \-a \-f *DATEINAME* [\-b *DATEINAME*] [\-v]
<br />
**openicc\-device** \-e \-d *NUMMER* [\-b *DATEINAME*] [\-v]
<br />
**openicc\-device** \-p [\-s] [\-v]
<br />
## BESCHREIBUNG
Bearbeitung von Geräteeinträgen in der Datenbank für OpenICC Farbmanagement
## OPTIONEN
### Zeige die Geräte aus der DB
**openicc\-device** \-l [\-d *NUMMER*] [\-j] [\-n] [\-b *DATEINAME*] [\-v]

* \-l|\-\-list\-devices	Zeige Geräte
* \-d|\-\-device *NUMMER*	Gerätebeposition
* \-j|\-\-dump\-json	Schreibe Json
* \-n|\-\-long	Zeige alle Schlüssel/Wertepaare

### Trage Gerät in die DB ein
**openicc\-device** \-a \-f *DATEINAME* [\-b *DATEINAME*] [\-v]

* \-a|\-\-add	Trage Gerät in die DB ein
* \-f|\-\-file\-name *DATEINAME*	Dateiname
   * \-f device\-file\-name.json		# Gerätedatei

### Lösche Gerät von der DB
**openicc\-device** \-e \-d *NUMMER* [\-b *DATEINAME*] [\-v]

* \-e|\-\-erase\-device	Lösche Geräte
* \-d|\-\-device *NUMMER*	Gerätebeposition

### Zeige den Dateipfad zur DB
**openicc\-device** \-p [\-s] [\-v]

* \-p|\-\-show\-path	Zeige Pfad
* \-s|\-\-scope	System

### Allgemeine Optionen

* \-b|\-\-db\-file *DATEINAME*	DB Dateiname
   * \-b DB\-file\-name.json		# DB Datei
* \-X|\-\-export *json|json+command|man|markdown*	Exportiere formatierten Text: Hole Benutzerschnittstelle als Text
   * \-X json		# Json
   * \-X json+command		# Json + Kommando
   * \-X man		# Handbuch
   * \-X markdown		# Markdown
* \-v|\-\-verbose	plaudernd
* \-h|\-\-help	Hilfe

## AUTOR
Kai\-Uwe Behrmann http://www.openicc.org
## KOPIERRECHT
*Copyright 2018 Kai\-Uwe Behrmann*


### Lizenz
newBSD
## FEHLER
[https://www.github.com/OpenICC/config/issues](https://www.github.com/OpenICC/config/issues)

