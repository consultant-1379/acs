#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

Name:      %{_name}
Summary:   Installation package for LCT BIN.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Application
BuildRoot: %_tmppath

Requires: APOS_OSCONFBIN
%define acs_lctcxc_path %{_cxcdir}
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for LCTBIN.

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
if [ ! -d $RPM_BUILD_ROOT%rootdir ]
then
	mkdir $RPM_BUILD_ROOT%rootdir
fi
if [ ! -d $RPM_BUILD_ROOT%APdir ]
then
	mkdir $RPM_BUILD_ROOT%APdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSdir ]
then
	mkdir $RPM_BUILD_ROOT%ACSdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSBINdir ]
then
	mkdir $RPM_BUILD_ROOT%ACSBINdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSCONFdir ]
then
	mkdir $RPM_BUILD_ROOT%ACSCONFdir
fi
#if [ ! -d $RPM_BUILD_ROOT/cluster/etc/userinfo ]
#then
#	mkdir -p $RPM_BUILD_ROOT/cluster/etc/
#fi
#if [ ! -d $RPM_BUILD_ROOT/cluster/storage/system/config/acs_lct/ ]
#then
#	mkdir -p $RPM_BUILD_ROOT`cat /usr/share/pso/storage-paths/config`"/acs_lct/"
#        mkdir -p $RPM_BUILD_ROOT/cluster/storage/system/config/acs_lct/
#fi


cp %acs_lctcxc_path/bin/integrityap $RPM_BUILD_ROOT%ACSBINdir/integrityap
cp %acs_lctcxc_path/bin/acs_lct_baseline_create $RPM_BUILD_ROOT%ACSBINdir/acs_lct_baseline_create
#cp %acs_lctcxc_path/bin/hwver $RPM_BUILD_ROOT%ACSBINdir/hwver
cp %acs_lctcxc_path/bin/acs_lct_configap.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_configap.sh
cp %acs_lctcxc_path/bin/acs_lct_addtsuser.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_addtsuser.sh
cp %acs_lctcxc_path/bin/acs_lct_listtsuser.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_listtsuser.sh
cp %acs_lctcxc_path/bin/acs_lct_removetsuser.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_removetsuser.sh
cp %acs_lctcxc_path/bin/acs_lct_pwdmodtsuser.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdmodtsuser.sh
cp %acs_lctcxc_path/bin/acs_lct_pwdresettsuser.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdresettsuser.sh
cp %acs_lctcxc_path/bin/acs_lct_pwdsettsadmin.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdsettsadmin.sh
cp %acs_lctcxc_path/bin/acs_lct_pwdresetlaadmin.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdresetlaadmin.sh
cp %acs_lctcxc_path/bin/acs_lct_modtsuser.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_modtsuser.sh
cp %acs_lctcxc_path/conf/acs_lct_acldata.conf $RPM_BUILD_ROOT%ACSCONFdir/acs_lct_acldata.conf
#cp %acs_lctcxc_path/conf/acs_lct_apdir_data.conf $RPM_BUILD_ROOT%ACSCONFdir/acs_lct_apdir_data.conf
cp %acs_lctcxc_path/conf/acs_lct_apgroup_data.conf $RPM_BUILD_ROOT%ACSCONFdir/acs_lct_apgroup_data.conf
#cp %acs_lctcxc_path/conf/acs_lct_sha1verify.conf $RPM_BUILD_ROOT%ACSCONFdir/acs_lct_sha1verify.conf
#cp %acs_lctcxc_path/conf/IntegrityAPreport.log /var/log/acs/IntegrityAPreport.log
cp %acs_lctcxc_path/conf/acs_lct_integrityAP_base.conf $RPM_BUILD_ROOT%ACSCONFdir/acs_lct_integrityAP_base.conf 
cp %acs_lctcxc_path/conf/APZIM_datadiskpath_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_datadiskpath_imm_classes.xml 
cp %acs_lctcxc_path/conf/APZIM_datadiskpath_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_datadiskpath_imm_objects.xml 
cp %acs_lctcxc_path/conf/APZIM_ftpvirtualpath_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_ftpvirtualpath_imm_classes.xml 
cp %acs_lctcxc_path/conf/APZIM_ftpvirtualpath_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_ftpvirtualpath_imm_objects.xml 
cp %acs_lctcxc_path/conf/APZIM_apDirData_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_apDirData_imm_classes.xml
cp %acs_lctcxc_path/conf/APZIM_apDirData_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/APZIM_apDirData_imm_objects.xml 
#cp %acs_lctcxc_path/conf/userinfo $RPM_BUILD_ROOT/cluster/etc/userinfo
#For LCT TS user wrapper commands
cp %acs_lctcxc_path/bin/addtsuser $RPM_BUILD_ROOT%ACSBINdir/addtsuser
cp %acs_lctcxc_path/bin/listtsuser $RPM_BUILD_ROOT%ACSBINdir/listtsuser
cp %acs_lctcxc_path/bin/removetsuser $RPM_BUILD_ROOT%ACSBINdir/removetsuser
cp %acs_lctcxc_path/bin/pwdmodtsuser $RPM_BUILD_ROOT%ACSBINdir/pwdmodtsuser
cp %acs_lctcxc_path/bin/pwdresettsuser $RPM_BUILD_ROOT%ACSBINdir/pwdresettsuser
cp %acs_lctcxc_path/bin/pwdsettsadmin $RPM_BUILD_ROOT%ACSBINdir/pwdsettsadmin
cp %acs_lctcxc_path/bin/pwdresetlaadmin $RPM_BUILD_ROOT%ACSBINdir/pwdresetlaadmin
cp %acs_lctcxc_path/bin/modtsuser $RPM_BUILD_ROOT%ACSBINdir/modtsuser

# for hardening
cp %acs_lctcxc_path/bin/acs_lct_hardeningd $RPM_BUILD_ROOT%ACSBINdir/acs_lct_hardeningd
cp %acs_lctcxc_path/bin/acs_lct_hardening_clc $RPM_BUILD_ROOT%ACSBINdir/acs_lct_hardening_clc
cp %acs_lctcxc_path/bin/acs_lct_hardening.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lct_hardening.sh
cp %acs_lctcxc_path/conf/AxeApSessionManagement_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/AxeApSessionManagement_imm_classes.xml
cp %acs_lctcxc_path/conf/AxeApSessionManagement_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/AxeApSessionManagement_imm_objects.xml
cp %acs_lctcxc_path/conf/AxeApSessionManagement_mp.xml $RPM_BUILD_ROOT%ACSCONFdir/AxeApSessionManagement_mp.xml
cp %acs_lctcxc_path/conf/AxeLocalTsUsersPolicy_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/AxeLocalTsUsersPolicy_imm_classes.xml
cp %acs_lctcxc_path/conf/AxeLocalTsUsersPolicy_imm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/AxeLocalTsUsersPolicy_imm_objects.xml
cp %acs_lctcxc_path/conf/AxeLocalTsUsersPolicy_mp.xml $RPM_BUILD_ROOT%ACSCONFdir/AxeLocalTsUsersPolicy_mp.xml
cp %acs_lctcxc_path/conf/ha_acs_lct_hardeningd_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_lct_hardeningd_objects.xml
#cp %acs_lctcxc_path/bin/acs_lctHardeningStartup.sh $RPM_BUILD_ROOT%ACSBINdir/acs_lctHardeningStartup.sh

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

#Start chmod
#chmod +x   $RPM_BUILD_ROOT%ACSBINdir/hwver 
chmod ug+x $RPM_BUILD_ROOT%ACSBINdir/acs_lct_addtsuser.sh 
chmod +x   $RPM_BUILD_ROOT%ACSBINdir/acs_lct_listtsuser.sh 
chmod +x   $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdmodtsuser.sh
chmod ug+x $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdresettsuser.sh
chmod ug+x $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdsettsadmin.sh 
chmod ug+x $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdresetlaadmin.sh
chmod ug+x $RPM_BUILD_ROOT%ACSBINdir/acs_lct_removetsuser.sh 
chmod ug+x $RPM_BUILD_ROOT%ACSBINdir/acs_lct_modtsuser.sh
chmod +x   $RPM_BUILD_ROOT%ACSBINdir/acs_lct_configap.sh
chmod +x   $RPM_BUILD_ROOT%ACSBINdir/integrityap 
chmod +x   $RPM_BUILD_ROOT%ACSBINdir/acs_lct_hardening.sh 
chmod +x   $RPM_BUILD_ROOT%ACSBINdir/acs_lct_hardeningd 
#chmod +x  $RPM_BUILD_ROOT%ACSBINdir/acs_lctHardeningStartup.sh

#for LCT TS user wrapper commands
chmod +x $RPM_BUILD_ROOT%ACSBINdir/addtsuser
chmod +x   $RPM_BUILD_ROOT%ACSBINdir/listtsuser
chmod +x   $RPM_BUILD_ROOT%ACSBINdir/pwdmodtsuser
chmod +x $RPM_BUILD_ROOT%ACSBINdir/pwdresettsuser
chmod +x $RPM_BUILD_ROOT%ACSBINdir/pwdsettsadmin
chmod +x $RPM_BUILD_ROOT%ACSBINdir/pwdresetlaadmin
chmod +x $RPM_BUILD_ROOT%ACSBINdir/removetsuser
chmod +x $RPM_BUILD_ROOT%ACSBINdir/modtsuser
ln -sf $RPM_BUILD_ROOT%ACSBINdir/addtsuser /usr/bin/addtsuser
ln -sf $RPM_BUILD_ROOT%ACSBINdir/listtsuser /usr/bin/listtsuser
ln -sf $RPM_BUILD_ROOT%ACSBINdir/pwdmodtsuser /usr/bin/pwdmodtsuser
ln -sf $RPM_BUILD_ROOT%ACSBINdir/pwdresettsuser /usr/bin/pwdresettsuser
ln -sf $RPM_BUILD_ROOT%ACSBINdir/pwdsettsadmin /usr/bin/pwdsettsadmin
ln -sf $RPM_BUILD_ROOT%ACSBINdir/pwdresetlaadmin /usr/bin/pwdresetlaadmin
ln -sf $RPM_BUILD_ROOT%ACSBINdir/removetsuser /usr/bin/removetsuser
ln -sf $RPM_BUILD_ROOT%ACSBINdir/modtsuser /usr/bin/modtsuser

#ln -sf $RPM_BUILD_ROOT%ACSBINdir/hwver /usr/bin/hwver
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lct_addtsuser.sh /usr/bin/addtsuser
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lct_listtsuser.sh /usr/bin/listtsuser
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdmodtsuser.sh /usr/bin/pwdmodtsuser
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lct_pwdresettsuser.sh /usr/bin/pwdresettsuser
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lct_removetsuser.sh /usr/bin/removetsuser
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lct_modtsuser.sh /usr/bin/modtsuser
ln -sf $RPM_BUILD_ROOT%ACSBINdir/integrityap /usr/bin/integrityap
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lct_configap.sh /usr/bin/configap
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lct_hardening.sh /usr/bin/hardening
#for hardening
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lct_hardeningd /usr/bin/acs_lct_hardeningd
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lctHardeningStartup.sh /usr/bin/acs_lctHardeningStartup.sh

if [  -f /tmp/configap_a ]
then
     #  echo "Resetting the flag for configap to execute during upgrade phase by LCT service"
        rm -f /tmp/configap_a
fi

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

#rm -f /usr/bin/hwver
rm -f /usr/bin/addtsuser
rm -f /usr/bin/listtsuser
rm -f /usr/bin/pwdmodtsuser
rm -f /usr/bin/pwdresettsuser
rm -f /usr/bin/pwdsettsadmin
rm -f /usr/bin/pwdresetlaadmin
rm -f /usr/bin/removetsuser
rm -f /usr/bin/modtsuser
rm -f /usr/bin/integrityap
#rm -f /cluster/etc/userinfo
rm -f /var/log/acs/configap.log
#for hardening
rm -f /usr/bin/acs_lct_hardeningd
rm -f /usr/bin/hardening
#rm -f /usr/bin/acs_lctHardeningStartup.sh
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

rm -f %ACSBINdir/integrityap
rm -f %ACSBINdir/acs_lct_baseline_create
#rm -f %ACSBINdir/hwver
rm -f %ACSBINdir/acs_lct_configap.sh
rm -f %ACSBINdir/acs_lct_addtsuser.sh
rm -f %ACSBINdir/acs_lct_listtsuser.sh
rm -f %ACSBINdir/acs_lct_removetsuser.sh
rm -f %ACSBINdir/acs_lct_pwdmodtsuser.sh
rm -f %ACSBINdir/acs_lct_pwdresettsuser.sh
rm -f %ACSBINdir/acs_lct_pwdsettsadmin.sh
rm -f %ACSBINdir/acs_lct_pwdresetlaadmin.sh
rm -f %ACSBINdir/acs_lct_modtsuser.sh
rm -f %ACSBINdir/acs_lct_hardening.sh
rm -f %ACSCONFdir/acs_lct_acldata.conf
#rm -f %ACSCONFdir/acs_lct_apdir_data.conf
rm -f %ACSCONFdir/acs_lct_apgroup_data.conf
#rm -f %ACSCONFdir/acs_lct_sha1verify.conf
rm -f /var/log/acs/integrityapreport.log
rm -f %ACSCONFdir/acs_lct_integrityAP_base.conf
rm -f %ACSCONFdir/APZIM_datadiskpath_imm_classes.xml
rm -f %ACSCONFdir/APZIM_datadiskpath_imm_objects.xml
rm -f %ACSCONFdir/APZIM_ftpvirtualpath_imm_classes.xml
rm -f %ACSCONFdir/APZIM_ftpvirtualpath_imm_objects.xml
rm -f %ACSCONFdir/APZIM_apDirData_imm_classes.xml
rm -f %ACSCONFdir/APZIM_apDirData_imm_objects.xml

#For LCT TS user wrapper commands 
rm -f %ACSBINdir/addtsuser
rm -f %ACSBINdir/listtsuser
rm -f %ACSBINdir/removetsuser
rm -f %ACSBINdir/pwdmodtsuser
rm -f %ACSBINdir/pwdresettsuser
rm -f %ACSBINdir/pwdsettsadmin
rm -f %ACSBINdir/pwdresetlaadmin
rm -f %ACSBINdir/modtsuser

#for hardening
rm -f %ACSBINdir/acs_lct_hardeningd
rm -f %ACSCONFdir/AxeApSessionManagement_imm_classes.xml
rm -f %ACSCONFdir/AxeApSessionManagement_imm_objects.xml
rm -f %ACSCONFdir/AxeApSessionManagement_mp.xml
rm -f %ACSCONFdir/AxeLocalTsUsersPolicy_imm_classes.xml
rm -f %ACSCONFdir/AxeLocalTsUsersPolicy_imm_objects.xml
rm -f %ACSCONFdir/AxeLocalTsUsersPolicy_mp.xml
rm -f %ACSCONFdir/ha_acs_lct_hardeningd_objects.xml
rm -f %ACSBINdir/acs_lct_hardening_clc
#rm -f %ACSBINdir/acs_lctHardeningStartup.sh
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%attr(0755,root,root) %ACSBINdir/integrityap
%attr(0755,root,root) %ACSBINdir/acs_lct_baseline_create
#%attr(0755,root,root) %ACSBINdir/hwver
%attr(0755,root,root) %ACSBINdir/acs_lct_configap.sh
%attr(0750,root,root) %ACSBINdir/acs_lct_addtsuser.sh
%attr(0755,root,root) %ACSBINdir/acs_lct_listtsuser.sh
%attr(0750,root,root) %ACSBINdir/acs_lct_removetsuser.sh
%attr(0755,root,root) %ACSBINdir/acs_lct_pwdmodtsuser.sh
%attr(0750,root,root) %ACSBINdir/acs_lct_pwdresettsuser.sh
%attr(0750,root,root) %ACSBINdir/acs_lct_pwdsettsadmin.sh
%attr(0750,root,root) %ACSBINdir/acs_lct_pwdresetlaadmin.sh
%attr(0750,root,root) %ACSBINdir/acs_lct_modtsuser.sh
%attr(0755,root,root) %ACSBINdir/acs_lct_hardening.sh
%attr(0755,root,root) %ACSCONFdir/acs_lct_acldata.conf
%attr(0755,root,root) %ACSCONFdir/acs_lct_apgroup_data.conf
#%attr(0755,root,root) %ACSCONFdir/acs_lct_apdir_data.conf
#%attr(0755,root,root) %ACSCONFdir/acs_lct_sha1verify.conf
#%attr(0755,root,root) /var/log/acs/IntegrityAPreport.log
%attr(0755,root,root) %ACSCONFdir/acs_lct_integrityAP_base.conf
%attr(0755,root,root) %ACSCONFdir/APZIM_datadiskpath_imm_classes.xml
%attr(0755,root,root) %ACSCONFdir/APZIM_datadiskpath_imm_objects.xml
%attr(0755,root,root) %ACSCONFdir/APZIM_ftpvirtualpath_imm_classes.xml
%attr(0755,root,root) %ACSCONFdir/APZIM_ftpvirtualpath_imm_objects.xml
%attr(0755,root,root) %ACSCONFdir/APZIM_apDirData_imm_classes.xml
%attr(0755,root,root) %ACSCONFdir/APZIM_apDirData_imm_objects.xml
#%attr(0755,root,root) /cluster/etc/userinfo
#%attr(0755,root,root) /cluster/storage/system/config/acs_lct/
#For LCT TS user wrapper commands
%attr(0755,root,root) %ACSBINdir/addtsuser
%attr(0755,root,root) %ACSBINdir/listtsuser
%attr(0755,root,root) %ACSBINdir/removetsuser
%attr(0755,root,root) %ACSBINdir/pwdmodtsuser
%attr(0755,root,root) %ACSBINdir/pwdresettsuser
%attr(0755,root,root) %ACSBINdir/pwdsettsadmin
%attr(0755,root,root) %ACSBINdir/pwdresetlaadmin
%attr(0755,root,root) %ACSBINdir/modtsuser


#for hardening
%attr(0755,root,root) %ACSBINdir/acs_lct_hardeningd
%attr(0755,root,root) %ACSCONFdir/AxeApSessionManagement_imm_classes.xml
%attr(0755,root,root) %ACSCONFdir/AxeApSessionManagement_imm_objects.xml
%attr(0755,root,root) %ACSCONFdir/AxeApSessionManagement_mp.xml
%attr(0755,root,root) %ACSCONFdir/AxeLocalTsUsersPolicy_imm_classes.xml
%attr(0755,root,root) %ACSCONFdir/AxeLocalTsUsersPolicy_imm_objects.xml
%attr(0755,root,root) %ACSCONFdir/AxeLocalTsUsersPolicy_mp.xml
%attr(0755,root,root) %ACSCONFdir/ha_acs_lct_hardeningd_objects.xml
%attr(0755,root,root) %ACSBINdir/acs_lct_hardening_clc
#%attr(0755,root,root) %ACSBINdir/acs_lctHardeningStartup.sh


%changelog
* Fri Jan 30 2015 - roni.newatia (at) tcs.com
- commented the shared(cluster) files to avoid modification during rpm installation.
acs_lct folder creation is moved to configap -d. 
* Wed Jul 07 2011 - sonali.nanda (at) tcs.com
- Initial implementation
