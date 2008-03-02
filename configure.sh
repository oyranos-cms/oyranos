#!/bin/sh

ERROR=0
STRIPOPT='s/-O.// ; s/-isysroot [[:graph:]]*//'

if [ -n "$PKG_CONFIG_PATH" ]; then
  PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$libdir/pkgconfig
else
  PKG_CONFIG_PATH=$libdir/pkgconfig
fi
export PKG_CONFIG_PATH

# let it affect all other tests
if [ -n "$LIBS" ] && [ $LIBS -gt 0 ]; then
  if [ -n "$LIBS_TEST" ]; then
    for l in $LIBS_TEST; do
      rm -f tests/libtest$EXEC_END
      $CXX $CFLAGS -I$includedir tests/lib_test.cxx $LDFLAGS -L$libdir -l$l -o tests/libtest 2>/dev/null
      if [ -f tests/libtest ]; then
          echo "$l=-l$l" >> "config.sh"
          LDFLAGS="$LDFLAGS -l$l"
          test -n "$ECHO" && $ECHO "lib$l is available"
          if [ -n "$MAKEFILE_DIR" ]; then
            for i in $MAKEFILE_DIR; do
              test -f "$i/makefile".in && echo "$l = -l$l" >> "$i/makefile"
            done
          fi
          rm tests/libtest$EXEC_END
      fi
    done
  fi
fi

if [ -n "$ELEKTRA" ] && [ "$ELEKTRA" -gt "0" ]; then
  if [ -z "$elektra_min" ]; then
    elektra_min="0.6"
  fi
  if [ -z "$elektra_max" ]; then
    elektra_max="0.6.100"
  fi
  if [ "$internalelektra" != "no" ]; then
   if [ `ls $ELEKTRA_VERSION | grep elektra | wc -l` -gt 0 ]; then
    echo "local copy of elektra   detected"
        echo "#define HAVE_ELEKTRA 1" >> $CONF_H
        echo "ELEKTRA = 1" >> $CONF
        echo "ELEKTRA_VERSION = $ELEKTRA_VERSION" >> $CONF
        echo "ELEKTRA_H = -I\$(ELEKTRA_VERSION)/src/include" >> $CONF
        echo "ELEKTRA_LIBS = \$(ELEKTRA_VERSION)/src/libelektra/libelektra.a" >> $CONF
        ELEKTRA_FOUND=1
   fi
  fi
  if [ -z "$ELEKTRA_FOUND" ]; then
    elektra_mod=`pkg-config --modversion elektra`
  fi
  if [ $? = 0 ] && [ -z "$ELEKTRA_FOUND" ]; then
    pkg-config  --atleast-version=$elektra_min elektra 2>>error.txt
    if [ $? = 0 ]; then
      pkg-config --max-version=$elektra_max elektra 2>>error.txt
      if [ $? = 0 ]; then
        test -n "$ECHO" && $ECHO "elektra `pkg-config --modversion elektra`           detected"
        echo "#define HAVE_ELEKTRA 1" >> $CONF_H
        echo "ELEKTRA = 1" >> $CONF
        echo "ELEKTRA_H = `pkg-config --cflags elektra`" >> $CONF
        if [ $elektra_mod = "0.6.4" ]; then
          echo "ELEKTRA_LIBS = `pkg-config --libs elektra` -lxml2 -ldb" >> $CONF
        else
          echo "ELEKTRA_LIBS = `pkg-config --libs elektra`" >> $CONF
        fi
        echo "ELEKTRA_SW = `pkg-config --cflags-only-I  elektra | sed 's/\-I// ; s%/include%/etc/kdb/%'`" >> $CONF
        ELEKTRA_FOUND=1
      else
        if [ $ELEKTRA -eq 1 ]; then
          ERROR=1
          test -n "$ECHO" && $ECHO "!!! Elektra: !!!"
        else
          test -n "$ECHO" && $ECHO "    Elektra:"
        fi
        test -n "$ECHO" && $ECHO "  too new Elektra found,"
        test -n "$ECHO" && $ECHO "  need a version not greater than $elektra_max, download: elektra.sf.net"
      fi
    else
      if [ $ELEKTRA -eq 1 ]; then
        test -n "$ECHO" && $ECHO "!!! ERROR Elektra: !!!"
        ERROR=1
      else
        test -n "$ECHO" && $ECHO "    Warning Elektra:"
      fi
      test -n "$ECHO" && $ECHO "  no or too old elektra found,"
      test -n "$ECHO" && $ECHO "  need at least version $elektra_min, download: elektra.sf.net"
    fi
  fi
  if [ -z "$ELEKTRA_FOUND" ]; then
      test -n "$ECHO" && $ECHO $elektra_mod
      if [ $ELEKTRA -eq 1 ]; then
        ERROR=1
      fi
  fi
fi

if [ -n "$ARGYLL" ] && [ "$ARGYLL" -gt "0" ]; then
  if [ -z "$argyll_min" ]; then
    argyll_min="0.60"
  fi
  if [ -z "$argyll_max" ]; then
    argyll_max="0.61"
  fi
  if [ "$internalargyll" != "no" ]; then
   if [ `ls $ARGYLL_VERSION | grep argyll | wc -l` -gt 0 ]; then
    echo "local copy of argyll    detected"
        echo "#define HAVE_ARGYLL 1" >> $CONF_H
        echo "ARGYLL = 1" >> $CONF
        echo "ARGYLL_VERSION = $ARGYLL_VERSION" >> $CONF
        echo "ARGYLL_H = -DUSE_ARGYLL -I\$(ARGYLL_VERSION) -I\$(ARGYLL_VERSION)/icc -I\$(ARGYLL_VERSION)/gamut -I\$(ARGYLL_VERSION)/numlib -I\$(ARGYLL_VERSION)/xicc -I\$(ARGYLL_VERSION)/cgats -I\$(ARGYLL_VERSION)/rspl -I\$(ARGYLL_VERSION)/spectro" >> $CONF
        echo "ARGYLL_LIBS = \$(ARGYLL_VERSION)/icc/.libs/libargyllgamut.a \$(ARGYLL_VERSION)/icc/.libs/libargyllxicc.a \$(ARGYLL_VERSION)/icc/.libs/libcgats.a \$(ARGYLL_VERSION)/icc/.libs/libicc.a \$(ARGYLL_VERSION)/icc/.libs/libargyllrsp.al \$(ARGYLL_VERSION)/icc/.libs/libargyllnum.a" >> $CONF
        ARGYLL_FOUND=1
   fi
  fi
  if [ -z "$ARGYLL_FOUND" ]; then
    argyll_mod=`pkg-config --modversion argyll`
  fi
  if [ $? = 0 ] && [ -z "$ARGYLL_FOUND" ]; then
    pkg-config  --atleast-version=$argyll_min argyll 2>>error.txt
    if [ $? = 0 ]; then
      pkg-config --max-version=$argyll_max argyll 2>>error.txt
      if [ $? = 0 ]; then
        test -n "$ECHO" && $ECHO "argyll `pkg-config --modversion argyll`           detected"
        echo "#define HAVE_ARGYLL 1" >> $CONF_H
        echo "ARGYLL = 1" >> $CONF
        echo "ARGYLL_H = `pkg-config --cflags argyll`" >> $CONF
        if [ $argyll_mod = "0.6.4" ]; then
          echo "ARGYLL_LIBS = `pkg-config --libs argyll` -lxml2 -ldb" >> $CONF
        else
          echo "ARGYLL_LIBS = `pkg-config --libs argyll`" >> $CONF
        fi
        ARGYLL_FOUND=1
      else
        if [ $ARGYLL -eq 1 ]; then
#          ERROR=1
#          test -n "$ECHO" && $ECHO "!!! Argyll: !!!"
#        else
          test -n "$ECHO" && $ECHO "    Argyll:"
        fi
        test -n "$ECHO" && $ECHO "  too new Argyll found,"
        test -n "$ECHO" && $ECHO "  need a version not greater than $argyll_max, download: www.argyllcms.com"
      fi
    else
      if [ $ARGYLL -eq 1 ]; then
#        test -n "$ECHO" && $ECHO "!!! ERROR Argyll: !!!"
#        ERROR=1
#      else
        test -n "$ECHO" && $ECHO "    Warning Argyll:"
      fi
      test -n "$ECHO" && $ECHO "  no or too old argyll found,"
      test -n "$ECHO" && $ECHO "  need at least version $argyll_min, download: www.argyllcms.com"
    fi
  fi
  if [ -z "$ARGYLL_FOUND" ]; then
      test -n "$ECHO" && $ECHO $argyll_mod
#      if [ $ARGYLL -eq 1 ]; then
#        ERROR=1
#      fi
  fi
fi

if [ -n "$OYRANOS" ] && [ "$OYRANOS" != "0" ]; then
  OY_=`oyranos-config 2>>error.txt`
  if [ $? = 0 ]; then
    test -n "$ECHO" && $ECHO "Oyranos `oyranos-config --version`           detected"
    echo "#define HAVE_OY 1" >> $CONF_H
    echo "OY = 1" >> $CONF
    echo "OYRANOS_H = `oyranos-config --cflags`" >> $CONF
    if [ -f /usr/X11R6/include/X11/extensions/xf86vmode.h ]; then
      echo "OYRANOS_LIBS = `oyranos-config --ld_x_flags`" >> $CONF
    else
      echo "OYRANOS_LIBS = `oyranos-config --ld_x_flags`" >> $CONF
    fi
  else
    test -n "$ECHO" && $ECHO "no Oyranos found"
  fi
fi

if [ -n "$LCMS" ] && [ $LCMS -gt 0 ]; then
  pkg-config  --atleast-version=1.14 lcms
  if [ $? = 0 ]; then
    HAVE_LCMS=1
    echo "#define HAVE_LCMS 1" >> $CONF_H
    echo "LCMS = 1" >> $CONF
    echo "LCMS_H = `pkg-config --cflags lcms | sed \"$STRIPOPT\"`" >> $CONF
    echo "LCMS_LIBS = `pkg-config --libs lcms | sed \"$STRIPOPT\"`" >> $CONF
  else
    l=lcms 
    rm -f tests/libtest$EXEC_END
    $CXX $CFLAGS -I$includedir tests/lib_test.cxx $LDFLAGS -L/usr/X11R6/lib$BARCH -L/usr/lib$BARCH -L$libdir -l$l -o tests/libtest 2>/dev/null
    if [ -f tests/libtest ]; then
      HAVE_LCMS=1
      echo "#define HAVE_LCMS 1" >> $CONF_H
      echo "LCMS = 1" >> $CONF
      echo "LCMS_H =" >> $CONF
      echo "LCMS_LIBS = -llcms" >> $CONF
      rm tests/libtest$EXEC_END
    fi
  fi
  if [ -n $HAVE_LCMS ]; then
    test -n "$ECHO" && $ECHO "littleCMS               detected"
  else
    if [ $LCMS -eq 1 ]; then
      test -n "$ECHO" && $ECHO "!!! ERROR: no or too old LCMS found, !!!"
      ERROR=1
    else
      test -n "$ECHO" && $ECHO "    Warning: no or too old LCMS found,"
    fi
    test -n "$ECHO" && $ECHO "  need at least version 1.14, download: www.littlecms.com"
  fi
fi

if [ -n "$X11" ] && [ $X11 -gt 0 ]; then
  if [ -f /usr/X11R6/include/X11/Xlib.h ] ||
     [ -f /usr/include/X11/Xlib.h ] ||
     [ -f $includedir/X11/Xlib.h ]; then
    test -n "$ECHO" && $ECHO "X11                     detected"
    echo "#define HAVE_X 1" >> $CONF_H
    if [ -n "$MAKEFILE_DIR" ]; then
      for i in $MAKEFILE_DIR; do
        test -f "$i/makefile".in && echo "X11 = X11" >> "$i/makefile"
        test -f "$i/makefile".in && echo "X_H = -I/usr/X11R6/include -I/usr/include" >> "$i/makefile"
      done
    fi
  elif [ $OSUNAME = "Linux" ]; then
    test -n "$ECHO" && $ECHO "X11 header not found in /usr/X11R6/include/X11/Xlib.h or"
    test -n "$ECHO" && $ECHO "/usr/include/X11/Xlib.h"
    X11=0
  fi
fi
if [ -n "$X11" ] && [ $X11 -gt 0 ]; then
  if [ -n "$XF86VMODE" ] && [ $XF86VMODE -gt 0 ]; then
    if [ -f /usr/X11R6/include/X11/extensions/xf86vmode.h ] ||
       [ -f /usr/include/X11/extensions/xf86vmode.h ] ||
       [ -f $includedir/X11/extensions/xf86vmode.h ]; then
      test -n "$ECHO" && $ECHO "X VidMode extension     detected"
      echo "#define HAVE_XF86VMODE 1" >> $CONF_H
      if [ -n "$MAKEFILE_DIR" ]; then
        for i in $MAKEFILE_DIR; do
          test -f "$i/makefile".in && echo "XF86VMODE = 1" >> "$i/makefile"
          test -f "$i/makefile".in && echo "XF86VMODE_LIB = -lXxf86vm" >> "$i/makefile"
        done
      fi
    elif [ $OSUNAME = "Linux" ]; then
      test -n "$ECHO" && $ECHO "X VidMode extension not found in /usr/X11R6/include/X11/extensions/xf86vmode.h or"
      test -n "$ECHO" && $ECHO "/usr/include/X11/extensions/xf86vmode.h"
    fi
  fi

  if [ -n "$XINERAMA" ] && [ $XINERAMA -gt 0 ]; then
    if [ -f /usr/X11R6/include/X11/extensions/Xinerama.h ] ||
       [ -f /usr/include/X11/extensions/Xinerama.h ] ||
       [ -f $includedir/X11/extensions/Xinerama.h ]; then
      test -n "$ECHO" && $ECHO "X Xinerama              detected"
      echo "#define HAVE_XIN 1" >> $CONF_H
      if [ -n "$MAKEFILE_DIR" ]; then
        for i in $MAKEFILE_DIR; do
          test -f "$i/makefile".in && echo "XIN = 1" >> "$i/makefile"
          test -f "$i/makefile".in && echo "XINERAMA_LIB = -lXinerama" >> "$i/makefile"
        done
      fi
    else
      if [ $OSUNAME = "Linux" ]; then
        test -n "$ECHO" && $ECHO "X Xinerma not found in /usr/X11R6/include/X11/extensions/Xinerama.h or"
        test -n "$ECHO" && $ECHO "/usr/include/X11/extensions/Xinerama.h"
      fi
    fi
  fi
  echo "X_CPP = \$(X_CPPFILES)" >> $CONF
  if [ -n "$MAKEFILE_DIR" ]; then
    for i in $MAKEFILE_DIR; do
      test -f "$i/makefile".in && echo "X11_LIB_PATH = -L/usr/X11R6/lib\$(BARCH) -L/usr/lib\$(BARCH) -L\$(libdir)" >> "$i/makefile"
    done
  fi

  if [ -n "$X_ADD" ]; then
    for l in $X_ADD; do
      rm -f tests/libtest$EXEC_END
      $CXX $CFLAGS -I$includedir tests/lib_test.cxx $LDFLAGS -L/usr/X11R6/lib$BARCH -L/usr/lib$BARCH -L$libdir -l$l -o tests/libtest 2>/dev/null
      if [ -f tests/libtest ]; then
          test -n "$ECHO" && $ECHO "lib$l is available"
          if [ -z "$X_ADD_LIBS" ]; then
            X_ADD_LIBS="-l$l"
          else
            X_ADD_LIBS="$X_ADD_LIBS -l$l"
          fi
          echo "#define HAVE_$l 1"  >> $CONF_H
          # Test if we need to link explicitely, possibly symbols are included
          if [ $OSUNAME = "Darwin" ]; then
            if [ -n "`otool -L tests/libtest | grep $l`" ]; then
              echo "$l=-l$l" >> "config.sh"
            fi
          else
            if [ -n "`ldd tests/libtest | grep $l`" ]; then
              echo "$l=-l$l" >> "config.sh"
            fi
          fi
          rm tests/libtest$EXEC_END
      else
        if [ $X11 -eq 1 ]; then
          test -n "$ECHO" && $ECHO "!!! ERROR lib$l is missed"
          ERROR=1
        else
          test -n "$ECHO" && $ECHO "  Warning lib$l is missed"
        fi
      fi
    done
  fi
  if [ -n "$X_ADD_2" ]; then
    for l in $X_ADD_2; do
      rm -f tests/libtest$EXEC_END
      $CXX $CFLAGS -I$includedir tests/lib_test.cxx $LDFLAGS -L/usr/X11R6/lib$BARCH -L/usr/lib$BARCH -L$libdir -l$l -o tests/libtest 2>/dev/null
      if [ -f tests/libtest ]; then
          test -n "$ECHO" && $ECHO "lib$l is available"
          if [ -z "$X_ADD_LIBS" ]; then
            X_ADD_LIBS="-l$l"
          else
            X_ADD_LIBS="$X_ADD_LIBS -l$l"
          fi
          echo "#define HAVE_$l 1"  >> $CONF_H
          echo "$l=-l$l" >> "config.sh"
          rm tests/libtest$EXEC_END
      else
        test -n "$ECHO" && $ECHO "lib$l not found"
      fi
    done
  fi
  if [ -n "$MAKEFILE_DIR" ]; then
    for i in $MAKEFILE_DIR; do
      test -f "$i/makefile".in && echo "X11_LIBS=\$(X11_LIB_PATH) -lX11 \$(XF86VMODE_LIB) $X_ADD_LIBS \$(XINERAMA_LIB)" >> "$i/makefile"
    done
  fi
fi

if [ -n "$FTGL" ] && [ $FTGL -gt 0 ]; then
  pkg-config  --atleast-version=1.0 ftgl
  if [ $? = 0 ]; then
    test -n "$ECHO" && $ECHO "FTGL      `pkg-config --modversion ftgl`         detected"
    echo "#define HAVE_FTGL 1" >> $CONF_H
    echo "FTGL = 1" >> $CONF
    echo "FTGL_H = `pkg-config --cflags ftgl | sed \"$STRIPOPT\"`" >> $CONF
    echo "FTGL_LIBS = `pkg-config --libs ftgl | sed \"$STRIPOPT\"`" >> $CONF
  else
    l=ftgl 
    rm -f tests/libtest$EXEC_END
    $CXX $CFLAGS -I$includedir tests/lib_test.cxx $LDFLAGS -L/usr/X11R6/lib$BARCH -L/usr/lib$BARCH -L$libdir -l$l -o tests/libtest 2>/dev/null
    if [ -f tests/libtest ]; then
      test -n "$ECHO" && $ECHO "FTGL                    detected"
      echo "#define HAVE_FTGL 1" >> $CONF_H
      echo "FTGL = 1" >> $CONF
      echo "FTGL_H =" >> $CONF
      echo "FTGL_LIBS = -lftgl -lfreetype" >> $CONF
      rm tests/libtest$EXEC_END
    else
      test -n "$ECHO" && $ECHO "  no or too old FTGL found, need FTGL to render text in OpenGL"
    fi
  fi
fi

if [ -z "$fltkconfig" ]; then
  # add /usr/X11R6/bin to path for Fedora
  fltkconfig=fltk-config
  PATH=$PATH:/usr/X11R6/bin; export PATH
  echo add fltk-config
fi
if [ -n "$FLTK" ] && [ $FLTK -gt 0 ]; then
  FLTK_="`$fltkconfig --cxxflags 2>>error.txt | sed \"$STRIPOPT\"`"
  if [ $? = 0 ] && [ -n "$FLTK_" ]; then
    # check for utf-8 capability
    if [ $fltkconfig != `echo $fltkconfig | sed "s%fltk2-config%% ; s%utf8%%"` ]; then
      echo "#define HAVE_FLTK_UTF8 1" >> $CONF_H
      echo "HAVE_FLTK_UTF8 = -DHAVE_FLTK_UTF8" >> $CONF_I18N
      fltk_utf8="utf-8 `$fltkconfig --version`"
    else
      fltk_utf8="`$fltkconfig --version`      "
    fi
    test -n "$ECHO" && $ECHO "FLTK $fltk_utf8        detected"
    if [ "0" -ne "`$fltkconfig --compile tests/fltk_test.cxx 2>&1 | grep lock | wc -l`" ]; then
      test -n "$ECHO" && $ECHO "!!! ERROR: FLTK has no threads support !!!"
      test -n "$ECHO" && $ECHO "           Configure FLTK with the --enable-threads option and recompile."
      ERROR=1
    else
      rm fltk_test$EXEC_END
    fi
    echo "#define HAVE_FLTK 1" >> $CONF_H
    echo "FLTK = 1" >> $CONF
    echo "FLTK_H = `$fltkconfig --cxxflags | sed \"$STRIPOPT\"`" >> $CONF
    echo "FLTK_LIBS = `$fltkconfig --use-images --use-gl --ldflags | sed \"$STRIPOPT\"`" >> $CONF
    echo "fltkconfig = $fltkconfig" >> $CONF
    echo "FLTK = 1" >> $CONF_I18N
    echo "FLTK_H = `$fltkconfig --cxxflags | sed \"$STRIPOPT\"`" >> $CONF_I18N
    echo "FLTK_LIBS = `$fltkconfig --use-images --use-gl --ldflags | sed \"$STRIPOPT\"`" >> $CONF_I18N
    echo "fltkconfig = $fltkconfig" >> $CONF_I18N

  else
    if [ $FLTK -eq 1 ]; then
      ERROR=1
      test -n "$ECHO" && $ECHO "!!! ERROR !!!"
    else
      test -n "$ECHO" && $ECHO "    Warning"
    fi
    test -n "$ECHO" && $ECHO "           FLTK ($fltkconfig) is not found; download: www.fltk.org"
  fi
fi

if [ -n "$FLU" ] && [ $FLU -gt 0 ]; then
  FLU_=`flu-config --cxxflags 2>>error.txt`
  if [ "`$fltkconfig --version`" = "1.1.7" ]; then
    echo -e "\c"
    #"
    test -n "$ECHO" && $ECHO "FLTK version 1.1.7 is not supported by FLU"
    if [ "$FLU" = 1 ]; then
      ERROR=1
    fi
  else
    if [ -n "$FLU_" ] && [ -n "$FLTK_" ]; then
      test -n "$ECHO" && $ECHO "FLU                     detected"
      echo "#define HAVE_FLU 1" >> $CONF_H
      echo "FLU = 1" >> $CONF
      echo "FLU_H = `flu-config --cxxflags | sed \"$STRIPOPT\"`" >> $CONF
      echo "FLU_LIBS = `flu-config --ldflags --use-gl | sed \"$STRIPOPT\"`" >> $CONF
    else
      if [ "$FLU" -gt 1 ]; then
        test -n "$ECHO" && $ECHO "   no FLU found, will not use it"
      else
        test -n "$ECHO" && $ECHO "ERROR:   FLU is not found; download:"
        test -n "$ECHO" && $ECHO "         http://www.osc.edu/~jbryan/FLU/http://www.osc.edu/~jbryan/FLU/"
        ERROR=1
      fi
    fi
  fi
fi

if [ -n "$DOXYGEN" ] && [ $DOXYGEN -gt 0 ]; then
  if [ "`doxygen --help`" != "" ]; then
    test -n "$ECHO" && $ECHO "Doxygen `doxygen --version`           detected"
  else
    test -n "$ECHO" && $ECHO "Doxygen                 not detected"
  fi
fi

if [ -n "$LIBPNG" ] && [ $LIBPNG -gt 0 ]; then
  LIBPNG=libpng
  pkg-config  --atleast-version=1.0 $LIBPNG 2>>error.txt
  if [ $? != 0 ]; then
    LIBPNG=libpng12
    pkg-config  --atleast-version=1.0 $LIBPNG 2>>error.txt
  fi
  if [ $? = 0 ]; then
    test -n "$ECHO" && $ECHO "PNG `pkg-config --modversion $LIBPNG`               detected"
    echo "#define HAVE_PNG 1" >> $CONF_H
    echo "PNG = 1" >> $CONF
    echo "PNG_H = `pkg-config --cflags $LIBPNG | sed \"$STRIPOPT\"`" >> $CONF
    echo "PNG_LIBS = `pkg-config --libs $LIBPNG | sed \"$STRIPOPT\"`" >> $CONF
  else
    test -n "$ECHO" && $ECHO "no or too old libpng found,"
    test -n "$ECHO" && $ECHO "  need at least version 1.0, download: www.libpng.org"
  fi
fi

if [ -n "$LIBTIFF" ] && [ $LIBTIFF -gt 0 ]; then
  rm -f tests/libtest$EXEC_END
  $CXX $CFLAGS -I$includedir tests/tiff_test.cxx $LDFLAGS -L$libdir -ltiff -ljpeg -o tests/libtest 2>error.txt
    if [ -f tests/libtest ]; then
      test -n "$ECHO" && $ECHO "`tests/libtest`
                        detected"
      echo "#define HAVE_TIFF 1" >> $CONF_H
      echo "TIFF = 1" >> $CONF
      rm tests/libtest$EXEC_END
    else
      test -n "$ECHO" && $ECHO "no or too old libtiff found,"
    fi
fi

if [ -n "$GETTEXT" ] && [ $GETTEXT -gt 0 ]; then
  rm -f tests/libtest$EXEC_END
    $CXX $CFLAGS -I$includedir tests/gettext_test.cxx $LDFLAGS -L$libdir -o tests/libtest 2>/dev/null
    if [ ! -f tests/libtest ]; then
      $CXX $CFLAGS -I$includedir tests/gettext_test.cxx $LDFLAGS -L$libdir -lintl -o tests/libtest 2>error.txt
    fi
    if [ -f tests/libtest ]; then
      test -n "$ECHO" && $ECHO "Gettext                 detected"
      echo "#define USE_GETTEXT 1" >> $CONF_H
      echo "GETTEXT = -DUSE_GETTEXT" >> $CONF
      echo "GETTEXT = -DUSE_GETTEXT" >> $CONF_I18N
      rm tests/libtest$EXEC_END
    else
      test -n "$ECHO" && $ECHO "no or too old Gettext found,"
    fi
fi

if [ -n "$PO" ] && [ $PO -gt 0 ]; then
  pos_dir="`ls po/*.po 2> /dev/null`"
  LING="`echo $pos_dir`"
  LINGUAS="`echo $pos_dir | sed 's/\.po//g ; s/po\///g'`"
  echo "LINGUAS = $LINGUAS" >> $CONF
  echo "translations available: $LINGUAS"
  echo "LING = $LING" >> $CONF
  #echo "#define USE_GETTEXT 1" >> $CONF_H
fi

if [ -n "$PREPARE_MAKEFILES" ] && [ $PREPARE_MAKEFILES -gt 0 ]; then
  if [ -n "$MAKEFILE_DIR" ]; then
    for i in $MAKEFILE_DIR; do
      echo preparing Makefile in "$i/"
      if [ $OSUNAME = "BSD" ]; then
        test -f "$i/makefile".in && cat  "$i/makefile".in | sed 's/#if/.if/g ; s/#end/.end/g ; s/#else/.else/g '  >> "$i/makefile"
      else
        test -f "$i/makefile".in && cat  "$i/makefile".in | sed 's/#if/if/g ; s/#elif/elif/g ; s/#else/else/g ; s/#end/end/g '  >> "$i/makefile"
      fi
      mv "$i/makefile" "$i/Makefile"
    done
  fi
fi


if [ -n "$DEBUG" ] && [ $DEBUG -gt 0 ]; then
  if [ -n "$MAKEFILE_DIR" ]; then
    for i in $MAKEFILE_DIR; do
      if [ "$debug" -eq "1" ]; then
        if [ $OSUNAME == "Darwin" ] || [ $OSUNAME == "Windows" ]; then
          DEBUG_="-Wall -g -DDEBUG"
        else
          DEBUG_="-Wall -g -DDEBUG --pedantic"
        fi
        test -f "$i/makefile".in && echo "DEBUG = $DEBUG_"  >> "$i/makefile"
        test -f "$i/makefile".in && echo "DEBUG_SWITCH = -v"  >> "$i/makefile"
      else
        test -f "$i/makefile".in && echo ".SILENT:"  >> "$i/makefile"
      fi
    done
  fi
fi

# we cannot reimport, just return
if [ "$ERROR" -ne "0" ]; then 
  echo "error" > error.tmp
fi
exit $ERROR

