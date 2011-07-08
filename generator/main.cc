#include <iostream>

#include <QDir>

#include <QtDebug>

#include "config.h"
#include "ClassGenerator.h"

using namespace std;


int main(int argc, char *argv[])
{
  if (QString(argv[1]) == "-h" ||
      QString(argv[1]) == "--help") {
    cout << "Usage 1: " << argv[0] << endl;
    cout << "Usage 2: " << argv[0] << " <template dir1>:<sources dir1>"
                                   << " [<template dir2>:<sources dir2>] ..."  << endl;
    return 0;
  }

  QHash<QString,QString> dirMap;
  if (argc == 1)
    dirMap[TEMPLATES_STD_DIR] = SOURCES_STD_DIR;
  else {
    for (int arg = 1; arg<argc; arg++) {
      QString argument( argv[arg] );
      QDir templatesDir( argument.section( ':', 0,0 ) );
      QDir sourcesDir( argument.section( ':', 1,1 ) );

      if (!templatesDir.exists()) {
        qCritical() << "Directory" << templatesDir.path() << "does not exist";
        return 1;
      }
      if (!sourcesDir.exists()) {
        qCritical() << "Directory" << sourcesDir.path() << "does not exist";
        return 1;
      }

      dirMap[templatesDir.canonicalPath()] = sourcesDir.canonicalPath();
    }
  }

  QDir outputDir( OUTPUT_STD_DIR );
  if (!outputDir.exists()) {
    qDebug() << "Creating directory" << outputDir.path();
    QDir currentDir;
    if (!currentDir.mkdir(outputDir.path())) {
      qCritical() << "Directory" << outputDir.path() << "cannot be created";
      return 1;
    }
  }

  ClassGenerator cg( dirMap, outputDir.canonicalPath() );

  cg.initTemplates();
  qDebug() << "";
  cg.render();

  return 0;
}
