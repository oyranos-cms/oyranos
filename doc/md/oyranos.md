# oyranos  {#oyranos}
[NAME](#name) [DESCRIPTION](#description) [SYNOPSIS](#synopsis) [API DESCRIPTION](#apidescription) [SEE ALSO](#seealso) [AUTHOR](#author) [BUGS](#bugs) 

## NAME <a name="name"></a>
oyranos  \- Oyranos color management system (CMS)
## DESCRIPTION <a name="description"></a>
The Oyranos color management system (CMS) is designed as a entry point for color savy applications. In its current stage it configures profile paths, sets default profiles, maps devices to profiles, sets a monitor profile in X and uploads a vcgt tag. This means for instance all applications using Oyranos will use for incoming digital camera picture the same profile and watch it through the same monitor profile. For the future is planed to offer color conversions through a framework inside Oyranos. Installation and selection of CMM's will then happen transparently. Device profiles will be better supported. Options are planed for HDR imagery handling.
## SYNOPSIS <a name="synopsis"></a>
### Include the appropriate headers
&nbsp;&nbsp;#include <oyranos.h>
### and compile
&nbsp;&nbsp;cc \`oyranos\-config \-\-cflags\` \`oyranos\-config \-\-ldflags\` mycode.c \-o myApp
  <br />
&nbsp;&nbsp;A pkg\-config file is provided as well.
## API DESCRIPTION <a name="apidescription"></a>
### A html documentation should come with your installation. Try otherwise
### http://www.oyranos.org/doc/index.html
## SEE ALSO <a name="seealso"></a>
###  [oyranos\-config](oyranosconfig.html)<a href="oyranosconfig.md">(3)</a>  [oyranos\-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>  [oyranos\-monitor](oyranosmonitor.html)<a href="oyranosmonitor.md">(1)</a>
## AUTHOR <a name="author"></a>
Kai\-Uwe Behrmann (ku.b (at) gmx.de) 
## BUGS <a name="bugs"></a>
at: [https://github.com/oyranos\-cms/oyranos/issues](https://github.com/oyranos\-cms/oyranos/issues)

