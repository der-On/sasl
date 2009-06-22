#! /bin/sh
# Shotgun builder for all of the OS X components
cd libavionics; make clean; make -f Makefile.mac
cd ../slava; make clean; make -f Makefile.mac
cd ../xap; make clean; make -f Makefile.mac
cp mac.xpl "/Users/julik/Desktop/X-Plane 9/Aircraft/IL-14_930/plugins/xap/mac.xpl"