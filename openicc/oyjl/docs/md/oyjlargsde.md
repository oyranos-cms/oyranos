# oyjl\-args v1.0.0 {#oyjlargsde}
<a name="toc"></a>
[NAME](#name) [ÜBERSICHT](#synopsis) [BESCHREIBUNG](#description) [OPTIONEN](#options) [BEISPIELE](#examples) [SIEHE AUCH](#seealso) [FORMAT](#format) [AUTOR](#author) [KOPIERRECHT](#copyright) [FEHLER](#bugs) 

<strong>"oyjl-args"</strong> *1* <em>"10. März 2023"</em> "User Commands"

<h2>NAME <a href="#toc" name="name">&uarr;</a></h2>

oyjl-args v1.0.0 - Benutzerschnittstellen nach Quelltext

<h2>ÜBERSICHT <a href="#toc" name="synopsis">&uarr;</a></h2>

<strong>oyjl-args</strong> <a href="#input"><strong>-i</strong>=<em>DATEINAME</em></a> [<strong>--c-stand-alone</strong><em>[=base]</em>] [<strong>--completion-bash</strong>] [<strong>-v</strong>]
<br />
<strong>oyjl-args</strong> <a href="#help"><strong>-h</strong><em>[=synopsis|...]</em></a> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-R</strong>=<em>gui|cli|web|...</em> <strong>-V</strong> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-v</strong>]

<h2>BESCHREIBUNG <a href="#toc" name="description">&uarr;</a></h2>

Werkzeug um Benutzerschnittstellen in JSON von *-X export* nach Quelltext zu übertragen.

<h2>OPTIONEN <a href="#toc" name="options">&uarr;</a></h2>

<h3 id="input">Erzeuge Quelltext</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-args</strong></a> <strong>-i</strong>=<em>DATEINAME</em> [<strong>--c-stand-alone</strong><em>[=base]</em>] [<strong>--completion-bash</strong>] [<strong>-v</strong>]

<table style='width:100%'>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-i</strong>|<strong>--input</strong>=<em>DATEINAME</em></td> <td>Setze Eingabe<br />Für C Quelltext Ausgabe(voreingestellt) und --completion-bash Ausgabe nutze -X=export JSON. Für --render=XXX nutze -X=json JSON. </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--c-stand-alone</strong><em>[=base]</em></td> <td>Erzeuge C Quelltext für oyjl_args.c Referenz.<br />Lasse libOyjlCore Referenz weg.
  <table>
   <tr><td style='padding-left:0.5em'><strong>--c-stand-alone</strong> base</td><td># Basis : Benutze nur OAJL_ARGS_BASE API.</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>--completion-bash</strong></td> <td>Erzeuge Bash Vervollständigungsskript</td> </tr>
</table>

<h3 id="help">Allgemeine Optionen</h3>

&nbsp;&nbsp; <a href="#synopsis"><strong>oyjl-args</strong></a> <strong>-h</strong><em>[=synopsis|...]</em> <strong>-X</strong>=<em>json|json+command|man|markdown</em> <strong>-R</strong>=<em>gui|cli|web|...</em> <strong>-V</strong> [<strong>-i</strong>=<em>DATEINAME</em>] [<strong>-v</strong>]

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
   <tr><td style='padding-left:0.5em'><strong>-X</strong> export</td><td># Export : Alle verfügbaren Daten - Get UI data for developers. The format can be converted by the oyjl-args tool.</td></tr>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-R</strong>|<strong>--render</strong>=<em>gui|cli|web|...</em></td> <td>Wähle Darstellung<br />Wähle und konfiguriere eine Ausgabeform. -R=gui wird eine grafische Ausgabe starten. -R="web:port=port_nummer:https_key=TLS_privater_Schlüssel_Dateiname:https_cert=TLS_CA_Zertifikat_Dateiname:css=Stil_Dateiname.css" wird einen Web Service starten.
  <table>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> gui</td><td># Gui : Zeige UI - Zeige eine interaktive grafische Benutzerschnittstelle.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> cli</td><td># Cli : Zeige UI - Zeige Hilfstext für Benutzerschnittstelle auf der Kommandozeile.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> web</td><td># Web : Starte Web Server - Starte lokalen Web Service für die Darstellung in einem Webbrowser. Die -R=web:help Unteroption zeigt weitere Informationen an.</td></tr>
   <tr><td style='padding-left:0.5em'><strong>-R</strong> -</td>
  </table>
  </td>
 </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-V</strong>|<strong>--version</strong></td> <td>Version</td> </tr>
 <tr><td style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'><strong>-v</strong>|<strong>--verbose</strong></td> <td>plaudernd</td> </tr>
</table>


<h2>BEISPIELE <a href="#toc" name="examples">&uarr;</a></h2>

#### Wandle EntwicklerJSON nach C Quelltext
&nbsp;&nbsp;oyjl-args -X export | oyjl-args -i -

<h2>SIEHE AUCH <a href="#toc" name="seealso">&uarr;</a></h2>

&nbsp;&nbsp;[oyjl](oyjl.html)<a href="oyjl.md">(1)</a>&nbsp;&nbsp;[oyjl-translate](oyjltranslate.html)<a href="oyjltranslate.md">(1)</a>&nbsp;&nbsp;[oyjl-args-qml](oyjlargsqml.html)<a href="oyjlargsqml.md">(1)</a>

&nbsp;&nbsp;<a href="https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html">https://codedocs.xyz/oyranos-cms/oyranos/group__oyjl.html</a>

<h2>FORMAT <a href="#toc" name="format">&uarr;</a></h2>

#### Übersicht
&nbsp;&nbsp;Werkzeuge folgen Regeln.
  <br />
&nbsp;&nbsp;OyjlArgs verlangt, prüft und erzwingt Regeln. Diese Regeln werden in jeder Übersichtszeile ausgedrückt.
  <br />
&nbsp;&nbsp;Ein Werkzeug kann verschiedene Regelgruppen festlegen, welche in einzelnen Übersichtszeilen sitzen.
#### prog -o
&nbsp;&nbsp;Einfaches Werkzeug mit nur einer Option.
  <br />
&nbsp;&nbsp;Die Option besteht aus einem einzelnen Buchstaben und beginnt deshalb mit einem Strich.
  <br />
&nbsp;&nbsp;Das Kommandozeilenwerkzeug wird in den folgenden Beispielen einfach "prog" genannt.
#### prog --schalter
&nbsp;&nbsp;Einfaches Werkzeug mit nur einer Option.
  <br />
&nbsp;&nbsp;Die Option besteht aus mehreren Buchstaben und beginnt mit zwei Strichen.
  <br />
&nbsp;&nbsp;Dies wird ein langer Optionsname genannt. Der gleichen Option kann ein Einbuchstabenname und langer Name zugeordnet sein. Innerhalb der Übersichtszeile wird nur eine Form benannt.
#### prog -o=ARG --name=eins|zwei|...
&nbsp;&nbsp;Einfaches Werkzeug mit zwei Optionen, welche beide Argumente annehmen.
  <br />
&nbsp;&nbsp;Das Argument in Großbuchstaben kann einen Hinweis auf den Inhalt geben, wie DATEI, NUMMER usw. Oder es enthält eine Liste von Auswahlmöglichkeiten, welche durch senkrechten Strich '|' getrennt werden.
  <br />
&nbsp;&nbsp;Die spätere --name Option bezeichnet ein paar Auswahlmöglichkeiten und zeigt mit den unmittelbar folgenden drei Punkten '...', das die Auswahl nicht ausschließlich sind und editiert werden können. OyjlArgs prüft auf Argumente, welche dem Optionsnamen folgen, auch wenn das Istgleichzeichen nicht geschieben wird.
#### prog -o [-v]
&nbsp;&nbsp;Werkzeug mit zwei unterschiedlichen verlangten Optionen.
  <br />
&nbsp;&nbsp;Üblicherweise werden alle Optionen benötigt. Die zweite Option steht in eckigen Klammern und kann benutzt werden. Sie wird optional genannt.
#### prog -h[=synopsis|...] [--option[=NUMMER]]
&nbsp;&nbsp;Werkzeugoptionen, welchen ein Argument nachgestellt sein kann.
#### prog --schalter=auswahl ... [--schater2=auswahl2 ...]
&nbsp;&nbsp;Werkzeugoptionen mit drei Punkten nach einem Leerzeichen ' ...' dürfen mehrfach eingesetzt werden.
  <br />
&nbsp;&nbsp;Kommandozeilenbeispiel: prog -f=datei1.end -f=datei2.end -f datei3.end
#### prog | [-v]
&nbsp;&nbsp;Werkzeug ohne Option.
  <br />
&nbsp;&nbsp;Das Werkzeug kann ohne ein Argument aufgerufen werden. Aber eine Option wäre möglich.
#### prog modul -o [-i] [-v]
&nbsp;&nbsp;Werkzeug mit einem Untermoduloption Muster.
  <br />
&nbsp;&nbsp;Das Werkzeug hat eine lange verpflichtende Option ohne führende Striche.
  <br />
&nbsp;&nbsp;Dieser Stil wird manchmal für komplexe Werkzeuge benutz, um verschiedene Funktionsbereiche zu beschreiben.
#### prog [-v] DATEI ...
&nbsp;&nbsp;Werkzeug mit freien Argumenten zu einer namenlosen Option.
  <br />
&nbsp;&nbsp;Das namenlosse @ Optionsargument wird als letzes genannt, um es nicht mit Untermoduloptionen oder mit Optionsargumenten zu verwechseln.
#### Gramatik für Optionen
&nbsp;&nbsp;Optionen werden einzeln detailierter beschrieben.
  <br />
&nbsp;&nbsp;Optionsnamen mit einem Buchstaben und lange Namen werden beide dargestellt und sind durch einen senkrechten Strich '|' getrennt.
  <br />
&nbsp;&nbsp;Z.B. -o|--option
#### -k|--kelvin=NUMMER        Lambert (NUMMER:0 [≥0 ≤25000 Δ100])
&nbsp;&nbsp;Zeile für ein Nummernargument.
  <br />
&nbsp;&nbsp;Der Einzelbuchstabe und der Langname werden gefolgt von dem symbolischen Nummernnamen. Danach wird eine kurze Bezeichnung wird gedruckt. Auf eine Klammer folgt nochmal der symbolische Nummername mit dem vorgestellten Wert. In quadratischen Klammern folgen ≥ Mindestwert, ≤ Maximalwert und Δ Schrittweite.
#### Kommandozeileninterpret
&nbsp;&nbsp;Der OyjlArgs Kommandozeileninterpret folgt den obigen Regeln.
#### prog -hvi=datei.end
&nbsp;&nbsp;Optionen können auf der Kommandozeile zusammengefügt werden.
  <br />
&nbsp;&nbsp;OyjlArgs interpretiert nach einem einzelnen Minuszeichen jeden Buchstaben als einzelne Option.
  <br />
&nbsp;&nbsp;Die letzte Option darf ein Argument erhalten.
#### prog -i=datei-rein.end -o datei-raus.end
&nbsp;&nbsp;Argumente für Optionen können mit einem Gleichheitszeichen oder nach einem Leerzeichen angehangen werden.
#### prog -f=datei1.end -f datei2.end -f datei3.end
&nbsp;&nbsp;Mehrfachargumente für eine Option benötigen jede den Optionsnamen davor.

<h2>AUTOR <a href="#toc" name="author">&uarr;</a></h2>

Kai-Uwe Behrmann http://www.oyranos.org

<h2>KOPIERRECHT <a href="#toc" name="copyright">&uarr;</a></h2>

*Copyright © 2017-2022 Kai-Uwe Behrmann*


<a name="license"></a>
### Lizenz
newBSD <a href="http://www.oyranos.org">http://www.oyranos.org</a>

<h2>FEHLER <a href="#toc" name="bugs">&uarr;</a></h2>

<a href="https://www.gitlab.com/oyranos/oyranos/issues">https://www.gitlab.com/oyranos/oyranos/issues</a>

