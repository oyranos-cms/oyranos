#/bin/sh


CONF_H=config.h
CONF=config
VERSION_A=0
VERSION_B=0
VERSION_C=1
VERSION=$VERSION_A.$VERSION_B$VERSION_C
VERSION_L=$VERSION_A.$VERSION_B.$VERSION_C
TIME="November 2004 - April 2005"

prefix=/opt/local


test -f error.txt && rm error.txt
test -f config && make clean


echo "# automatically generated file - do not edit" > $CONF
echo "/* automatically generated file - do not edit */" > $CONF_H
echo "prefix = $prefix" >> $CONF

echo ""

UNAME_=`uname`
if [ $? = 0 ] && [ $UNAME_ = "Darwin" ]; then
  echo "Darwin PPC              detected"
  echo "APPLE = 1" >> $CONF
  echo "OSX_H = -DHAVE_OSX" >> $CONF
else
  if [ $UNAME_ = "Linux" ]; then
    echo "LINUX = 1" >> $CONF
    echo "Linux system            detected"
  else
    if [ $UNAME_ = "SunOS" ]; then
      echo "SOLARIS = 1" >> $CONF
      echo "SunOS                   detected"
    else
      echo "Oyranos may or may not compile on your $UNAME_ system"
    fi
  fi
fi

FLTK_=`fltk-config --cxxflags 2>>error.txt`
if [ $? = 0 ] && [ -n "$FLTK_" ]; then
  echo "FLTK `fltk-config --version`              detected"
  echo "#define HAVE_FLTK" >> $CONF_H
  echo "FLTK = 1" >> $CONF
  echo "FLTK_H = -DHAVE_FLTK `fltk-config --cxxflags`" >> $CONF
  echo "FLTK_LIBS = `fltk-config --use-images --use-gl --use-glut --ldflags`" >> $CONF
else
  echo "FLTK is not found; you optionally can download at: www.fltk.org"
  echo "to use the gui you need as well the FLU extensions to fltk"
fi


FLU_=`flu-config --cxxflags 2>>error.txt`
if [ $? = 0 ] && [ -n "$FLU_" ] && [ -n "$FLTK_" ]; then
  echo "FLU                     detected"
  echo "#define HAVE_FLU" >> $CONF_H
  echo "FLU = 1" >> $CONF
  echo "FLU_H = -DHAVE_FLU `flu-config --cxxflags`" >> $CONF
  echo "FLU_LIBS = `flu-config --ldflags`" >> $CONF
else
  echo "no FLU found, will not use it"
fi



`pkg-config  --atleast-version=1.14 lcms`
if [ $? = 0 ] && [ 0 = 1 ]; then
  echo "littleCMS `pkg-config --modversion lcms`          detected"
  echo "#define HAVE_LCMS" >> $CONF_H
  echo "LCMS = 1" >> $CONF
  echo "LCMS_H = `pkg-config --cflags lcms`" >> $CONF
  echo "LCMS_LIBS = `pkg-config --libs lcms`" >> $CONF
else
  if [ 0 = 1 ]; then
  echo "no or too old LCMS found,\n  need at least version 1.14, download: www.littlecms.com"
  fi
fi

echo "" >> $CONF
echo "VERSION_A = $VERSION_A" >> $CONF
echo "VERSION_B = $VERSION_B" >> $CONF
echo "VERSION_C = $VERSION_C" >> $CONF
echo "VERSION   = $VERSION" >> $CONF
echo "VERSION_L = $VERSION_L" >> $CONF
echo "src_dir = `pwd`/oyranos_$VERSION_L" >> $CONF

echo "#ifndef OYRANOS_VERSION_H" > oyranos_version.h
echo "#define OYRANOS_VERSION_H" >> oyranos_version.h
echo "" >> oyranos_version.h
echo "#define OYRANOS_VERSION_A $VERSION_A" >> oyranos_version.h
echo "#define OYRANOS_VERSION_B $VERSION_B" >> oyranos_version.h
echo "#define OYRANOS_VERSION_C $VERSION_C" >> oyranos_version.h
echo "#define OYRANOS_VERSION $VERSION" >> oyranos_version.h
echo "#define OYRANOS_DATE _(\"$TIME\")" >> oyranos_version.h
echo "" >> oyranos_version.h
echo "#endif /*OYRANOS_VERSION_H*/" >> oyranos_version.h

echo ""
echo ""


if [ `cat error.txt | wc -l` -lt 1 ]; then
  rm error.txt
fi
