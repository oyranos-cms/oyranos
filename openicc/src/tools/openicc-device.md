# openicc\-device v0.1.1 {#openiccdevice}
*"openicc\-device"* *1* *""* "User Commands"
## NAME
openicc\-device v0.1.1 \- OpenICC Device
## SYNOPSIS
**openicc\-device** \-l [\-d *NUMBER*] [\-j] [\-n] [\-b *FILENAME*] [\-v]
<br />
**openicc\-device** \-a \-f *FILENAME* [\-b *FILENAME*] [\-v]
<br />
**openicc\-device** \-e \-d *NUMBER* [\-b *FILENAME*] [\-v]
<br />
**openicc\-device** \-p [\-s] [\-v]
<br />
## DESCRIPTION
Manipulation of OpenICC color management data base device entries.
## OPTIONS
### Print the Devices in the DB
**openicc\-device** \-l [\-d *NUMBER*] [\-j] [\-n] [\-b *FILENAME*] [\-v]

* \-l|\-\-list\-devices	List Devices
* \-d|\-\-device *NUMBER*	Device position
* \-j|\-\-dump\-json	Dump OpenICC JSON
* \-n|\-\-long	List all key/values pairs
* \-b|\-\-db\-file *FILENAME*	DB File Name: File Name of OpenICC Device Data Base JSON

### Add Device to DB
**openicc\-device** \-a \-f *FILENAME* [\-b *FILENAME*] [\-v]

* \-a|\-\-add	Add Device to DB
* \-f|\-\-file\-name *FILENAME*	File Name: The File Name of the OpenICC Device in Json format.
* \-b|\-\-db\-file *FILENAME*	DB File Name: File Name of OpenICC Device Data Base JSON

### Erase a Devices from the DB
**openicc\-device** \-e \-d *NUMBER* [\-b *FILENAME*] [\-v]

* \-e|\-\-erase\-device	Erase Devices
* \-d|\-\-device *NUMBER*	Device position
* \-b|\-\-db\-file *FILENAME*	DB File Name: File Name of OpenICC Device Data Base JSON

### Show Filepath to the DB
**openicc\-device** \-p [\-s] [\-v]

* \-p|\-\-show\-path	Show Path
* \-s|\-\-scope	System

### General options

* \-b|\-\-db\-file *FILENAME*	DB File Name: File Name of OpenICC Device Data Base JSON
* \-X|\-\-export *json|json+command|man|markdown*	Export formated text: Get UI converted into text formats
   * \-X json		# Json
   * \-X json+command		# Json + Command
   * \-X man		# Man
   * \-X markdown		# Markdown
* \-v|\-\-verbose	verbose
* \-h|\-\-help	Help

## AUTHOR
Kai\-Uwe Behrmann http://www.openicc.org
## COPYRIGHT
*Copyright 2018 Kai\-Uwe Behrmann*


### License
newBSD
## BUGS
[https://www.github.com/OpenICC/config/issues](https://www.github.com/OpenICC/config/issues)

