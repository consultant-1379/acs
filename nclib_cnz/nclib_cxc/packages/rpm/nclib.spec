#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#

Name:      %{_name}
Summary:   Installation package for NCLIB.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires: APOS_OSCONFBIN

#BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%define nclib_cxc_path %{_cxcdir}

%define _NCLIB_LIB_NAME libacs_nclib.so.1.4.0
%define _NCLIB_LIB_LINKNAME libacs_nclib.so.1
%define _NCLIB_LIB_SONAME libacs_nclib.so

%define _NCGET_CMD ncget
%define _NCEDITCFG_CMD nceditconfig
%define _NCACTION_CMD ncaction
%define _IRONSIDE_CMD ironsidecmd

%description
Installation package for NCLIB.

%pre

%install
echo "Installing ACS_NCLIB package"

mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/bin
mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/lib64
mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/conf

cp %nclib_cxc_path/bin/lib_ext/%_NCLIB_LIB_NAME $RPM_BUILD_ROOT/opt/ap/acs/lib64/%_NCLIB_LIB_NAME

cp %nclib_cxc_path/bin/%_NCGET_CMD $RPM_BUILD_ROOT/opt/ap/acs/bin/%_NCGET_CMD
cp %nclib_cxc_path/bin/%_NCEDITCFG_CMD $RPM_BUILD_ROOT/opt/ap/acs/bin/%_NCEDITCFG_CMD
cp %nclib_cxc_path/bin/%_NCACTION_CMD $RPM_BUILD_ROOT/opt/ap/acs/bin/%_NCACTION_CMD
cp %nclib_cxc_path/bin/%_IRONSIDE_CMD $RPM_BUILD_ROOT/opt/ap/acs/bin/%_IRONSIDE_CMD

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

echo "Finalizing ACS_NCLIB package installation"
ln -sf /opt/ap/acs/lib64/%_NCLIB_LIB_NAME $RPM_BUILD_ROOT/usr/lib64/%_NCLIB_LIB_LINKNAME
ln -sf %_NCLIB_LIB_LINKNAME $RPM_BUILD_ROOT/usr/lib64/%_NCLIB_LIB_SONAME

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"		
		rm -f /usr/lib64/%_NCLIB_LIB_LINKNAME
		rm -f /usr/lib64/%_NCLIB_LIB_SONAME
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"	
		rm -f /opt/ap/acs/lib64/%_NCLIB_LIB_NAME
		rm -f /opt/ap/acs/lib64/%_NCLIB_LIB_LINKNAME
		rm -f /opt/ap/acs/lib64/%_NCLIB_LIB_SONAME

		rm -f /opt/ap/acs/bin/%_NCGET_CMD
		rm -f /opt/ap/acs/bin/%_NCEDITCFG_CMD
		rm -f /opt/ap/acs/bin/%_NCACTION_CMD
		rm -f /opt/ap/acs/bin/%_IRONSIDE_CMD

fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
/opt/ap/acs/lib64/%_NCLIB_LIB_NAME
/opt/ap/acs/bin/%_NCGET_CMD
/opt/ap/acs/bin/%_NCEDITCFG_CMD
/opt/ap/acs/bin/%_NCACTION_CMD
/opt/ap/acs/bin/%_IRONSIDE_CMD
#/opt/ap/ntacs/conf/ha_acs_nclib_objects.xml

%changelog
* Tue Feb 17 2015 - stefano.volpe (at) tei.it
- Added Ironside command
* Fri Jan 31 2014 - stefano.volpe (at) tei.it
- Added NETCONF commands
* Thu Oct 18 2012 - stefano.volpe (at) tei.it
- Initial implementation

