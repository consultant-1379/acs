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
%define logm_cxc_path %{_cxcdir}
Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for LOGM. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 


%description
Installation Package for LOGM

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


cp %logm_cxc_path/bin/acs_logmaintd  $RPM_BUILD_ROOT%ACSBINdir/acs_logmaintd
cp %logm_cxc_path/bin/acs_logm_logmaint_clc $RPM_BUILD_ROOT%ACSBINdir/acs_logm_logmaint_clc
cp %logm_cxc_path/conf/APZIM_LogMaintenanceM_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_LogMaintenanceM_imm_classes.xml
cp %logm_cxc_path/conf/APZIM_LogMaintenanceM_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_LogMaintenanceM_imm_objects.xml
cp %logm_cxc_path/conf/ha_acs_logm_logmaint_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_logm_logmaint_objects.xml


%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_logmaintd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_logm_logmaint_clc
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_LogMaintenanceM_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_LogMaintenanceM_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_logm_logmaint_objects.xml
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_logmaintd $RPM_BUILD_ROOT%{_bindir}/acs_logmaintd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_logm_logmaint_clc $RPM_BUILD_ROOT%{_bindir}/acs_logm_logmaint_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"
	rm -f %{_bindir}/acs_logmaintd
	rm -f %{_bindir}/acs_logm_logmaint_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"
	rm -f %ACSBINdir/acs_logmaintd
	rm -f %ACSBINdir/acs_logm_logmaint_clc
	rm -f %ACSCONFdir/APZIM_LogMaintenanceM_imm_classes.xml
	rm -f %ACSCONFdir/APZIM_LogMaintenanceM_imm_objects.xml
	rm -f %ACSCONFdir/ha_acs_logm_logmaint_objects.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/acs_logmaintd
%ACSBINdir/acs_logm_logmaint_clc
%ACSCONFdir/APZIM_LogMaintenanceM_imm_classes.xml
%ACSCONFdir/APZIM_LogMaintenanceM_imm_objects.xml
%ACSCONFdir/ha_acs_logm_logmaint_objects.xml


%changelog
* Wed Sep 28 2011  -Shyam Chirania  (at) tcs.com
* Thu Jul 14 2011  - Harika Bavana  (at) tcs.com
* Fri Nov 28  2010 - sk.jaiswal     (at) tcs.com
- Added LOGM functionality to the implementation
- Added XML for PHA  
- Modified as per Latest DR - prabhu.tsk (at) tcs.com
- Added XML, CLC script for HA - bavana.harika (at) tcs.com
- MOdified spec file according to latest DR


