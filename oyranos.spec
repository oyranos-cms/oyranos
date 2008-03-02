Summary: Colour Management System
Name: oyranos
Version: 0.1.0
Release: 1
License: GPL,LGPL
Group: Development/Libraries
Source: oyranos_0.1.0.tar.gz
#Source: http://www.behrmann.name/index.php?option=com_content&task=view&id=34&Itemid=68
URL: http://www.oyranos.org
Vendor: Kai-Uwe Behrmann
Packager: Kai-Uwe Behrmann <ku.b@gmx.de>
BuildRoot: %{_tmppath}/%{name}-root
Prefix:    %{_prefix}

%description
Oyranos will be first implemented and used to communicate
colour relevant information on free operating systems.
Developers from other OSes can jump in the current
disscussion to make Oyranos aware of cross platform
standards. You are cordially invited to make constructive
suggestions.

Authors:
--------
    Oyranos     Kai-Uwe Behrmann <www.behrmann.name>

used components and libraries:
    FLTK        Bill Spizak and others
    FLU         Jason Bryan
    Elektra     Avi Alkalay and others

%prep
%setup -q -n %{name}_%{version}
#-%{release}

%build
%configure
configure --prefix %{_prefix}

%install
rm -rf %{buildroot}
make
make oyranos_config_flu_static
make DESTDIR=%{buildroot} install_gui

%clean
rm -rf %{buildroot}

%post

%postun

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog README
%{_bindir}/oyranos_config_flu


%changelog
* Tue Apr 21 2005 Kai-Uwe Behrmann
- first release


