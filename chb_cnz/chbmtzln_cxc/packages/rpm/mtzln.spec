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
Summary:   Installation package for CHB MTZLN. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for CHB MTZLN.

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

cp %chb_cxc_path/bin/acs_chbmtzlnd 		$RPM_BUILD_ROOT%ACSBINdir/acs_chbmtzlnd
cp %chb_cxc_path/bin/mtzln 			$RPM_BUILD_ROOT%ACSBINdir/mtzln
cp %chb_cxc_path/bin/mtzln.sh 			$RPM_BUILD_ROOT%ACSBINdir/mtzln.sh
cp %chb_cxc_path/bin/acs_chb_mtzln_clc 		$RPM_BUILD_ROOT%ACSBINdir/acs_chb_mtzln_clc
cp %chb_cxc_path/bin/acs_chbtimezone 		$RPM_BUILD_ROOT%ACSBINdir/acs_chbtimezone
cp %chb_cxc_path/bin/lib_ext/libacs_chb_mtz.so.2.5.3 $RPM_BUILD_ROOT%ACSLIB64dir/libacs_chb_mtz.so.2.5.3
cp %chb_cxc_path/conf/APZIM_TimeZoneLink_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_TimeZoneLink_imm_classes.xml
cp %chb_cxc_path/conf/APZIM_TimeZoneLink_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_TimeZoneLink_imm_objects.xml
cp %chb_cxc_path/conf/ha_acs_chb_mtzln_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_chb_mtzln_objects.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_chbmtzlnd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_chbtimezone
chmod +x $RPM_BUILD_ROOT%ACSBINdir/mtzln
chmod +x $RPM_BUILD_ROOT%ACSBINdir/mtzln.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_chb_mtzln_clc
chmod +x $RPM_BUILD_ROOT%ACSLIB64dir/libacs_chb_mtz.so.2.5.3
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_TimeZoneLink_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_TimeZoneLink_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_chb_mtzln_objects.xml
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_chb_mtz.so.2.5.3 $RPM_BUILD_ROOT%{_lib64dir}/libacs_chb_mtz.so.2
ln -sf $RPM_BUILD_ROOT%{_lib64dir}/libacs_chb_mtz.so.2 $RPM_BUILD_ROOT%{_lib64dir}/libacs_chb_mtz.so
ln -sf $RPM_BUILD_ROOT%ACSBINdir/mtzln.sh $RPM_BUILD_ROOT%{_bindir}/mtzln
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_chbmtzlnd $RPM_BUILD_ROOT%{_bindir}/acs_chbmtzlnd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_chb_mtzln_clc $RPM_BUILD_ROOT%{_bindir}/acs_chb_mtzln_clc
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_chbtimezone $RPM_BUILD_ROOT%{_bindir}/acs_chbtimezone

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_lib64dir}/libacs_chb_mtz.so.2
	rm -f %{_lib64dir}/libacs_chb_mtz.so
	rm -f %{_bindir}/mtzln
	rm -f %{_bindir}/acs_chbmtzlnd
	rm -f %{_bindir}/acs_chb_mtzln_clc
	rm -f %{_bindir}/acs_chbtimezone
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSLIB64dir/libacs_chb_mtz.so.2.5.3
	rm -f %ACSBINdir/acs_chbmtzlnd
	rm -f %ACSBINdir/acs_chbtimezone
	rm -f %ACSBINdir/mtzln
	rm -f %ACSBINdir/mtzln.sh
	rm -f %ACSBINdir/acs_chb_mtzln_clc
	rm -f %ACSCONFdir/APZIM_TimeZoneLink_imm_classes.xml
	rm -f %ACSCONFdir/APZIM_TimeZoneLink_imm_objects.xml
	rm -f %ACSCONFdir/ha_acs_chb_mtzln_objects.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/acs_chbmtzlnd
%ACSBINdir/mtzln
%ACSBINdir/mtzln.sh
%ACSBINdir/acs_chb_mtzln_clc
%ACSBINdir/acs_chbtimezone
%ACSLIB64dir/libacs_chb_mtz.so.2.5.3
%ACSCONFdir/APZIM_TimeZoneLink_imm_classes.xml
%ACSCONFdir/APZIM_TimeZoneLink_imm_objects.xml
%ACSCONFdir/ha_acs_chb_mtzln_objects.xml

%changelog
* Thu Oct 04 2012 - umesh3.k (at) tcs.com
-New script added for invoking commands from comcli
* Fri Mar 02 2012 - bavan.harika (at) tcs.com
-Implementation of new model
* Wed Nov 30 2011 - ramakrishna.matutu (at) tcs.com
-Initial Implementation

