ERROR=0

if [ -n "$PKG_CONFIG_PATH" ]; then
  PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$libdir/pkgconfig
else
  PKG_CONFIG_PATH=$libdir/pkgconfig
fi
export PKG_CONFIG_PATH

if [ -n "$ELEKTRA" ] && [ $ELEKTRA -gt 0 ]; then
  if [ -z "$elektra_min" ]; then
    elektra_min="0.6"
  fi
  if [ -z "$elektra_max" ]; then
    elektra_max="0.6.100"
  fi
  `pkg-config  --atleast-version=$elektra_min elektra 2>>error.txt`
  if [ $? = 0 ]; then
   `pkg-config --max-version=$elektra_max elektra 2>>error.txt`
    if [ $? = 0 ]; then
      echo "elektra `pkg-config --modversion elektra`           detected"
      echo "#define HAVE_ELEKTRA 1" >> $CONF_H
      echo "ELEKTRA = 1" >> $CONF
      echo "ELEKTRA_H = `pkg-config --cflags elektra`" >> $CONF
      echo "ELEKTRA_LIBS = `pkg-config --libs elektra`" >> $CONF
      ELEKTRA_FOUND=1
    else
      echo "Elektra:"
      echo "  too new Elektra found,"
      echo "  need a version not greater than $elektra_max, download: elektra.sf.net"
      ERROR=1
    fi
  else
    echo "Elektra:"
    echo "  no or too old elektra found,"
    echo "  need at least version $elektra_min, download: elektra.sf.net"
    ERROR=1
  fi
fi

if [ -n "$OYRANOS" ] && [ $OYRANOS -gt 0 ]; then
  OY_=`oyranos-config 2>>error.txt`
  if [ $? = 0 ] && [ -n $OY_ ]; then
    echo "Oyranos `oyranos-config --version`           detected"
    echo "#define HAVE_OY 1" >> $CONF_H
    echo "OY = 1" >> $CONF
    echo "OYRANOS_H = `oyranos-config --cflags`" >> $CONF
    if [ -f /usr/X11R6/include/X11/extensions/xf86vmode.h ]; then
      echo "OYRANOS_LIBS = `oyranos-config --ld_x_flags`" >> $CONF
    else
      echo "OYRANOS_LIBS = `oyranos-config --ld_x_flags`" >> $CONF
    fi
  else
    echo "no Oyranos found"
  fi
fi

if [ -n "$LCMS" ] && [ $LCMS -gt 0 ]; then
  `pkg-config  --atleast-version=1.14 lcms`
  if [ $? = 0 ]; then
    echo "littleCMS `pkg-config --modversion lcms`          detected"
    echo "#define HAVE_LCMS 1" >> $CONF_H
    echo "LCMS = 1" >> $CONF
    echo "LCMS_H = `pkg-config --cflags lcms`" >> $CONF
    echo "LCMS_LIBS = `pkg-config --libs lcms`" >> $CONF
  else
    echo "no or too old LCMS found,\n  need at least version 1.14, download: www.littlecms.com"
    ERROR=1
  fi
fi

if [ -n "$X11" ] && [ $X11 -gt 0 ]; then
  if [ -f /usr/X11R6/include/X11/Xlib.h ] ||
     [ -f /usr/include/X11/Xlib.h ] ||
     [ -f $includedir/X11/Xlib.h ]; then
    echo "X11                     detected"
    echo "#define HAVE_X 1" >> $CONF_H
    echo "X11 = 1" >> $CONF
    echo "X_H = -I/usr/X11R6/include -I/usr/include" >> $CONF
  elif [ $OSUNAME = "Linux" ]; then
    echo "X11 header not found in /usr/X11R6/include/X11/Xlib.h or"
    echo "/usr/include/X11/Xlib.h"
    X11=0
  fi
fi
if [ "$X11" = 1 ] && [ $X11 -gt 0 ]; then
  if [ -n "$XF86VMODE" ] && [ $XF86VMODE -gt 0 ]; then
    if [ -f /usr/X11R6/include/X11/extensions/xf86vmode.h ] ||
       [ -f /usr/include/X11/extensions/xf86vmode.h ] ||
       [ -f $includedir/X11/extensions/xf86vmode.h ]; then
      echo "X VidMode extension     detected"
      echo "#define HAVE_XF86VMODE 1" >> $CONF_H
      echo "XF86VMODE = 1" >> $CONF
      echo "XF86VMODE_LIB = -lXxf86vm" >> $CONF
    elif [ $OSUNAME = "Linux" ]; then
      echo "X VidMode extension not found in /usr/X11R6/include/X11/extensions/xf86vmode.h or"
      echo "/usr/include/X11/extensions/xf86vmode.h"
    fi
  fi

  if [ -n "$XINERAMA" ] && [ $XINERAMA -gt 0 ]; then
    if [ -f /usr/X11R6/include/X11/extensions/Xinerama.h ] ||
       [ -f /usr/include/X11/extensions/Xinerama.h ] ||
       [ -f $includedir/X11/extensions/Xinerama.h ]; then
      echo "X Xinerama              detected"
      echo "#define HAVE_XIN 1" >> $CONF_H
      echo "XIN = 1" >> $CONF
      echo "XINERAMA_LIB = -lXinerama" >> $CONF
    else
      if [ $OSUNAME = "Linux" ]; then
        echo "X Xinerma not found in /usr/X11R6/include/X11/extensions/Xinerama.h or"
        echo "/usr/include/X11/extensions/Xinerama.h"
      fi
    fi
  fi
  echo "X_CPP = \$(X_CPPFILES)" >> $CONF
  echo "X11_LIB_PATH = -L/usr/X11R6/lib\$(BARCH) -L/usr/lib\$(BARCH) -L\$libdir" >> $CONF
  echo "X11_LIBS=\$(X11_LIB_PATH) -lX11 \$(XF86VMODE_LIB) -lXpm -lXext \$(XINERAMA_LIB)" >> $CONF
fi

if [ -n "$FTGL" ] && [ $FTGL -gt 0 ]; then
  `pkg-config  --atleast-version=1.0 ftgl`
  if [ $? = 0 ]; then
    echo "FTGL      `pkg-config --modversion ftgl`         detected"
    echo "#define HAVE_FTGL 1" >> $CONF_H
    echo "FTGL = 1" >> $CONF
    echo "FTGL_H = `pkg-config --cflags ftgl`" >> $CONF
    echo "FTGL_LIBS = `pkg-config --libs ftgl`" >> $CONF
  else
    echo "  no or too old FTGL found, need FTGL to render text in OpenGL"
  fi
fi

if [ -n "$FLTK" ] && [ $FLTK -gt 0 ]; then
  FLTK_=`fltk-config --cxxflags 2>>error.txt`
  if [ $? = 0 ] && [ -n "$FLTK_" ]; then
    echo "FLTK `fltk-config --version`              detected"
    echo "#define HAVE_FLTK 1" >> $CONF_H
    echo "FLTK = 1" >> $CONF
    echo "FLTK_H = `fltk-config --cxxflags | sed 's/-O[0-9]//'`" >> $CONF
    echo "FLTK_LIBS = `fltk-config --use-images --use-gl --ldflags`" >> $CONF
  else
    echo "   FLTK is not found; download: www.fltk.org"
    ERROR=1
  fi
fi

if [ -n "$FLU" ] && [ $FLU -gt 0 ]; then
  FLU_=`flu-config --cxxflags 2>>error.txt`
  if [ `fltk-config --version` == "1.1.7" ]; then
    echo -e "\c"
    echo "FLTK version 1.1.7 is not supported by FLU"
    if [ "$FLU" = 1 ]; then
      ERROR=1
    fi
  else
    if [ -n "$FLU_" ] && [ -n "$FLTK_" ]; then
      echo "FLU                     detected"
      echo "#define HAVE_FLU 1" >> $CONF_H
      echo "FLU = 1" >> $CONF
      echo "FLU_H = `flu-config --cxxflags`" >> $CONF
      echo "FLU_LIBS = `flu-config --ldflags --use-gl`" >> $CONF
    else
      if [ "$FLU" -gt 1 ]; then
        echo "   no FLU found, will not use it"
      else
        echo "   FLU is not found; download:"
        echo "   http://www.osc.edu/~jbryan/FLU/http://www.osc.edu/~jbryan/FLU/"
        ERROR=1
      fi
    fi
  fi
fi

if [ -n "$DOXYGEN" ] && [ $DOXYGEN -gt 0 ]; then
  if [ "`which doxygen`" != "" ]; then
    echo "Doxygen `doxygen --version`           detected"
  else
    echo "Doxygen                 not detected"
  fi
fi

if [ -n "$LIBPNG" ] && [ $LIBPNG -gt 0 ]; then
  LIBPNG=libpng
  `pkg-config  --atleast-version=1.0 $LIBPNG 2>>error.txt`
  if [ $? != 0 ]; then
    LIBPNG=libpng12
    `pkg-config  --atleast-version=1.0 $LIBPNG 2>>error.txt`
  fi
  if [ $? = 0 ]; then
    echo "PNG `pkg-config --modversion $LIBPNG`               detected"
    echo "#define HAVE_PNG 1" >> $CONF_H
    echo "PNG = 1" >> $CONF
    echo "PNG_H = `pkg-config --cflags $LIBPNG`" >> $CONF
    echo "PNG_LIBS = `pkg-config --libs $LIBPNG`" >> $CONF
  else
    echo "no or too old libpng found,\n  need at least version 1.0, download: www.libpng.org"
  fi
fi

if [ -n "$PO" ] && [ $PO -gt 0 ]; then
  pos_dir="`ls po/*.po 2> /dev/null`"
  LING="`echo $pos_dir`"
  LINGUAS="`echo $pos_dir | sed 's/\.po//g ; s/po\///g'`"
  echo "LINGUAS = $LINGUAS" >> $CONF
  echo "Languages detected:     $LINGUAS"
  echo "LING = $LING" >> $CONF
fi

if [ -n "$PREPARE_MAKEFILES" ] && [ $PREPARE_MAKEFILES -gt 0 ]; then
  if [ -n "$MAKEFILES" ]; then
    for i in $MAKEFILES; do
      echo preparing "$i"
      if [ $OSUNAME = "BSD" ]; then
        cat  "$i".in | sed 's/^\#if/.if/g ; s/^\#end/.end/g '  >> "$i"
      else
        cat  "$i".in | sed 's/^\#if/if/g ; s/^\#elif/elif/g ; s/^\#else/else/g ; s/^\ \ \#if/\ \ if/g ; s/^\#end/end/g '  >> "$i"
      fi
    done
  fi
fi

# we cannot reimport, just return
exit $ERROR

