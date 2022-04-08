# Packaging

## Components


### Default ICC Profiles
Make shure the icc-profiles-openicc and icc-profiles-basiccolor-printing2009 
packages are in place. The policy files in the settings directory and the 
default profile selection relies on them. These settings files are essential 
to Oyranos. And applications migth fail if the ICC profiles are not available.


### Libraries
Do not omit the provided libraries for normal distribution. You can consider 
splitting it out into separate packages. E.g. 
packaging liboyranos_CUPS_cmm_module.so 
separately from liboyranos_SANE_cmm_module.so or liboyranos_oyX1_cmm_module.so 
is appreciated for some special cases like headless print servers.

#### Essential modules are:
* liboyranos_oyIM_cmm_module.so
* liboyranos_oicc_cmm_module.so
* liboyranos_oyra_cmm_module.so
* liboyranos_lcm[s,2]\_cmm_module.so

#### X11 functionality depends on:
* liboyranos_oyX1_cmm_module.so
* liboyranos_oydi_cmm_module.so

For more actual details see the provided oyranos.spec file inside the source 
tree.
System libraries can be used instead of the shipped elektra or yail versions.
Do not try to install the included Elektra. It is modified for Oyranos and not 
useful for installing.


### GUI
oyranos-config-fltk is a simple front end to the library. It can be replaced 
with KolorManager, Synnefo or possible other front ends. The device settings
will only appear if the according modules are installed (oyX1,CUPS,SANE,oyRE)


### Tools
oyranos-monitor and oyranos-monitor-daemon depend on X11 modules (oydi+oyX1)
oyranos-profile-graph depends on lCC modules (oicc+lcm[s,2])


## Sources
Online source are on https://gitlab.com/oyranos/oyranos


## Licenses
Oyranos tar ball includes pices with different licenses.
Read the first part of the [COPYING](COPYING.md) file carefully. 


## Documentation
The Doxyfile_intern is for Oyranos developers, not for public installation.


## Dependencies
Can be found in [README](README.md).

