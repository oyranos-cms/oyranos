# OpenICC Options Renderer

A QML based renderer of OpenICC options in JSON format.

### Dependencies
* [Qt](http://qt.io) - a cross platform framework

### Build Android App
Be careful to use the boilerplate from the oyjl-args tool.
Here described are the incredients using Qt Creator.
* Give the Android project a simple name.
* Use the Android project name inside the AppActivity.java and
  adapt the path name to the project name.
  project name: myapp
  android/src/org/oyranos/myapp/AppActivity.java: package org.oyranos.myapp;
* Take the same compiler from Qt libs for all required libraries.
  Otherwise it is possible to run in header issues, e.g.
  undefined stdout.
* Add a dummy.qml file to trigger importing all required
  QML files.
* Translation: add a myapp.i18n.c file containing your translations only.
  Generation example: LANG=C oyranos-profile-graph -X json | oyjl-translate -a -i - -k name,description,help,label -d oyranos,oyjl -p `pwd`/build/locale -l de_DE,es_ES -t -w C
  Include the above obtained text as header and pass the text array to
  oyjlArgsRender(oyranos_json).
* Ison: add a myapp.qrc file pointing to your logo.svg
