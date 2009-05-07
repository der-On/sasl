#! /bin/sh

# prepare directory
VERSION=`awk '/VERSION_MAJOR/ { major=$3 } /VERSION_MINOR/ { minor=$3 } /VERSION_PATCH/ { patch=$3 } END { print major "." minor "." patch }'` < version.h
DIR=xap-$VERSION

if [ -d tmp ] ; then
    rm -rf tmp
fi
mkdir tmp
mkdir tmp/$DIR
mkdir tmp/$DIR/src


# build libavionics
cd libavionics
make clean
cp -r ../libavionics ../tmp/$DIR/src/
make
if [ $? != 0 ] ; then
    echo "Can't build libavionics!"
    exit -1
fi

# build x-plane plugin
cd ../xap
make clean
cp -r ../xap ../tmp/$DIR/src/
make
if [ $? != 0 ] ; then
    echo "Can't build xap!"
    exit -1
fi
cd ..

cd tmp
cp -r ../doc $DIR/
cp -r ../examples $DIR/
cp ../version.h $DIR/

mkdir $DIR/xap
cp ../xap/lin.xpl $DIR/xap/
strip --strip-all $DIR/xap/lin.xpl
cp /mnt/windows/asso/ngauges/xap/win.xpl $DIR/xap/
cp -r ../data $DIR/xap


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
        zip -r -9 ../distr/$t.zip $t
        cd ../examples
    fi
done
cd ..

rm -rf tmp

