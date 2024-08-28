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
Summary:   Installation package for ACS-APBM.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires: APOS_OSCONFBIN
%define acs_apbmcxc_path %{_cxcdir}


#BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for ACS-APBM.


%pre
if [ $1 -eq 1 ]
then
echo "This is the %{_name} package %{_rel} preinstall script during installation phase"
fi

if [ $1 -eq 2 ]
then
echo "This is the %{_name} package %{_rel} preinstall script during upgrade phase --> start"
echo "removing links"
rm -f /usr/lib64/libacs_apbm.so*
rm -f /usr/lib64/libacs_apbmpip.so*
rm -f /usr/bin/acs_apbmd
rm -f /usr/bin/dShelfMngrLog
rm -f /usr/bin/bios_set
rm -f /usr/bin/hwiprint
rm -f /usr/bin/hwmscbls
rm -f /usr/bin/hwmxls
echo "This is the %{_name} package %{_rel} preinstall script during upgrade phase --> end"
fi


%install
echo "Installing ACS_APBM package"

mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/bin
mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/lib64
#mkdir -p $RPM_BUILD_ROOT/cluster/etc/ap/acs/apbm/conf

cp %acs_apbmcxc_path/bin/lib_ext/libacs_apbm.so.3.3.1 $RPM_BUILD_ROOT/opt/ap/acs/lib64/
cp %acs_apbmcxc_path/bin/lib_ext/libacs_apbmpip.so.2.2.2 $RPM_BUILD_ROOT/opt/ap/acs/lib64/
cp %acs_apbmcxc_path/bin/acs_apbmd $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/dShelfMngrLog $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/bios_set $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/hwiprint $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/hwmscbls $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/hwmxls $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/dShelfMngrLog.sh $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/bios_set.sh $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/hwiprint.sh $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/hwmscbls.sh $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/hwmxls.sh $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_apbmcxc_path/bin/acs_apbm_clc $RPM_BUILD_ROOT/opt/ap/acs/bin/


%post
echo "Finalizing ACS_APBM package installation"
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_apbm.so.3.3.1 $RPM_BUILD_ROOT/usr/lib64/libacs_apbm.so.3
ln -sf libacs_apbm.so.3 $RPM_BUILD_ROOT/usr/lib64/libacs_apbm.so
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_apbmpip.so.2.2.2 $RPM_BUILD_ROOT/usr/lib64/libacs_apbmpip.so.2
ln -sf libacs_apbmpip.so.2 $RPM_BUILD_ROOT/usr/lib64/libacs_apbmpip.so
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_apbmd $RPM_BUILD_ROOT/usr/bin/acs_apbmd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/dShelfMngrLog.sh $RPM_BUILD_ROOT/usr/bin/dShelfMngrLog
ln -sf $RPM_BUILD_ROOT%ACSBINdir/bios_set.sh $RPM_BUILD_ROOT/usr/bin/bios_set
ln -sf $RPM_BUILD_ROOT%ACSBINdir/hwiprint.sh $RPM_BUILD_ROOT/usr/bin/hwiprint
ln -sf $RPM_BUILD_ROOT%ACSBINdir/hwmscbls.sh $RPM_BUILD_ROOT/usr/bin/hwmscbls
ln -sf $RPM_BUILD_ROOT%ACSBINdir/hwmxls.sh $RPM_BUILD_ROOT/usr/bin/hwmxls

%preun
echo "Uninstalling ACS_APBM package"


%postun
if [ $1 -eq 0 ] 
then
echo "This is the %{_name} package %{_rel} postun script during unistall phase --> start"
echo "remove links"
rm -f /usr/bin/dShelfMngrLog
rm -f /usr/bin/hwmxls
rm -f /usr/bin/hwmscbls
rm -f /usr/bin/hwiprint
rm -f /usr/bin/bios_set
rm -f /usr/bin/acs_apbmd
rm -f /usr/lib64/libacs_apbm.so*
rm -f /usr/lib64/libacs_apbmpip.so*

rm -f $RPM_BUILD_ROOT%ACSLIB64dir/libacs_apbm.so*
rm -f $RPM_BUILD_ROOT%ACSLIB64dir/libacs_apbmpip.so*
rm -f $RPM_BUILD_ROOT%ACSBINdir/acs_apbmd
rm -f $RPM_BUILD_ROOT%ACSBINdir/dShelfMngrLog
rm -f $RPM_BUILD_ROOT%ACSBINdir/bios_set
rm -f $RPM_BUILD_ROOT%ACSBINdir/hwiprint
rm -f $RPM_BUILD_ROOT%ACSBINdir/hwmscbls
rm -f $RPM_BUILD_ROOT%ACSBINdir/hwmxls
rm -f $RPM_BUILD_ROOT%ACSBINdir/dShelfMngrLog.sh
rm -f $RPM_BUILD_ROOT%ACSBINdir/bios_set.sh
rm -f $RPM_BUILD_ROOT%ACSBINdir/hwiprint.sh
rm -f $RPM_BUILD_ROOT%ACSBINdir/hwmscbls.sh
rm -f $RPM_BUILD_ROOT%ACSBINdir/hwmxls.sh
rm -f $RPM_BUILD_ROOT%ACSBINdir/acs_apbm_clc
fi


if [ $1 -eq 1 ] 
then
echo "This is the %{_name} package %{_rel} postun script during upgrade phase"
fi


%files
%defattr(-,root,root)
%ACSLIB64dir/libacs_apbm.so.3.3.1
%ACSLIB64dir/libacs_apbmpip.so.2.2.2
%attr(555,root,root) /opt/ap/acs/bin/acs_apbmd
%attr(555,root,root) /opt/ap/acs/bin/dShelfMngrLog
%attr(555,root,root) /opt/ap/acs/bin/bios_set
%attr(555,root,root) /opt/ap/acs/bin/hwiprint
%attr(555,root,root) /opt/ap/acs/bin/hwmscbls
%attr(555,root,root) /opt/ap/acs/bin/hwmxls
%attr(555,root,root) /opt/ap/acs/bin/dShelfMngrLog.sh
%attr(555,root,root) /opt/ap/acs/bin/bios_set.sh
%attr(555,root,root) /opt/ap/acs/bin/hwiprint.sh
%attr(555,root,root) /opt/ap/acs/bin/hwmscbls.sh
%attr(555,root,root) /opt/ap/acs/bin/hwmxls.sh
%attr(555,root,root) /opt/ap/acs/bin/acs_apbm_clc

%changelog
* Wed Jul 07 2010 - nicola.muto (at) its.na.it
- Initial implementation
