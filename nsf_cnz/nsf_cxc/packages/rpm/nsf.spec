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
%define nsf_cxc_path %{_cxcdir}
Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for NSF. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for Network Surveillance Function 

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

cp %nsf_cxc_path/bin/acs_nsfserverd 		$RPM_BUILD_ROOT%ACSBINdir/acs_nsfserverd
cp %nsf_cxc_path/bin/acs_nsfbiostimerecovery 	$RPM_BUILD_ROOT%ACSBINdir/acs_nsfbiostimerecovery
#cp %nsf_cxc_path/bin/nsfstart 			$RPM_BUILD_ROOT%ACSBINdir/nsfstart
#cp %nsf_cxc_path/bin/nsfstop 			$RPM_BUILD_ROOT%ACSBINdir/nsfstop
#cp %nsf_cxc_path/bin/nsfreset 			$RPM_BUILD_ROOT%ACSBINdir/nsfreset
cp %nsf_cxc_path/bin/acs_nsf_server_clc 	$RPM_BUILD_ROOT%ACSBINdir/acs_nsf_server_clc
cp %nsf_cxc_path/bin/acs_nsf_node_state.sh 	$RPM_BUILD_ROOT%ACSBINdir/acs_nsf_node_state.sh
cp %nsf_cxc_path/conf/AxeExtNetworkSurveillance_imm_classes.xml 	$RPM_BUILD_ROOT%ACSCONFdir/AxeExtNetworkSurveillance_imm_classes.xml
cp %nsf_cxc_path/conf/AxeExtNetworkSurveillance_imm_objects.xml 	$RPM_BUILD_ROOT%ACSCONFdir/AxeExtNetworkSurveillance_imm_objects.xml
cp %nsf_cxc_path/conf/ha_acs_nsf_server_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_nsf_server_objects.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_nsfserverd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_nsfbiostimerecovery
#chmod +x $RPM_BUILD_ROOT%ACSBINdir/nsfstart
#chmod +x $RPM_BUILD_ROOT%ACSBINdir/nsfstop
#chmod +x $RPM_BUILD_ROOT%ACSBINdir/nsfreset
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_nsf_node_state.sh
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/AxeExtNetworkSurveillance_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/AxeExtNetworkSurveillance_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_nsf_server_objects.xml
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_nsfserverd $RPM_BUILD_ROOT%{_bindir}/acs_nsfserverd
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/nsfstart  $RPM_BUILD_ROOT%{_bindir}/nsfstart
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/nsfstop  $RPM_BUILD_ROOT%{_bindir}/nsfstop
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/nsfreset  $RPM_BUILD_ROOT%{_bindir}/nsfreset
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_nsf_server_clc  $RPM_BUILD_ROOT%{_bindir}/acs_nsf_server_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_bindir}/acs_nsfserverd
#	rm -f %{_bindir}/nsfstart
#	rm -f %{_bindir}/nsfstop
#	rm -f %{_bindir}/nsfreset
	rm -f %{_bindir}/acs_nsf_server_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/acs_nsfserverd
	rm -f %ACSBINdir/acs_nsfbiostimerecovery
#	rm -f %ACSBINdir/nsfstart
#	rm -f %ACSBINdir/nsfstop
#	rm -f %ACSBINdir/nsfreset
	rm -f %ACSBINdir/acs_nsf_node_state.sh
	rm -f %ACSBINdir/acs_nsf_server_clc
	rm -f %ACSCONFdir/AxeExtNetworkSurveillance_imm_classes.xml
	rm -f %ACSCONFdir/AxeExtNetworkSurveillance_imm_objects.xml
	rm -f %ACSCONFdir/ha_acs_nsf_server_objects.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/acs_nsfserverd
%ACSBINdir/acs_nsfbiostimerecovery
#%ACSBINdir/nsfstart
#%ACSBINdir/nsfstop
#%ACSBINdir/nsfreset
%ACSBINdir/acs_nsf_node_state.sh
%ACSBINdir/acs_nsf_server_clc
%ACSCONFdir/AxeExtNetworkSurveillance_imm_classes.xml
%ACSCONFdir/AxeExtNetworkSurveillance_imm_objects.xml
%ACSCONFdir/ha_acs_nsf_server_objects.xml

%changelog
* Wed Sep 28 2011 - tanu.aggarwal (at) tcs.com
- Renamed the classes and object xml
- Added NSF Command binaries By XCHVKUM on 30-11-2010
- Modified according to latest DR - prabhu.tsk (at) tcs.com 
- Commnented all nsf commands from spec file - Umesh3.k (at) tcs.com
- Renamed the name of class.xml and object.xml file - Shyam Chirania (at) tcs.com

