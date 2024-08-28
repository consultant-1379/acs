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
Summary:   Installation package for CHB HeartBeat. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for CHB HeartBeat.

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

cp %chb_cxc_path/bin/acs_chbheartbeatd 		$RPM_BUILD_ROOT%ACSBINdir/acs_chbheartbeatd
cp %chb_cxc_path/bin/acs_chbheartbeatchildd 	$RPM_BUILD_ROOT%ACSBINdir/acs_chbheartbeatchildd
cp %chb_cxc_path/bin/acs_chb_heartbeat_clc 	$RPM_BUILD_ROOT%ACSBINdir/acs_chb_heartbeat_clc
cp %chb_cxc_path/bin/lib_ext/libacs_chb_cpobject.so.2.1.1 $RPM_BUILD_ROOT%ACSLIB64dir/libacs_chb_cpobject.so.2.1.1
cp %chb_cxc_path/conf/APZIM_HeartBeat_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_HeartBeat_imm_classes.xml
cp %chb_cxc_path/conf/APZIM_HeartBeat_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_HeartBeat_imm_objects.xml
cp %chb_cxc_path/conf/ha_acs_chb_heartbeat_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_chb_heartbeat_objects.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_chbheartbeatd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_chbheartbeatchildd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_chb_heartbeat_clc
chmod +x $RPM_BUILD_ROOT%ACSLIB64dir/libacs_chb_cpobject.so.2.1.1
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_HeartBeat_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_HeartBeat_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_chb_heartbeat_objects.xml
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_chb_cpobject.so.2.1.1 $RPM_BUILD_ROOT%{_lib64dir}/libacs_chb_cpobject.so.2
ln -sf $RPM_BUILD_ROOT%{_lib64dir}/libacs_chb_cpobject.so.2 $RPM_BUILD_ROOT%{_lib64dir}/libacs_chb_cpobject.so
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_chbheartbeatd $RPM_BUILD_ROOT%{_bindir}/acs_chbheartbeatd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_chbheartbeatchildd $RPM_BUILD_ROOT%{_bindir}/acs_chbheartbeatchildd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_chb_heartbeat_clc $RPM_BUILD_ROOT%{_bindir}/acs_chb_heartbeat_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_lib64dir}/libacs_chb_cpobject.so.2
	rm -f %{_lib64dir}/libacs_chb_cpobject.so
	rm -f %{_bindir}/acs_chbheartbeatd
	rm -f %{_bindir}/acs_chbheartbeatchildd
	rm -f %{_bindir}/acs_chb_heartbeat_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/acs_chbheartbeatd
	rm -f %ACSBINdir/acs_chbheartbeatchildd
	rm -f %ACSBINdir/acs_chb_heartbeat_clc
	rm -f %ACSLIB64dir/libacs_chb_cpobject.so.2.1.1
	rm -f %ACSCONFdir/APZIM_HeartBeat_imm_classes.xml
	rm -f %ACSCONFdir/APZIM_HeartBeat_imm_objects.xml
	rm -f %ACSCONFdir/ha_acs_chb_heartbeat_objects.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/acs_chbheartbeatd
%ACSBINdir/acs_chbheartbeatchildd
%ACSBINdir/acs_chb_heartbeat_clc
%ACSLIB64dir/libacs_chb_cpobject.so.2.1.1
%ACSCONFdir/APZIM_HeartBeat_imm_classes.xml
%ACSCONFdir/APZIM_HeartBeat_imm_objects.xml
%ACSCONFdir/ha_acs_chb_heartbeat_objects.xml

%changelog
* Wed Nov 30 2011 - ramakrishna.maturu (at) tcs.com
-Initial Implementation
-Changed library revision

