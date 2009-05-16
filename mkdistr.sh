#! /bin/sh

# prepare directory
VERSION=`awk '/VERSION_MAJOR/ { major=$3 } /VERSION_MINOR/ { minor=$3 } /VERSION_PATCH/ { patch=$3 } END { print major "." minor "." patch }'` < version.h

# enable snapshot mode
if [ "x$1" != "x" ] ; then
    VERSION=${VERSION}-$1
    export CXXFLAGS=-DSNAPSHOT=$1
fi

DIR=xap-$VERSION

if [ -d tmp ] ; then
    rm -rf tmp
fi
mkdir tmp
mkdir tmp/$DIR


# build libavionics
cd libavionics
make clean
make 
if [ $? != 0 ] ; then
    echo "Can't build libavionics!"
    exit -1
fi

# build x-plane plugin
cd ../xap
make clean
make 
if [ $? != 0 ] ; then
    echo "Can't build xap!"
    exit -1
fi

# build slava
cd ../slava
make clean
make 
if [ $? != 0 ] ; then
    echo "Can't build slava!"
    exit -1
fi
cd ..

cd tmp
cp -r ../doc $DIR/

mkdir $DIR/xap
cp ../xap/lin.xpl $DIR/xap/
strip --strip-all $DIR/xap/lin.xpl
cp /mnt/windows/asso/sasl/xap/win.xpl $DIR/xap/
cp -r ../data $DIR/xap
find $DIR -name .svn -exec rm -rf {} ';'

# make archive
if [ ! -d ../distr ] ; then
    mkdir ../distr
fi
if [ -f ../distr/${DIR}.zip ] ; then
    rm ../distr/${DIR}.zip
fi
zip -r -9 ../distr/${DIR}.zip $DIR
cd ..

# make aircrafts distributions

cd examples
for t in * ; do
    if [ -d $t ] ; then
        cp -R $t ../tmp
        cd ../tmp
        rm $t/plugins
        mkdir $t/plugins
        cp -R $DIR/xap $t/plugins
        if [ -d $t/doc ] ; then
            rm -f $t/doc/*.odt
        fi
        rm -f ../distr/$t.zip
        find . -name .svn -exec rm -rf {} ';'
        zip -r -9 ../distr/$t.zip $t
        cd ../examples
    fi
done
cd ..

# make slava distr
cd tmp
DIR=slava-$VERSION
mkdir ${DIR}
cp ../slava/slava ${DIR}
strip --strip-all ${DIR}/slava
cp ../slava/slava-dist.sh ${DIR}/slava.sh
cp ../slava/slava-dist.bat ${DIR}/slava.bat
cp /mnt/windows/asso/sasl/slava/slava.exe ${DIR}
cp ../../SDL.dll ${DIR}
cp -r ../data ${DIR}
find ${DIR} -name .svn -exec rm -rf {} ';'
if [ -f ../distr/${DIR}.zip ] ; then
    rm ../distr/${DIR}.zip
fi
zip -r -9 ../distr/${DIR}.zip $DIR
cd ..


rm -rf tmp

