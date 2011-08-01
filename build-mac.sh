#! /bin/sh
# Shotgun builder for all of the OS X components
cd libogl; make clean -f Makefile.mac; make -f Makefile.mac
cd libavionics; make clean -f Makefile.mac; make -f Makefile.mac
# skip slava compilation
# cd ../slava; make clean; make -f Makefile.mac
cd ../xap; make clean -f Makefile.mac; make -f Makefile.mac
# cp mac.xpl "/Users/julik/Desktop/X-Plane-Beta/Aircraft/IL-14_930/plugins/xap/mac.xpl"