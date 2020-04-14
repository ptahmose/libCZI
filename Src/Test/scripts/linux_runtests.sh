#!/bin/sh

#EXECUTABLE=~/github/libCZI/Src/CZIcmd/CZIcmd
#EXECUTABLE=~/github/libCZI/Src/CZICmd/CZIcmd
EXECUTABLE=~/Dev/New/buildlibCZI/Src/CZICmd/CZIcmd
#SRCIMAGES=/home/jbohl/atom2/hdd/2/tmp/TestData/
SRCIMAGES=/mnt/atom2/hdd/2/TestData/
OUTIMAGES=~/testOut/

if [ -z "$1"] 
then
  python runtests.py -s $SRCIMAGES -d $OUTIMAGES -e $EXECUTABLE -t ./testdata_linux.txt 
else
  python runtests.py -s $SRCIMAGES -d $OUTIMAGES -e $EXECUTABLE -t ./testdata_linux.txt --skip $1
fi

