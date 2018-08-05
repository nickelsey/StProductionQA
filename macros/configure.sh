#!/bin/sh

mkdir libs
mkdir sandbox
mkdir -p scheduler/report
mkdir -p scheduler/list
mkdir -p scheduler/csh
mkdir -p log
mkdir -p out
mkdir -p tmplogs
cp -r StRoot sandbox/
cp -r macros/*cxx sandbox/
