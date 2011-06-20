#!/bin/bash

function grp_color() {
  case $1 in
    cmm_handling)       COLOR=1 ;;
    module_api)         COLOR=2 ;;
    objects_conversion) COLOR=3 ;;
    objects_generic)    COLOR=4 ;;
    objects_value)      COLOR=5 ;;
    objects_profile)    COLOR=6 ;;
  esac
}

function ctor() {
  PATTERN="Implement the constructor for oy${1}_s"
  git log --oneline | grep -q "$PATTERN" && c=2 || c=1
  tput setaf $c
  echo -n C
  tput sgr0
}

function dtor() {
  PATTERN="Implement the destructor for oy${1}_s"
  git log --oneline | grep -q "$PATTERN" && c=2 || c=1
  tput setaf $c
  echo -n D
  tput sgr0
}

function cctor() {
  PATTERN="Implement the copy constructor for oy${1}_s"
  git log --oneline | grep -q "$PATTERN" && c=2 || c=1
  tput setaf $c
  echo -n C
  tput sgr0
}

function privateAPI() {
  PATTERN="Import private methods for oy${1}_s"
  git log --oneline | grep -q "$PATTERN" && c=2 || c=1
  tput setaf $c
  echo -n Private
  tput sgr0
}

function publicAPI() {
  PATTERN="Import public methods for oy${1}_s"
  git log --oneline | grep -q "$PATTERN" && c=2 || c=1
  tput setaf $c
  echo -n Public
  tput sgr0
}

function print_list() {
  for d in sources/*dox sources_weg/*dox; do
    GROUP=$(grep '@ingroup' $d | awk '{print $3}')
    CLASS=$(basename $d .dox)
    test $CLASS = "Class" && continue
    grp_color $GROUP
    tput setaf $COLOR
    echo -n $GROUP
    tput sgr0
    echo -en ": $CLASS\t\t\t["
    ctor $CLASS
    dtor $CLASS
    cctor $CLASS
    echo -n ']'
    echo -n ', Imported API: ['
    publicAPI $CLASS
    echo -n ':'
    privateAPI $CLASS
    echo ']'
  done
}

print_list | sort
