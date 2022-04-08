The feature list can be found on the
[Oyranos](http://www.oyranos.org/features) site.

* [What is Oyranos?](#what-is-oyranos)
* [What is the goal of Oyranos?](#what-is-the-goal-of-oyranos)
* [Where comes the name Oyranos from?](#where-comes-the-name-oyranos-from)
* [License(s)](#licenses)
* [Where can I get Oyranos?](#where-can-i-get-oyranos)
* [Where can I find ICC Profiles](#where-can-i-find-icc-profiles)
* [Who is developing Oyranos?](#who-is-developing-oyranos)
* [How can I help with the Oyranos CMS?](#how-can-i-help-with-the-oyranos-cms)
* [Which applications use Oyranos?](#which-applications-use-oyranos)
* [Can Oyranos replace other Colour management Systems?](#can-oyranos-replace-other-colour-management-systems)
* [Does Oyranos rely on FLTK?](#does-oyranos-rely-on-fltk)
* [Elektra dictates the core design?](#elektra-dictates-the-core-design)
* [Elektra is a lively project?](#elektra-is-a-lively-project)
* [What about "Mixing the configuration, with the UI, with the back end."?](#what-about-mixing-the-configuration-with-the-ui-with-the-back-end)
* [What about "Reliance on compiz for full screen color management."?](#what-about-reliance-on-compiz-for-full-screen-color-management)
* [Why has Oyranos a own type and object system?](#why-has-oyranos-a-own-type-and-object-system)
* [Distribution Packages](#distribution-packages)

If you have the impression an important question is missed, feel free to
add it.

### What is Oyranos?

The Oyranos project is a colour management system (CMS). It is intended
to work in a comparable fashion like KCMS, ICM or ColorSync expect it
is published with sources and is available for various platforms.

### What is the goal of Oyranos?

Oyranos shall help in making end to end colour management working for
open source operating systems. Ideally that works for most users without
intervention.

With colour management being a often complicated pattern, the need rises
to easily understand what goes on behind the scenes. Doing this once is
often hard enough. Having different colour settings in every application
and service may easily multiply the work to reliable understand how
colours get managed. Thus Oyranos is a call to collaborate between
applications to make them all more attractive.

For instance all Oyranos conform applications should display colour
content on the monitor in the same manner, equally if the user has set
soft proofing on by default in the Oyranos configuration panel or not.

Oyranos provides colour management services for desktops, applications
and services like KDE, Gnome, Scribus, CinePaint, Krita, Gimp, Inkscape,
Gutenprint, UFraw, Cups, Xorg, Sane and so on.

### Where comes the name Oyranos from?

Oyranos is the greek word of sky.

It was pointed out that ouranos is more adequate for english readers.
Just as with the german Uranos and other variants it seems pretty good
to stay with the name.

### License(s)

Oyranos is licensed under [BSD-3-Clause](http://www.opensource.org/licenses/BSD-3-Clause). It is possible
that parts will be released under different licenses. But that is open.
The goal is to not exclude anyone from using Oyranos. The ICC profiles
are licensed under
[zip/libpng](http://opensource.org/licenses/zlib-license.php) license.
They are separately packaged in the OpenICC-data package.

### Where can I get Oyranos?

Oyranos is officially published on
[Oyranos/Download](http://www.oyranos.org/downloads/ "Link").

### Where can I find ICC Profiles

Some are already packaged in OpenICC on www.sourceforge.net, like sRGB,
ECI- and a Adobe RGB as well as Gray, CIE\*Lab, CIE\*XYZ, ITU-Lab and
some press profiles like for FOGRA, SWOP, SNAP and GRACoL printing
conditions. The used printing condition character sets are as well
included. More profiles can be found on this [link
collection](http://www.behrmann.name/index.php%3Foption=com_weblinks&catid=73&Itemid=95.html).
They should be installed in the [system
paths](http://www.oyranos.org/wiki/index.php%3Ftitle=OpenIccDirectoryProposal.html "wikilink") to be seen by most
applications.

### Who is developing Oyranos?

Oyranos was started by [Kai-Uwe Behrmann](http://www.behrmann.name).
Yiannis Belias has joined. More details can be read in the AUTHORS file
in the sources. Still we are searching for
[help](https://gitlab.com/oyranos "gitlab development page"). Code
contributions came from students through their
[Summer](http://www.freedesktop.org/wiki/OpenIccForGoogleSoC2008/#colormanagementnearx)
of
[Code](http://code.google.com/p/google-summer-of-code-2008-openicc/downloads/list)
[projects](http://freedesktop.org/wiki/OpenIccForGoogleSoC2008) at
[OpenICC](http://www.openicc.info "wikilink"). OpenICC people help very much around
organisation of GSoC.

### How can I help with the Oyranos CMS?

You could help by:

-   testing
-   reporting bugs
-   providing according patches
-   doing translations
-   suggesting useful changes
-   proofreading, updating and extenting the documentation
-   write a tutorial on installing and using
-   package Oyranos, the OpenICC profiles and the other depending
    projects

Becoming part of the development team is easy, just ask.

So if your have a question around Oyranos thats a good sign to 
start ask that.

See as well [here](https://gitlab.com/oyranos/oyranos/-/issues?label_name[]=help%20wanted "wikilink").

A suggestion list can be found [here](https://gitlab.com/oyranos/oyranos/-/issues?label_name[]=enhancement "wikilink").

### Which applications use Oyranos?

A list of Oyranos users is [here](http://www.oyranos.org/about/index.html).

### Can Oyranos replace other Colour management Systems?

This is not intentional. Each colour management system has its strengths
not easily reachable by an other. If your application runs just on osX
it is possibly best to stay with the advanced features of ColourSync. Is
your intention to run your application on different platforms such as
Windows, osX and the various unixes, Oyranos may be of much value, as it
will provide a single API allowing real cross platform development.
Oyranos tries to use as many as possible of the underlying colour
management services, as long as it conforms to its cross platform
behaviour.

Naturally advanced features will be unlikely to get supported as they
are not cross platform, and would have to been accessed directly.

### Does Oyranos rely on FLTK?

Oyranos comes with a front end GUI written in FLTK. However building
that can be switched off during configuration, in case a desktop decides
to have a equivalent or better replacement as front end. E.g. KDE might
ship the [KolorManager](http://www.oyranos.org/kolormanager) front end
to Oyranos instead. A Gtk front end would be great too.

### Elektra dictates the core design?

No. Elektra provides just a API to store settings in a data base. Thats
a very common task and available through gconf or the Qt framework. But
Elektra is cross desktop. The Elektra API calls are abstracted in
Oyranos. So Elektra can be replaced by an other desktop independent
configuration engine, e.g. "oiDB". But as the Elektra project sees
continuing development there is currently no reason. It is planed to
formalise settings storage and share that among CMS's.

### Elektra is a lively project?

As of writing, in 2019, Elektra exists after more than 15 years
continually development. The noise on email lists is not very loud, but
there is academic interest and industry use for Elektra. Its a small and
specialised project with a very focused problem to solve. Thus is is not
uncommon to see fewer comments around the project.

### What about "Mixing the configuration, with the UI, with the back end"?

You guess it right. This statement is nonsense. Configuration, UI and
back ends are very well separated. E.g. you do not need to call into a
toolkit to access the configuration data base. oyranos-policy as a
command line tool is an examples for not relying on Xorg to be usable.

### What about "Reliance on compiz for full screen color management"?

The compiz ICC colour server is at the time of writing the only
available open source solution to allow colour management across the
full desktop on multiple monitors. So naturally it is recommended,
because it is technical superior to most other desktop colour correction
strategies. E.g. The loading or the 'vcgt' tag is only a calibration,
which helps not much with wide gamut displays. However the underlying
net-color spec is open and free to be implemented in say Xorg. The KDE
KWin window manager has seen [two](https://www.freedesktop.org/wiki/OpenIcc/GoogleSoC2012/#kwincolourcorrection) efforts too.

### Why has Oyranos a own type and object system?

The decision came as we realised the project needs some efficient way to
manage its internal objects and classes alike structures. As the
selected programming language comes with no explicit support for that it
was written from scratch. We looked as well on other systems for
instance Glib. But we found that the language extension for inheritance
and so on where realised outside of the core C language specification in
a Macro called second language, which is commonly referred to as bad
style programming. So Glib would have come with lots of disadvantages.

### Distribution Packages

Please see the related distribution packages. You might find the [Open
Build Service](https://www.oyranos.org/downloads/index.html) packages
useful. There are stable and development versions available.

