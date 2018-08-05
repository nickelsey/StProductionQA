#!/bin/tcsh

# builds libraries from scratch
#
# Currently builds:
# 1) StEfficiencyMaker

echo "[i] loading embedding library"
starver pro

echo "[i] Remove any existing libs"
rm -v libStProductionQA.so
rm -v StProductionQA.so
rm -v libs/libStProductionQA.so
rm -v libs/StProductionQA.so
rm -v sandbox/libStProductionQA.so
rm -v sandbox/StProductionQA.so
rm -v libStProductionTrackQA.so
rm -v StProductionTrackQA.so
rm -v libs/libStProductionTrackQA.so
rm -v libs/StProductionTrackQA.so
rm -v sandbox/libStProductionTrackQA.so
rm -v sandbox/StProductionTrackQA.so

setenv CXXFLAGSNEW "-pipe -fPIC -Wall -Woverloaded-virtual -ansi -Wno-long-long -pthread -m32 -std=c++11"
#LDFLAGS       += -m32

echo "[i] Changing CXXFLAGS to: "${CXXFLAGSNEW}

echo "[i] Running cons for StProductionQA"

cons CXXFLAGS="${CXXFLAGSNEW}" +StProductionQA +StProductionTrackQA


# places copies of the libraries into the local lib directory
# as well as into sandbox/
echo "[i] Copying libraries to the lib & sandbox"
find .sl*/lib -name "libStProductionQA.so" -exec cp -v {} ./libs/ \;
find .sl*/lib -name "libStProductionQA.so" -exec cp -v {} ./sandbox/ \;
find .sl*/lib -name "libStProductionTrackQA.so" -exec cp -v {} ./libs/ \;
find .sl*/lib -name "libStProductionTrackQA.so" -exec cp -v {} ./sandbox/ \;


echo "[i] Run a zero level test"
root4star macros/LoadLibs.C -q -b
