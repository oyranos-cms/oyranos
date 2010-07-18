#!/bin/bash

rm -fr API_generated/*

./oyAPIGenerator

cd API_generated/

cmake .
make
