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
%define _topdir /var/tmp/%(echo "$USER")/rpms
%define _tmppath %_topdir/tmp
%define emf_cxc_path %{_cxcdir}
Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for EMF 
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     ACS
BuildRoot: %_tmppath

%description
Installation package for EMF Monitor Service Function

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

cp %emf_cxc_path/bin/acs_emfserverd $RPM_BUILD_ROOT%ACSBINdir/acs_emfserverd
cp %emf_cxc_path/bin/emfcopy $RPM_BUILD_ROOT%ACSBINdir/emfcopy
cp %emf_cxc_path/bin/emfinfo $RPM_BUILD_ROOT%ACSBINdir/emfinfo
cp %emf_cxc_path/bin/emfcopy.sh $RPM_BUILD_ROOT%ACSBINdir/emfcopy.sh
cp %emf_cxc_path/bin/emfinfo.sh $RPM_BUILD_ROOT%ACSBINdir/emfinfo.sh
cp %emf_cxc_path/bin/acs_emf_activeshare.sh $RPM_BUILD_ROOT%ACSBINdir/acs_emf_activeshare.sh
cp %emf_cxc_path/bin/acs_emf_removeshare.sh $RPM_BUILD_ROOT%ACSBINdir/acs_emf_removeshare.sh
cp %emf_cxc_path/bin/acs_emf_passiveshare.sh $RPM_BUILD_ROOT%ACSBINdir/acs_emf_passiveshare.sh
cp %emf_cxc_path/bin/acs_emf_remove_passivedvd.sh $RPM_BUILD_ROOT%ACSBINdir/acs_emf_remove_passivedvd.sh
cp %emf_cxc_path/bin/acs_emf_nfsOps.sh $RPM_BUILD_ROOT%ACSBINdir/acs_emf_nfsOps.sh
cp %emf_cxc_path/bin/acs_emf_generalOps.sh $RPM_BUILD_ROOT%ACSBINdir/acs_emf_generalOps.sh
cp %emf_cxc_path/conf/APZIM_ExternalMediaFunctionM_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_ExternalMediaFunctionM_imm_classes.xml
cp %emf_cxc_path/conf/APZIM_ExternalMediaFunctionM_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_ExternalMediaFunctionM_imm_objects.xml
cp %emf_cxc_path/bin/acs_emf_emfservice_clc $RPM_BUILD_ROOT%ACSBINdir/acs_emf_emfservice_clc
cp %emf_cxc_path/conf/ha_acs_emf_emfservice_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_emf_emfservice_objects.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_emfserverd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/emfcopy
chmod +x $RPM_BUILD_ROOT%ACSBINdir/emfinfo
chmod +x $RPM_BUILD_ROOT%ACSBINdir/emfcopy.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/emfinfo.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_emf_activeshare.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_emf_removeshare.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_emf_passiveshare.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_emf_remove_passivedvd.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_emf_nfsOps.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_emf_generalOps.sh
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_ExternalMediaFunctionM_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_ExternalMediaFunctionM_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_emf_emfservice_clc
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_emf_emfservice_objects.xml
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_emfserverd $RPM_BUILD_ROOT%{_bindir}/acs_emfserverd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/emfcopy.sh $RPM_BUILD_ROOT%{_bindir}/emfcopy
ln -sf $RPM_BUILD_ROOT%ACSBINdir/emfinfo.sh $RPM_BUILD_ROOT%{_bindir}/emfinfo
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_emf_emfservice_clc $RPM_BUILD_ROOT%{_bindir}/acs_emf_emfservice_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

		rm -f %{_bindir}/acs_emfserverd
		rm -f %{_bindir}/emfcopy
		rm -f %{_bindir}/emfinfo
		rm -f %{_bindir}/acs_emf_emfservice_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi



%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/acs_emfserverd
	rm -f %ACSBINdir/emfcopy
	rm -f %ACSBINdir/emfinfo
	rm -f %ACSBINdir/emfcopy.sh
	rm -f %ACSBINdir/emfinfo.sh
	rm -f %ACSBINdir/acs_emf_activeshare.sh
	rm -f %ACSBINdir/acs_emf_removeshare.sh
	rm -f %ACSBINdir/acs_emf_passiveshare.sh
	rm -f %ACSBINdir/acs_emf_remove_passivedvd.sh
	rm -f %ACSBINdir/acs_emf_nfsOps.sh
	rm -f %ACSBINdir/acs_emf_generalOps.sh
	rm -f %ACSCONFdir/APZIM_ExternalMediaFunctionM_imm_classes.xml
	rm -f %ACSCONFdir/APZIM_ExternalMediaFunctionM_imm_objects.xml
	rm -f %ACSBINdir/acs_emf_emfservice_clc
	rm -f %ACSCONFdir/ha_acs_emf_emfservice_objects.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi


%files
%defattr(-,root,root)
%ACSBINdir/acs_emfserverd
%ACSBINdir/emfcopy
%ACSBINdir/emfinfo
%ACSBINdir/emfcopy.sh
%ACSBINdir/emfinfo.sh
%ACSBINdir/acs_emf_activeshare.sh
%ACSBINdir/acs_emf_removeshare.sh
%ACSBINdir/acs_emf_passiveshare.sh
%ACSBINdir/acs_emf_remove_passivedvd.sh
%ACSBINdir/acs_emf_nfsOps.sh
%ACSBINdir/acs_emf_generalOps.sh
%ACSCONFdir/APZIM_ExternalMediaFunctionM_imm_classes.xml
%ACSCONFdir/APZIM_ExternalMediaFunctionM_imm_objects.xml
%ACSBINdir/acs_emf_emfservice_clc
%ACSCONFdir/ha_acs_emf_emfservice_objects.xml

%changelog
* Tue Dec 27 2013 -  Shyam Chirania (at) tcs.com
- Added EMF Server and emfcopy in spec
- Added emfinfo in spec
- Modified spec file according to new DR.
- Added clc and xml for HA
- Added symbolic link for acs_emf_emfservice_clc to /usr/bin
- Added emfget binary
- Renamed Classes and objects xmls
- Removed emfget binary from RPM
- Added share and remove scripts.
- Added emfcopy.sh and emfinfo.sh
- Added script for sharing passive dvd through nfs - umesh3.k (at) tcs.com 
