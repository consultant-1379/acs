#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#

%define packer %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)
%define _topdir /var/tmp/%(echo "$USER")/rpms/
%define _tmppath %_topdir/tmp
%define chb_cxc_path %{_cxcdir}

Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for CHB Clock Sync. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for CHB Clock Sync

%pre
if [ $1 == 1 ]
then
	echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
fi
if [ $1 == 2 ]
then
	echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
fi

%install
echo "This is the %{_name} package %{_rel} install script"

if [ ! -d $RPM_BUILD_ROOT%rootdir ]
then
        mkdir $RPM_BUILD_ROOT%rootdir
fi
if [ ! -d $RPM_BUILD_ROOT%APdir ]
then
        mkdir $RPM_BUILD_ROOT%APdir
fi
if  [ ! -d $RPM_BUILD_ROOT%ACSdir ]
then
        mkdir $RPM_BUILD_ROOT%ACSdir
fi
if [ ! -d  $RPM_BUILD_ROOT%ACSBINdir ]
then
        mkdir $RPM_BUILD_ROOT%ACSBINdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSCONFdir ]
then
        mkdir $RPM_BUILD_ROOT%ACSCONFdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSLIB64dir ]
then
        mkdir $RPM_BUILD_ROOT%ACSLIB64dir
fi

cp %chb_cxc_path/bin/acs_chbclocksyncd 		$RPM_BUILD_ROOT%ACSBINdir/acs_chbclocksyncd
cp %chb_cxc_path/bin/acs_chb_clocksync_clc 	$RPM_BUILD_ROOT%ACSBINdir/acs_chb_clocksync_clc
cp %chb_cxc_path/conf/APZIM_ClockSync_imm_classes.xml 	$RPM_BUILD_ROOT%ACSCONFdir/APZIM_ClockSync_imm_classes.xml
cp %chb_cxc_path/conf/APZIM_ClockSync_imm_objects.xml 	$RPM_BUILD_ROOT%ACSCONFdir/APZIM_ClockSync_imm_objects.xml
cp %chb_cxc_path/conf/ha_acs_chb_clocksync_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_chb_clocksync_objects.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_chbclocksyncd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_chb_clocksync_clc
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_ClockSync_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_ClockSync_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_chb_clocksync_objects.xml
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_chbclocksyncd $RPM_BUILD_ROOT%{_bindir}/acs_chbclocksyncd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_chb_clocksync_clc $RPM_BUILD_ROOT%{_bindir}/acs_chb_clocksync_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_bindir}/acs_chbclocksyncd
	rm -f %{_bindir}/acs_chb_clocksync_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/acs_chbclocksyncd
	rm -f %ACSBINdir/acs_chb_clocksync_clc
	rm -f %ACSCONFdir/APZIM_ClockSync_imm_classes.xml
	rm -f %ACSCONFdir/APZIM_ClockSync_imm_objects.xml
	rm -f %ACSCONFdir/ha_acs_chb_clocksync_objects.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/acs_chbclocksyncd
%ACSBINdir/acs_chb_clocksync_clc
%ACSCONFdir/APZIM_ClockSync_imm_classes.xml
%ACSCONFdir/APZIM_ClockSync_imm_objects.xml
%ACSCONFdir/ha_acs_chb_clocksync_objects.xml

%changelog
* Wed Sep 28 2011 - tanu.aggarwal (at) tcs.com
* Wed Mar 09 2011 - tanu.aggarwal (at) tcs.com
-Initial Implementation
- Added XML, CLC script for HA - bavana.harika (at) tcs.com
- Removed heartbeat and mtzln specific binaries from this spec file - ramakrishna.maturu (at) tcs.com

