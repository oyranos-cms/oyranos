# oyranos  {#oyranos}
[NAME](#name) [DESCRIPTION](#description) [SYNOPSIS](#synopsis) [API DESCRIPTION](#apidescription) [SEE ALSO](#seealso) [AUTHOR](#author) [BUGS](#bugs) 

<h2> NAME <a name="name"></a></h2>
oyranos  \- Oyranos color management system (CMS)
<h2> DESCRIPTION <a name="description"></a></h2>
The Oyranos color management system (CMS) is designed as a entry point for color savy applications. In its current stage it configures profile paths, sets default profiles, maps devices to profiles, sets a monitor profile in X and uploads a vcgt tag. This means for instance all applications using Oyranos will use for incoming digital camera picture the same profile and watch it through the same monitor profile. For the future is planed to offer color conversions through a framework inside Oyranos. Installation and selection of CMM's will then happen transparently. Device profiles will be better supported. Options are planed for HDR imagery handling.
<h2> SYNOPSIS <a name="synopsis"></a></h2>
<h3> Include the appropriate headers</h3>
&nbsp;&nbsp;#include <oyranos.h>
<h3> and compile</h3>
&nbsp;&nbsp;cc \`oyranos\-config \-\-cflags\` \`oyranos\-config \-\-ldflags\` mycode.c \-o myApp
  <br />
&nbsp;&nbsp;A pkg\-config file is provided as well.
<h2> API DESCRIPTION <a name="apidescription"></a></h2>
A html documentation should come with your installation. Try otherwise
http://www.oyranos.org/doc/index.html
<h2> SEE ALSO <a name="seealso"></a></h2>
&nbsp;&nbsp;[oyranos-config](oyranosconfig.html)<a href="oyranosconfig.md">(3)</a>  [oyranos-policy](oyranospolicy.html)<a href="oyranospolicy.md">(1)</a>  [oyranos-monitor](oyranosmonitor.html)<a href="oyranosmonitor.md">(1)</a>
<h2> AUTHOR <a name="author"></a></h2>
Kai\-Uwe Behrmann (ku.b (at) gmx.de) 
<h2> BUGS <a name="bugs"></a></h2>
at: [https://gitlab.com/oyranos/oyranos/issues](https://gitlab.com/oyranos/oyranos/issues)

