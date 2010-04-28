#!/bin/bash

mkdir -p src
rm -fr src/*

./oyAPIGenerator templates/ src/
