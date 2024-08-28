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
%define ssu_cxc_path %{_cxcdir}
Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for SSU. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for SSU Monitor Service Function

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

if [ ! -d $RPM_BUILD_ROOT%ACSETCdir ]
then
        mkdir $RPM_BUILD_ROOT%ACSETCdir
fi

if [ ! -d $RPM_BUILD_ROOT%ACSCONFdir ]
then
        mkdir $RPM_BUILD_ROOT%ACSCONFdir
fi

if [ ! -d $RPM_BUILD_ROOT%ACSLIB64dir ]
then
        mkdir $RPM_BUILD_ROOT%ACSLIB64dir
fi

if [ ! -d $RPM_BUILD_ROOT/var/log ]
then
        mkdir -p $RPM_BUILD_ROOT/var/log
fi

cp %ssu_cxc_path/bin/acs_ssumonitord $RPM_BUILD_ROOT%ACSBINdir/acs_ssumonitord
cp %ssu_cxc_path/bin/acs_ssu_procls.sh $RPM_BUILD_ROOT%ACSBINdir/acs_ssu_procls.sh
cp %ssu_cxc_path/bin/acs_ssu_pstat.sh $RPM_BUILD_ROOT%ACSBINdir/acs_ssu_pstat.sh
cp %ssu_cxc_path/conf/SSU_Quotas $RPM_BUILD_ROOT%ACSETCdir/SSU_Quotas
cp %ssu_cxc_path/conf/SSU_FileMQuotas $RPM_BUILD_ROOT%ACSETCdir/SSU_FileMQuotas
cp %ssu_cxc_path/bin/acs_ssu_ssuservice_clc $RPM_BUILD_ROOT%ACSBINdir/acs_ssu_ssuservice_clc
cp %ssu_cxc_path/conf/APZIM_SystemSupervisionM_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_SystemSupervisionM_imm_classes.xml
cp %ssu_cxc_path/conf/APZIM_SystemSupervisionM_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_SystemSupervisionM_imm_objects.xml
cp %ssu_cxc_path/conf/ha_acs_ssu_ssuservice_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_ssu_ssuservice_objects.xml
#cp %ssu_cxc_path/bin/ssucfg $RPM_BUILD_ROOT%ACSBINdir/ssucfg
#cp %ssu_cxc_path/bin/ssuls $RPM_BUILD_ROOT%ACSBINdir/ssuls

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_ssumonitord
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_ssu_procls.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_ssu_pstat.sh
chmod +x $RPM_BUILD_ROOT%ACSETCdir/SSU_Quotas
chmod +x $RPM_BUILD_ROOT%ACSETCdir/SSU_FileMQuotas
#chmod +x $RPM_BUILD_ROOT%ACSBINdir/ssucfg
#chmod +x $RPM_BUILD_ROOT%ACSBINdir/ssuls
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_ssu_ssuservice_clc
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_SystemSupervisionM_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/APZIM_SystemSupervisionM_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_ssu_ssuservice_objects.xml
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_ssumonitord $RPM_BUILD_ROOT%{_bindir}/acs_ssumonitord
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/ssucfg $RPM_BUILD_ROOT%{_bindir}/ssucfg
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/ssuls  $RPM_BUILD_ROOT%{_bindir}/ssuls
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_ssu_ssuservice_clc $RPM_BUILD_ROOT%{_bindir}/acs_ssu_ssuservice_clc
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_ssu_procls.sh $RPM_BUILD_ROOT%{_bindir}/acs_ssu_procls.sh
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_ssu_pstat.sh $RPM_BUILD_ROOT%{_bindir}/acs_ssu_pstat.sh

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

		rm -f %{_bindir}/acs_ssumonitord
		#rm -f %{_bindir}/ssuls
		#rm -f %{_bindir}/ssucfg
		rm -f %{_bindir}/acs_ssu_ssuservice_clc
		rm -f %{_bindir}/acs_ssu_procls.sh
		rm -f %{_bindir}/acs_ssu_pstat.sh
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/acs_ssumonitord
	rm -f %ACSBINdir/acs_ssu_procls.sh
	rm -f %ACSBINdir/acs_ssu_pstat.sh
	rm -f %ACSETCdir/SSU_Quotas
	rm -f %ACSETCdir/SSU_FileMQuotas
	#rm -f %ACSBINdir/ssucfg
	#rm -f %ACSBINdir/ssuls
	rm -f %ACSBINdir/acs_ssu_ssuservice_clc
	rm -f %ACSCONFdir/APZIM_SystemSupervisionM_imm_classes.xml
	rm -f %ACSCONFdir/APZIM_SystemSupervisionM_imm_objects.xml
	rm -f %ACSCONFdir/ha_acs_ssu_ssuservice_objects.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/acs_ssumonitord
%ACSBINdir/acs_ssu_procls.sh
%ACSBINdir/acs_ssu_pstat.sh
%ACSETCdir/SSU_Quotas
%ACSETCdir/SSU_FileMQuotas
#%ACSBINdir/ssucfg
#%ACSBINdir/ssuls
%ACSBINdir/acs_ssu_ssuservice_clc
%ACSCONFdir/APZIM_SystemSupervisionM_imm_classes.xml
%ACSCONFdir/APZIM_SystemSupervisionM_imm_objects.xml
%ACSCONFdir/ha_acs_ssu_ssuservice_objects.xml

%changelog
* Mon May 09 2011 - n.naresh (at)tcs.com

* Wed Mar 09 2011 - prabhu.tsk (at) tcs.com
-Initial Implementation
-Renamed classes and objects xmls
-Removing ssu commands from RPM

