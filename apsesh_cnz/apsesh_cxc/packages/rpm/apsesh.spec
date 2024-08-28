#
# spec file for configuration of package apsesh service
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

%define packer %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)
%define _topdir /var/tmp/%(echo "$USER")/rpms/
%define _tmppath %_topdir/tmp
%define apsesh_cxc_path %{_cxcdir}

Name:      %{_name} 
Summary:   Installation package for APSESH. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Application 
BuildRoot: %_tmppath 
AutoReq: no
Requires:  APOS_OSCONFBIN

%description
Installation package for APSESH Service

%install
echo " This is the APSESH package install section"

mkdir -p $RPM_BUILD_ROOT/%rootdir
mkdir -p $RPM_BUILD_ROOT/%APDir
mkdir -p $RPM_BUILD_ROOT/%ACSdir
mkdir -p $RPM_BUILD_ROOT/%ACSLIB64dir
mkdir -p $RPM_BUILD_ROOT/%ACSBINdir
mkdir -p $RPM_BUILD_ROOT/%ACSdir/conf

cp -af %apsesh_cxc_path/bin/acs_apseshd $RPM_BUILD_ROOT/%ACSBINdir
cp -af %apsesh_cxc_path/script/acs_apsesh_clc $RPM_BUILD_ROOT/%ACSBINdir
cp -af %apsesh_cxc_path/conf/ha_acs_apsesh_2N_objects.xml $RPM_BUILD_ROOT%ACSdir/conf

%clean
rm -rf $RPM_BUILD_ROOT/*

%pre
echo " This is the APSESH package preinstall section"

%post
echo " This is the APSESH package post install section"

if [ "$1" == "2" ];then
        # Upgrade happens, delete all links which are created in Old version, & recreate them again
        # If no changes, leave this section blank
		#rm -f %_bindir/acs_apseshd
		echo " This is the APSESH package post install section during upgrade phase"
		
fi

echo " Creating APSESH links in %_bindir"
ln -sf %ACSBINdir/acs_apseshd %_bindir/acs_apseshd

echo " Done with APSESH post install activities"

%preun
echo " This is the APSESH package pre uninstall section"

%postun
echo " This is the APSESH package post uninstall section"

if [ "$1" == "0" ]; then
	# * Uninstallation.. remove all the files here! 
	echo " Removing APSESH links in %_bindir"
	rm -f %_bindir/acs_apseshd
	rm -f %_bindir/acs_apsesh_clc
fi

if [ "$1" == "1" ]; then #true for upgrade
        echo "upgrade postun section calls for %{_rel} "
        # Delete any other files which are not necessary in Old revision
        # * Do not delete binaries here *;
fi

echo " Done with APSESH post uninstall activities"

%files
%defattr(-,root,root,-)
%ACSBINdir/acs_apseshd
%ACSBINdir/acs_apsesh_clc
%ACSdir/conf/ha_acs_apsesh_2N_objects.xml

%changelog
* Fri Jan 04 2013 <paolo.elefante (at) ericsson.com>
- Adapted to new design rule
* Wed Aug 10 2011 <magnus.lexhagen (at) cybercom.com> CXC1371367-R1A06
- Added HA files
* Tue Apr 26 2011 <magnus.lexhagen (at) cybercom.com> CXC1371367-R1A01
- Initial implementation 
