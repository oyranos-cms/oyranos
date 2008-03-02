#!/bin/sh

set > testset.txt
ERROR=0
STRIPOPT='s/-O.// ; s/-isysroot [[:graph:]]*// ; s/-arch ppc// ; s/-arch i386//'

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
          echo "$l=-l$l" >> "$CONF_TEMP_SH"
          LDFLAGS="$LDFLAGS -l$l"
          echo_="lib$l is available"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
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
    elektra_min="0.7"
  fi
  if [ -z "$elektra_max" ]; then
    elektra_max="0.7.100"
  fi
  if [ "$internalelektra" != "no" ]; then
   if [ `ls $ELEKTRA_VERSION | grep elektra | wc -l` -gt 0 ]; then
     echo_="local copy of elektra   detected"; echo "$echo_" >> $CONF_LOG; echo "$echo_"
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
    pkg-config  --atleast-version=$elektra_min elektra 2>>$CONF_LOG
    if [ $? = 0 ]; then
      pkg-config --max-version=$elektra_max elektra 2>>$CONF_LOG
      if [ $? = 0 ]; then
        echo_="elektra `pkg-config --modversion elektra`           detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
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
          echo_="!!! Elektra: !!!"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        else
          echo_="    Elektra:"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        fi
        echo_="  too new Elektra found,"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        echo_="  need a version not greater than $elektra_max, download: elektra.sf.net"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      fi
    else
      if [ $ELEKTRA -eq 1 ]; then
        echo_="!!! ERROR Elektra: !!!"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        ERROR=1
      else
        echo_="    Warning Elektra:"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      fi
      echo_="  no or too old elektra found,"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo_="  need at least version $elektra_min, download: elektra.sf.net"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    fi
  fi
  if [ -z "$ELEKTRA_FOUND" ]; then
      echo_="$elektra_mod"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
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
   if [ `ls $ARGYLL_VERSION | grep Readme.txt | wc -l` -gt 0 ]; then
    echo_="local copy of argyll    detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        echo "#define HAVE_ARGYLL 1" >> $CONF_H
        echo "ARGYLL = 1" >> $CONF
        echo "ARGYLL_VERSION = $ARGYLL_VERSION" >> $CONF
        echo "ARGYLL_H = -DUSE_ARGYLL -I\$(ARGYLL_VERSION) -I\$(ARGYLL_VERSION)/icc -I\$(ARGYLL_VERSION)/gamut -I\$(ARGYLL_VERSION)/numlib -I\$(ARGYLL_VERSION)/xicc -I\$(ARGYLL_VERSION)/cgats -I\$(ARGYLL_VERSION)/rspl -I\$(ARGYLL_VERSION)/spectro" >> $CONF
        echo "ARGYLL_LIBS = \$(ARGYLL_VERSION)/gamut/.libs/libargyllgamut.a \$(ARGYLL_VERSION)/xicc/.libs/libargyllxicc.a \$(ARGYLL_VERSION)/cgats/.libs/libcgats.a \$(ARGYLL_VERSION)/icc/.libs/libicc.a \$(ARGYLL_VERSION)/rspl/.libs/libargyllrspl.a \$(ARGYLL_VERSION)/numlib/.libs/libargyllnum.a" >> $CONF
        ARGYLL_FOUND=1
   fi
  fi
  if [ -z "$ARGYLL_FOUND" ]; then
    argyll_mod=`pkg-config --modversion argyll`
  fi
  if [ $? = 0 ] && [ -z "$ARGYLL_FOUND" ]; then
    pkg-config  --atleast-version=$argyll_min argyll 2>>$CONF_LOG
    if [ $? = 0 ]; then
      pkg-config --max-version=$argyll_max argyll 2>>$CONF_LOG
      if [ $? = 0 ]; then
        echo_="argyll `pkg-config --modversion argyll`           detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
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
          echo_="    Argyll:"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        fi
        echo_="  too new Argyll found,"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        echo_="  need a version not greater than $argyll_max, download: www.argyllcms.com"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      fi
    else
      if [ $ARGYLL -eq 1 ]; then
#        test -n "$ECHO" && $ECHO "!!! ERROR Argyll: !!!"
#        ERROR=1
#      else
        echo_="    Warning Argyll:"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      fi
      echo_="  no or too old argyll found,"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo_="  need at least version $argyll_min, download: www.argyllcms.com"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    fi
  fi
  if [ -z "$ARGYLL_FOUND" ]; then
      echo_="$argyll_mod"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
#      if [ $ARGYLL -eq 1 ]; then
#        ERROR=1
#      fi
  fi
fi

if [ -n "$OYRANOS" ] && [ "$OYRANOS" != "0" ]; then
  OY_=`oyranos-config 2>>$CONF_LOG`
  if [ $? = 0 ]; then
    echo_="Oyranos `oyranos-config --version`           detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    echo "#define HAVE_OY 1" >> $CONF_H
    echo "OY = 1" >> $CONF
    echo "OYRANOS_H = `oyranos-config --cflags`" >> $CONF
    if [ -f /usr/X11R6/include/X11/extensions/xf86vmode.h ]; then
      echo "OYRANOS_LIBS = `oyranos-config --ld_x_flags`" >> $CONF
    else
      echo "OYRANOS_LIBS = `oyranos-config --ld_x_flags`" >> $CONF
    fi
  else
    echo_="no Oyranos found"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
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
    echo_="littleCMS               detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
  else
    if [ $LCMS -eq 1 ]; then
      echo_="!!! ERROR: no or too old LCMS found, !!!"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      ERROR=1
    else
      echo_="    Warning: no or too old LCMS found,"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    fi
    echo_="  need at least version 1.14, download: www.littlecms.com"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
  fi
fi

if [ -n "$X11" ] && [ $X11 -gt 0 ]; then
    found=""
    version=""
    pc_package=x11
    if [ -z "$found" ]; then
      pkg-config  --atleast-version=1.0 $pc_package
      if [ $? = 0 ]; then
        found=`pkg-config --cflags $pc_package`
        version=`pkg-config --modversion $pc_package`
      fi
    fi
    if [ -z "$found" ]; then
      if [ -f /usr/X11R6/include/X11/Xlib.h ]; then
        found="-I/usr/X11R6/include"
      elif [ -f /usr/include/X11/Xlib.h ]; then
        found="-I/usr/include"
      elif [ -f $includedir/X11/Xlib.h ]; then
        found="-I$includedir"
      fi
    fi
    if [ -z "$found" ] && [ $OSUNAME = "Linux" ]; then
      echo_="X11 header not found in /usr/X11R6/include/X11/Xlib.h or"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo_="  /usr/include/X11/Xlib.h or"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo_="  $pc_package.pc"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      X11=0
    fi
fi
if [ -n "$X11" ] && [ $X11 -gt 0 ]; then
  if [ -n "$XF86VMODE" ] && [ $XF86VMODE -gt 0 ]; then
    found=""
    version=""
    pc_package=xf86vidmodeproto
    if [ -z "$found" ]; then
      pkg-config  --atleast-version=1.0 $pc_package
      if [ $? = 0 ]; then
        found=`pkg-config --cflags $pc_package`
        version=`pkg-config --modversion $pc_package`
      fi
    fi
    if [ -z "$found" ]; then
      if [ -f /usr/X11R6/include/X11/extensions/xf86vmode.h ]; then
        found="-I/usr/X11R6/include"
      elif [ -f /usr/include/X11/extensions/xf86vmode.h ]; then
        found="-I/usr/include"
      elif [ -f $includedir/X11/extensions/xf86vmode.h ]; then
        found="-I$includedir"
      fi
    fi
    if [ -n "$found" ]; then
      if [ -n "$version" ]; then
        echo_="X VidMode $version         detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      else
        echo_="X VidMode               detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      fi
      echo "#define HAVE_XF86VMODE 1" >> $CONF_H
      if [ -n "$MAKEFILE_DIR" ]; then
        for i in $MAKEFILE_DIR; do
          test -f "$i/makefile".in && echo "XF86VMODE = 1" >> "$i/makefile"
          test -f "$i/makefile".in && echo "XF86VMODE_INC = $found" >> "$i/makefile"
          test -f "$i/makefile".in && echo "XF86VMODE_LIB = -lXxf86vm" >> "$i/makefile"
        done
      fi
    elif [ $OSUNAME = "Linux" ]; then
      echo_="X VidMode extension not found in /usr/X11R6/include/X11/extensions/xf86vmode.h or"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo_="  /usr/include/X11/extensions/xf86vmode.h or"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo_="  $pc_package.pc"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    fi
  fi

  if [ -n "$XINERAMA" ] && [ $XINERAMA -gt 0 ]; then
    found=""
    version=""
    pc_package=xinerama
    if [ -z "$found" ]; then
      pkg-config  --atleast-version=1.0 $pc_package
      if [ $? = 0 ]; then
        found=`pkg-config --cflags $pc_package`
        version=`pkg-config --modversion $pc_package`
      fi
    fi
    if [ -z "$found" ]; then
      if [ -f /usr/X11R6/include/X11/extensions/Xinerama.h ]; then
        found="-I/usr/X11R6/include"
      elif [ -f /usr/include/X11/extensions/Xinerama.h ]; then
        found="-I/usr/include"
      elif [ -f $includedir/X11/extensions/Xinerama.h ]; then
        found="-I$includedir"
      fi
    fi
    if [ -n "$found" ]; then
      if [ -n "$version" ]; then
        echo_="X Xinerama $version        detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      else
        echo_="X Xinerama              detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      fi
      echo "#define HAVE_XIN 1" >> $CONF_H
      if [ -n "$MAKEFILE_DIR" ]; then
        for i in $MAKEFILE_DIR; do
          test -f "$i/makefile".in && echo "XIN = 1" >> "$i/makefile"
          test -f "$i/makefile".in && echo "XINERAMA_INC = $found" >> "$i/makefile"
          test -f "$i/makefile".in && echo "XINERAMA_LIB = -lXinerama" >> "$i/makefile"
        done
      fi
    elif [ $OSUNAME = "Linux" ]; then
        echo_="X Xinerma not found in /usr/X11R6/include/X11/extensions/Xinerama.h or"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        echo_="  /usr/include/X11/extensions/Xinerama.h or"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        echo_="  $pc_package.pc"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    fi
  fi
  echo "X_CPP = \$(X_CPPFILES)" >> $CONF
  if [ -n "$MAKEFILE_DIR" ]; then
    for i in $MAKEFILE_DIR; do
      test -f "$i/makefile".in && echo "X11_LIB_PATH = -L/usr/X11R6/lib\$(BARCH) -L/usr/lib\$(BARCH) -L\$(libdir)" >> "$i/makefile"
    done
  fi
  if [ -n "$CONF_SH" ]; then
    test -f "$CONF_SH".in && echo "X11_LIB_PATH=\"-L/usr/X11R6/lib\$BARCH -L/usr/lib\$BARCH -L\$libdir\"" >> "$CONF_SH"
  fi

  if [ -n "$X_ADD" ]; then
    for l in $X_ADD; do
      rm -f tests/libtest$EXEC_END
      $CXX $CFLAGS -I$includedir tests/lib_test.cxx $LDFLAGS -L/usr/X11R6/lib$BARCH -L/usr/lib$BARCH -L$libdir -l$l -o tests/libtest 2>/dev/null
      if [ -f tests/libtest ]; then
          echo_="lib$l is available"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
          if [ -z "$X_ADD_LIBS" ]; then
            X_ADD_LIBS="-l$l"
          else
            X_ADD_LIBS="$X_ADD_LIBS -l$l"
          fi
          echo "#define HAVE_$l 1"  >> $CONF_H
          # Test if we need to link explicitely, possibly symbols are included
          if [ $OSUNAME = "Darwin" ]; then
            if [ -n "`otool -L tests/libtest | grep $l`" ]; then
              echo "$l=-l$l" >> "$CONF_TEMP_SH"
            fi
          else
            if [ -n "`ldd tests/libtest | grep $l`" ]; then
              echo "$l=-l$l" >> "$CONF_TEMP_SH"
            fi
          fi
          rm tests/libtest$EXEC_END
      else
        if [ $X11 -eq 1 ]; then
          echo_="!!! ERROR lib$l is missed"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
          ERROR=1
        else
          echo_="  Warning lib$l is missed"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        fi
      fi
    done
  fi
  if [ -n "$X_ADD_2" ]; then
    for l in $X_ADD_2; do
      rm -f tests/libtest$EXEC_END
      $CXX $CFLAGS -I$includedir tests/lib_test.cxx $LDFLAGS -L/usr/X11R6/lib$BARCH -L/usr/lib$BARCH -L$libdir -l$l -o tests/libtest 2>/dev/null
      if [ -f tests/libtest ]; then
          echo_="lib$l is available"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
          if [ -z "$X_ADD_LIBS" ]; then
            X_ADD_LIBS="-l$l"
          else
            X_ADD_LIBS="$X_ADD_LIBS -l$l"
          fi
          echo "#define HAVE_$l 1"  >> $CONF_H
          echo "$l=-l$l" >> "$CONF_TEMP_SH"
          rm tests/libtest$EXEC_END
      else
        echo_="lib$l not found"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      fi
    done
  fi
  if [ -n "$MAKEFILE_DIR" ]; then
    for i in $MAKEFILE_DIR; do
      test -f "$i/makefile".in && echo "X11_INCL=\$(XF86VMODE_INC) \$(XINERAMA_INC)" >> "$i/makefile"
      test -f "$i/makefile".in && echo "X11_LIBS=\$(X11_LIB_PATH) -lX11 \$(XF86VMODE_LIB) $X_ADD_LIBS \$(XINERAMA_LIB)" >> "$i/makefile"
    done
  fi
fi
if [ -n "$X11" ] && [ $X11 -gt 0 ]; then
    found=""
    version=""
    pc_package=x11
    if [ -z "$found" ]; then
      pkg-config  --atleast-version=1.0 $pc_package
      if [ $? = 0 ]; then
        found=`pkg-config --cflags $pc_package`
        version=`pkg-config --modversion $pc_package`
      fi
    fi
    if [ -z "$found" ]; then
      if [ -f /usr/X11R6/include/X11/Xlib.h ]; then
        found="-I/usr/X11R6/include"
      elif [ -f /usr/include/X11/Xlib.h ]; then
        found="-I/usr/include"
      elif [ -f $includedir/X11/Xlib.h ]; then
        found="-I$includedir"
      fi
    fi
    if [ -n "$found" ]; then
      if [ -n "$version" ]; then
        echo_="X11 $version               detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      else
        echo_="X11                     detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      fi
      echo "#define HAVE_X 1" >> $CONF_H
      if [ -n "$MAKEFILE_DIR" ]; then
        for i in $MAKEFILE_DIR; do
          test -f "$i/makefile".in && echo "X11 = X11" >> "$i/makefile"
          test -f "$i/makefile".in && echo "X_H = -I/usr/X11R6/include -I/usr/include $found \$(X11_INCL)" >> "$i/makefile"
        done
      fi
    elif [ $OSUNAME = "Linux" ]; then
      echo_="X11 header not found in /usr/X11R6/include/X11/Xlib.h or"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo_="  /usr/include/X11/Xlib.h or"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo_="  $pc_package.pc"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      X11=0
    fi
fi

if [ -n "$FTGL" ] && [ $FTGL -gt 0 ]; then
  pkg-config  --atleast-version=1.0 ftgl
  if [ $? = 0 ]; then
    echo_="FTGL      `pkg-config --modversion ftgl`         detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    echo "#define HAVE_FTGL 1" >> $CONF_H
    echo "FTGL = 1" >> $CONF
    echo "FTGL_H = `pkg-config --cflags ftgl | sed \"$STRIPOPT\"`" >> $CONF
    echo "FTGL_LIBS = `pkg-config --libs ftgl | sed \"$STRIPOPT\"`" >> $CONF
  else
    l=ftgl 
    rm -f tests/libtest$EXEC_END
    $CXX $CFLAGS -I$includedir tests/lib_test.cxx $LDFLAGS -L/usr/X11R6/lib$BARCH -L/usr/lib$BARCH -L$libdir -l$l -o tests/libtest 2>/dev/null
    if [ -f tests/libtest ]; then
      echo_="FTGL                    detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo "#define HAVE_FTGL 1" >> $CONF_H
      echo "FTGL = 1" >> $CONF
      echo "FTGL_H =" >> $CONF
      echo "FTGL_LIBS = -lftgl -lfreetype" >> $CONF
      rm tests/libtest$EXEC_END
    else
      echo_="  no or too old FTGL found, need FTGL to render text in OpenGL"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    fi
  fi
fi

if [ -z "$fltkconfig" ]; then
  # add /usr/X11R6/bin to path for Fedora
  fltkconfig=fltk-config
  PATH=$PATH:/usr/X11R6/bin; export PATH
  echo_="add fltk-config"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
fi
if [ -n "$FLTK" ] && [ $FLTK -gt 0 ]; then
  FLTK_="`$fltkconfig --api-version 2>>$CONF_LOG | sed \"$STRIPOPT\"`"
  if [ $? = 0 ] && [ -n "$FLTK_" ]; then
    # check for utf-8 capability
    if [ $fltkconfig != `echo $fltkconfig | sed "s%fltk2-config%% ; s%utf8%%"` ]; then
      echo "#define HAVE_FLTK_UTF8 1" >> $CONF_H
      echo "HAVE_FLTK_UTF8 = -DHAVE_FLTK_UTF8" >> $CONF_I18N
      fltk_utf8="utf-8 `$fltkconfig --version`"
    else
      fltk_utf8="`$fltkconfig --version`      "
    fi
    echo_="FLTK $fltk_utf8        detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    if [ "0" -ne "`$fltkconfig --compile tests/fltk_test.cxx 2>&1 | grep lock | wc -l`" ]; then
      echo_="!!! ERROR: FLTK has no threads support !!!"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo_="           Configure FLTK with the --enable-threads option and recompile."; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      $fltkconfig --compile tests/fltk_test.cxx 1>> $CONF_LOG 2>> $CONF_LOG
      ERROR=1
    else
      test -f fltk_test$EXEC_END && rm fltk_test$EXEC_END || rm fltk-test$EXEC_END
    fi
    if [ -z "$fltkldflags" ]; then
      fltkldflags="--ldflags"
    fi
    echo "#define HAVE_FLTK 1" >> $CONF_H
    echo "FLTK = 1" >> $CONF
    echo "FLTK_H = `$fltkconfig --cxxflags | sed \"$STRIPOPT\"`" >> $CONF
    echo "FLTK_LIBS = `$fltkconfig --use-images --use-gl $fltkldflags | sed \"$STRIPOPT\"`" >> $CONF
    echo "fltkconfig = $fltkconfig" >> $CONF
    echo "FLTK = 1" >> $CONF_I18N
    echo "FLTK_H = `$fltkconfig --cxxflags | sed \"$STRIPOPT\"`" >> $CONF_I18N
    echo "FLTK_LIBS = `$fltkconfig --use-images --use-gl $fltkldflags | sed \"$STRIPOPT\"`" >> $CONF_I18N
    echo "fltkconfig = $fltkconfig" >> $CONF_I18N

  else
    if [ $FLTK -eq 1 ]; then
      ERROR=1
      echo_="!!! ERROR !!!"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    else
      echo_="    Warning"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    fi
    echo_="           FLTK ($fltkconfig) is not found; download: www.fltk.org"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
  fi
fi

if [ -n "$FLU" ] && [ $FLU -gt 0 ]; then
  FLU_=`flu-config --cxxflags 2>>$CONF_LOG`
  if [ "`$fltkconfig --version`" = "1.1.7" ]; then
    echo -e "\c"
    #"
    echo_="FLTK version 1.1.7 is not supported by FLU"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    if [ "$FLU" = 1 ]; then
      ERROR=1
    fi
  else
    if [ -n "$FLU_" ] && [ -n "$FLTK_" ]; then
      echo_="FLU                     detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo "#define HAVE_FLU 1" >> $CONF_H
      echo "FLU = 1" >> $CONF
      echo "FLU_H = `flu-config --cxxflags | sed \"$STRIPOPT\"`" >> $CONF
      echo "FLU_LIBS = `flu-config --ldflags --use-gl | sed \"$STRIPOPT\"`" >> $CONF
    else
      if [ "$FLU" -gt 1 ]; then
        echo_="   no FLU found, will not use it"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      else
        echo_="ERROR:   FLU is not found; download:"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        echo_="         http://www.osc.edu/~jbryan/FLU/http://www.osc.edu/~jbryan/FLU/"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
        ERROR=1
      fi
    fi
  fi
fi

if [ -n "$DOXYGEN" ] && [ $DOXYGEN -gt 0 ]; then
  if [ "`doxygen --help`" != "" ]; then
    echo_="Doxygen `doxygen --version`           detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
  else
    echo_="Doxygen                 not detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
  fi
fi

if [ -n "$LIBPNG" ] && [ $LIBPNG -gt 0 ]; then
  LIBPNG=libpng
  pkg-config  --atleast-version=1.0 $LIBPNG 2>>$CONF_LOG
  if [ $? != 0 ]; then
    LIBPNG=libpng12
    pkg-config  --atleast-version=1.0 $LIBPNG 2>>$CONF_LOG
  fi
  if [ $? = 0 ]; then
    echo_="PNG `pkg-config --modversion $LIBPNG`               detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    echo "#define HAVE_PNG 1" >> $CONF_H
    echo "PNG = 1" >> $CONF
    echo "PNG_H = `pkg-config --cflags $LIBPNG | sed \"$STRIPOPT\"`" >> $CONF
    echo "PNG_LIBS = `pkg-config --libs $LIBPNG | sed \"$STRIPOPT\"`" >> $CONF
  else
    echo_="no or too old libpng found,"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    echo_="  need at least version 1.0, download: www.libpng.org"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
  fi
fi

if [ -n "$LIBTIFF" ] && [ $LIBTIFF -gt 0 ]; then
  rm -f tests/libtest$EXEC_END
  $CXX $CFLAGS -I$includedir tests/tiff_test.cxx $LDFLAGS -L$libdir -ltiff -ljpeg -o tests/libtest 2>>$CONF_LOG
    if [ -f tests/libtest ]; then
      echo_="`tests/libtest`
                        detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo "#define HAVE_TIFF 1" >> $CONF_H
      echo "TIFF = 1" >> $CONF
      rm tests/libtest$EXEC_END
    else
      echo_="no or too old libtiff found,"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    fi
fi

if [ -n "$GETTEXT" ] && [ $GETTEXT -gt 0 ]; then
  rm -f tests/libtest$EXEC_END
    $CXX $CFLAGS -I$includedir tests/gettext_test.cxx $LDFLAGS -L$libdir -o tests/libtest 2>/dev/null
    if [ ! -f tests/libtest ]; then
       echo $CXX $CFLAGS -I$includedir tests/gettext_test.cxx $LDFLAGS -L$libdir -lintl -o tests/libtest >> $CONF_LOG
       $CXX $CFLAGS -I$includedir tests/gettext_test.cxx $LDFLAGS -L$libdir -lintl -o tests/libtest 2>>$CONF_LOG
    fi
    if [ -f tests/libtest ]; then
      echo_="Gettext                 detected"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
      echo "#define USE_GETTEXT 1" >> $CONF_H
      echo "GETTEXT = -DUSE_GETTEXT" >> $CONF
      echo "GETTEXT = -DUSE_GETTEXT" >> $CONF_I18N
      rm tests/libtest$EXEC_END
    else
      echo_="no or too old Gettext found,"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
    fi
fi

if [ -n "$PO" ] && [ $PO -gt 0 ]; then
  pos_dir="`ls po/*.po 2> /dev/null`"
  LING="`echo $pos_dir`"
  LINGUAS="`echo $pos_dir | sed 's/\.po//g ; s/po\///g'`"
  echo "LINGUAS = $LINGUAS" >> $CONF
  echo_="translations available: $LINGUAS"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
  echo "LING = $LING" >> $CONF
  #echo "#define USE_GETTEXT 1" >> $CONF_H
fi

if [ -n "$PREPARE_MAKEFILES" ] && [ $PREPARE_MAKEFILES -gt 0 ]; then
  if [ -n "$MAKEFILE_DIR" ]; then
    for i in $MAKEFILE_DIR; do
      echo_="preparing Makefile in $i/"; echo "$echo_" >> $CONF_LOG; test -n "$ECHO" && $ECHO "$echo_"
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
      fi
      if [ "$verbose" -eq "0" ]; then
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

