#
# spec file for configuration of package apache
#
# Copyright  (c)  2013  Ericsson 
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#

%define packer %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)
%define _topdir /var/tmp/%(echo "$USER")/rpms/
%define _tmppath %_topdir/tmp
%define hc_cxc_path %{_cxcdir}
%define model_path /opt/com/etc/model

Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for ACS HC. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for ACS HC

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
if [ ! -d $RPM_BUILD_ROOT%model_path ]
then
        mkdir -p $RPM_BUILD_ROOT%model_path
fi

cp %hc_cxc_path/bin/acs_hcd 		$RPM_BUILD_ROOT%ACSBINdir/acs_hcd
cp %hc_cxc_path/bin/acs_hc_clc 		$RPM_BUILD_ROOT%ACSBINdir/acs_hc_clc
cp %hc_cxc_path/conf/c_HealthCheck_imm_classes.xml  $RPM_BUILD_ROOT%ACSCONFdir/c_HealthCheck_imm_classes.xml
cp %hc_cxc_path/conf/o_HealthCheckInstances_imm_objects.xml  $RPM_BUILD_ROOT%ACSCONFdir/o_HealthCheckInstances_imm_objects.xml
cp %hc_cxc_path/conf/HealthCheck_mp.xml      $RPM_BUILD_ROOT%model_path/HealthCheck_mp.xml
cp %hc_cxc_path/conf/rules_msc_context.xml      $RPM_BUILD_ROOT%ACSCONFdir/rules_msc_context.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_hcd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_hc_clc
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/c_HealthCheck_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/o_HealthCheckInstances_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%model_path/HealthCheck_mp.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/rules_msc_context.xml
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_hcd $RPM_BUILD_ROOT%{_bindir}/acs_hcd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_hc_clc $RPM_BUILD_ROOT%{_bindir}/acs_hcs_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"
	rm -f %{_bindir}/acs_hcd
	rm -f %{_bindir}/acs_hc_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/acs_hcd
	rm -f %ACSBINdir/acs_hc_clc
        rm -f %ACSCONFdir/c_HealthCheck_imm_classes.xml
        rm -f %ACSCONFdir/o_HealthCheckInstances_imm_objects.xml
	rm -f %ACSCONFdir/rules_msc_context.xml
        rm -f %model_path/HealthCheck_mp.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/acs_hcd
%ACSBINdir/acs_hc_clc
%ACSCONFdir/c_HealthCheck_imm_classes.xml
%ACSCONFdir/o_HealthCheckInstances_imm_objects.xml
%ACSCONFdir/rules_msc_context.xml
%model_path/HealthCheck_mp.xml
%changelog
* Mon Oct 17 2013 - sridhar.lanka (at) tcs.com
-Edited to place initial rules file in /opt/ap/acs/conf path

