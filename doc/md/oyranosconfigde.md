# oyranos-config v0.9.7 {#oyranosconfigde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [ALLGEMEINE OPTIONEN](#general_options) [UMGEBUNGSVARIABLEN](#environmentvariables) [BEISPIELE](#examples) [SIEHE AUCH](#seeaswell) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyranos-config"</strong> *1* <em>"September 23, 2020"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyranos-config v0.9.7 - Konfiguration

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyranos-config</strong> <a href="#get"><strong>-g</strong>=<em>XPATH</em></a> | <strong>-s</strong>=<em>XPATH:WERT</em> | <strong>-l</strong> | <strong>--dump-db</strong> | <strong>-p</strong> [<strong>-v</strong>] [<strong>-z</strong>]
<br />
<strong>oyranos-config</strong> <a href="#daemon"><strong>-d</strong>=<em>0|1</em></a> [<strong>-v</strong>]
<br />
<strong>oyranos-config</strong> <a href="#syscolordir"><strong>--syscolordir</strong></a> | <strong>--usercolordir</strong> | <strong>--iccdirname</strong> | <strong>--settingsdirname</strong> | <strong>--cmmdir</strong> | <strong>--metadir</strong> [<strong>-v</strong>] [<strong>-z</strong>]
<br />
<strong>oyranos-config</strong> <a href="#Version"><strong>--Version</strong></a> | <strong>--api-version</strong> | <strong>--num-version</strong> | <strong>--git-version</strong> [<strong>-v</strong>]
<br />
<strong>oyranos-config</strong> <a href="#cflags"><strong>--cflags</strong></a> | <strong>--ldflags</strong> | <strong>--ldstaticflags</strong> | <strong>--sourcedir</strong> | <strong>--builddir</strong> [<strong>-v</strong>]
<br />
 <a href="#synopsis"><strong>oyranos-config</strong></a> <a href="#export"><strong>-X</strong>=<em>json|json+command|man|markdown</em></a> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|web|...</em> [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Das Werkzeug zeigt und ändert OpenICC DB Einstellungen, und es zeigt Pfade und statische Informationen.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="get">Einstellungen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>-g</strong>=<em>XPATH</em> | <strong>-s</strong>=<em>XPATH:WERT</em> | <strong>-l</strong> | <strong>--dump-db</strong> | <strong>-p</strong> [<strong>-v</strong>] [<strong>-z</strong>]

&nbsp;&nbsp;Handhabe OpenICC Konfigurations DB auf niedriger Ebene.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-g</strong>|<strong>--get</strong>=<em>XPATH</em></td> <td>Lese einen Wert </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-s</strong>|<strong>--set</strong>=<em>XPATH:WERT</em></td> <td>Setze einen Wert </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-l</strong>|<strong>--list</strong></td> <td>Zeige vorhandene Pfade innerhalb der OpenICC DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--dump-db</strong></td> <td>Ausgabe der OpenICC DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-p</strong>|<strong>--path</strong></td> <td>Zeige DB Dateipfad</td> </tr>
</table>

<h3 id="daemon">Beobachte Einstellungsänderungen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>-d</strong>=<em>0|1</em> [<strong>-v</strong>]

&nbsp;&nbsp;Arbeitet nur auf der Kommandozeile.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-d</strong>|<strong>--daemon</strong>=<em>0|1</em></td> <td>Beobachte Einstellungsänderungen
  <table>
   <tr><td style='padding-left:0.5em'><strong>-d 0</strong></td><td># Deaktiviere</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-d 1</strong></td><td># Aktiviere</td></tr>
  </table>
  </td>
 </tr>
</table>

<h3 id="syscolordir">Zeige Installationspfade</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>--syscolordir</strong> | <strong>--usercolordir</strong> | <strong>--iccdirname</strong> | <strong>--settingsdirname</strong> | <strong>--cmmdir</strong> | <strong>--metadir</strong> [<strong>-v</strong>] [<strong>-z</strong>]

&nbsp;&nbsp;Zeige statisch konfigurierte und unterstützte Pfade von Oyranos CMS.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--syscolordir</strong></td> <td>Pfad zum Hauptfarbverzeichnis im System</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--usercolordir</strong></td> <td>Pfad zum Hauptfarbverzeichnis des Benutzers</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--iccdirname</strong></td> <td>ICC Profilpfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--settingsdirname</strong></td> <td>Oyranos Einstellungspfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--cmmdir</strong></td> <td>Oyranos CMM Pfadname</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--metadir</strong></td> <td>Oyranos Metamodulpfad</td> </tr>
</table>

<h3 id="Version">Zeige Version</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>--Version</strong> | <strong>--api-version</strong> | <strong>--num-version</strong> | <strong>--git-version</strong> [<strong>-v</strong>]

&nbsp;&nbsp;Die Veröffentlichungsversion wird gebildet aus den Nummern zuHaupt(Major|API|ABI)-Neben(Minor|neue API)-Mini(Micro|Fehlerbehebung). Zur Orientierung in git gibt es die Versionsnummer der letzen Veröffentlichung, die Eintragsnummer, die SHA1 Identifikation und das Datum als Jahr-Monat-Tag.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--Version</strong></td> <td>Zeige offizielle Version<br />API|ABI-neueAPI-FehlerKorrektur Veröffentlichung</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--api-version</strong></td> <td>Zeige Version der API</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--num-version</strong></td> <td>Zeige Version als einfache Zahl<br />10000*API+100*Feature+Patch</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--git-version</strong></td> <td>Zeige Version wie in git<br />letzteVeröffentlichungsVersion-gitEintragsNummer-gitEintragsSHA1ID-Jahr-Monat-Tag</td> </tr>
</table>

<h3 id="cflags">Sonstige Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>--cflags</strong> | <strong>--ldflags</strong> | <strong>--ldstaticflags</strong> | <strong>--sourcedir</strong> | <strong>--builddir</strong> [<strong>-v</strong>]

&nbsp;&nbsp;Die Texte dienen dem Übersetzen von Programmen.

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--cflags</strong></td> <td>Übersetzungsargumente</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--ldflags</strong></td> <td>dynamische Verknüpfungsargumente</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--ldstaticflags</strong></td> <td>statische Verknüpfungsargumente</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--sourcedir</strong></td> <td>Oyranos lokaler Quelltextpfad</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--builddir</strong></td> <td>Oyranos lokaler Übersetzungpfad</td> </tr>
</table>


<h2>ALLGEMEINE OPTIONEN <a href="#toc" name="general_options">&uarr;</a></h2>

<h3 id="export">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyranos-config</strong></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> | <strong>-h</strong><em>[=synopsis|...]</em> | <strong>-V</strong> | <strong>-R</strong>=<em>gui|web|...</em> [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-h</strong>|<strong>--help</strong><em>[=synopsis|...]</em></td> <td>Zeige Hilfetext an<br />Zeige Benutzungsinformationen und Hinweise für das Werkzeug.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-h -</strong></td><td># Vollständige Hilfe</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-h synopsis</strong></td><td># Übersicht</td></tr>
  </table>
  </td>
 </tr>
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
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|web|...</em></td> <td>Wähle Darstellung<br />Wähle und konfiguriere eine Ausgabeform. -R=gui wird eine grafische Ausgabe starten. -R="port_number:api_path:TLS_private_key:TLS_CA_certificate:style.css" wird einen Web Service starten.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R gui</strong></td><td># Gui</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R web</strong></td><td># Web</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R -</strong></td><td># </td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-z</strong>|<strong>--system-wide</strong></td> <td>Einstellung in der systemweiten DB</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>


<h2>UMGEBUNGSVARIABLEN <a href="#toc" name="environmentvariables">&uarr;</a></h2>

#### OY_DEBUG
&nbsp;&nbsp;Setze das Oyranos Fehlersuchniveau.
  <br />
&nbsp;&nbsp;Die -v Option kann alternativ benutzt werden.
  <br />
&nbsp;&nbsp;Der gültige Bereich ist 1-20.
#### OY_MODULE_PATH
&nbsp;&nbsp;zeige Oyranos zusätzliche Verzeichnisse mit Modulen.

<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Zeige einen Einstellungswert
&nbsp;&nbsp;oyranos-config -g org/freedesktop/openicc/behaviour/effect_switch
#### Ändere eine Einstellung
&nbsp;&nbsp;oyranos-config -s org/freedesktop/openicc/behaviour/effect_switch:1
#### Zeige alle Einstellungswerte
&nbsp;&nbsp;oyranos-config -l -v
#### Beobachte Änderungen
&nbsp;&nbsp;oyranos-config -d 1 -v > log-datei.txt
#### Übersetze ein einfaches Programm
&nbsp;&nbsp;cc `oyranos-config --cflags` meineDatei.c `oyranos-config --ldflags` -o meinProgramm
#### Zeige systemsichtbare Profile im Oyranos Installationspfad
&nbsp;&nbsp;ls `oyranos-config --syscolordir --iccdirname`

<h2>SIEHE AUCH <a href="#toc" name="seeaswell">&uarr;</a></h2>

&nbsp;&nbsp;[oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>&nbsp;&nbsp;[oyranos-config-synnefo](oyranosconfigsynnefo.html)<a href="oyranosconfigsynnefo.md">(1)</a>&nbsp;&nbsp;[oyranos](oyranos.html)<a href="oyranos.md">(3)</a>

 http://www.oyranos.org


<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*© 2005-2020 Kai-Uwe Behrmann and others*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.github.com/oyranos-cms/oyranos/issues">https://www.github.com/oyranos-cms/oyranos/issues</a>

