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
%define aca_cxc_path %{_cxcdir}
%define model_path /opt/com/etc/model

Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for ACS ACA. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for ACS ACA

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

cp %aca_cxc_path/bin/acs_acad 		$RPM_BUILD_ROOT%ACSBINdir/acs_acad
cp %aca_cxc_path/bin/msdls 		$RPM_BUILD_ROOT%ACSBINdir/msdls
cp %aca_cxc_path/bin/msdls.sh $RPM_BUILD_ROOT%ACSBINdir/msdls.sh
cp %aca_cxc_path/bin/lib_ext/libacs_aca.so.3.5.2 $RPM_BUILD_ROOT%ACSLIB64dir/libacs_aca.so.3.5.2
cp %aca_cxc_path/bin/acs_aca_clc 		$RPM_BUILD_ROOT%ACSBINdir/acs_aca_clc
cp %aca_cxc_path/conf/c_AxeDataRecord_imm_classes.xml $RPM_BUILD_ROOT%ACSCONFdir/c_AxeDataRecord_imm_classes.xml
cp %aca_cxc_path/conf/o_AxeDataRecordInstances_imm_objects.xml 	$RPM_BUILD_ROOT%ACSCONFdir/o_AxeDataRecordInstances_imm_objects.xml
cp %aca_cxc_path/conf/ha_acs_aca_objects.xml 	$RPM_BUILD_ROOT%ACSCONFdir/ha_acs_aca_objects.xml
cp %aca_cxc_path/conf/AxeDataRecord_mp.xml 	$RPM_BUILD_ROOT%model_path/AxeDataRecord_mp.xml


%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_acad
chmod +x $RPM_BUILD_ROOT%ACSBINdir/msdls
chmod +x $RPM_BUILD_ROOT%ACSBINdir/msdls.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_aca_clc
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/c_AxeDataRecord_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/o_AxeDataRecordInstances_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_aca_objects.xml
chmod +x $RPM_BUILD_ROOT%model_path/AxeDataRecord_mp.xml
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_aca.so.3.5.2 $RPM_BUILD_ROOT%{_lib64dir}/libacs_aca.so.3
ln -sf $RPM_BUILD_ROOT%{_lib64dir}/libacs_aca.so.3 $RPM_BUILD_ROOT%{_lib64dir}/libacs_aca.so	
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_acad $RPM_BUILD_ROOT%{_bindir}/acs_acad
ln -sf $RPM_BUILD_ROOT%ACSBINdir/msdls.sh $RPM_BUILD_ROOT%{_bindir}/msdls
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_aca_clc $RPM_BUILD_ROOT%{_bindir}/acs_aca_clc



%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_bindir}/acs_acad
	rm -f %{_bindir}/msdls	
	rm -f %{_lib64dir}/libacs_aca.so.3
        rm -f %{_lib64dir}/libacs_aca.so
	rm -f %{_lib64dir}/libacs_aca.so.3
        rm -f %{_lib64dir}/libacs_aca.so
	rm -f %{_bindir}/acs_aca_clc
        rm -f %{_bindir}/aca_disk_cache.sh
	
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi



%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/acs_acad
	rm -f %ACSBINdir/msdls
	rm -f %ACSBINdir/msdls.sh
	rm -f %ACSLIB64dir/libacs_aca.so.3.5.2
	rm -f %ACSBINdir/acs_aca_clc
        rm -f %ACSBINdir/aca_disk_cache.sh
	rm -f %ACSCONFdir/c_AxeDataRecord_imm_classes.xml
	rm -f %ACSCONFdir/o_AxeDataRecordInstances_imm_objects.xml
	rm -f %ACSCONFdir/ha_acs_aca_objects.xml
	rm -f %model_path/AxeDataRecord_mp.xml
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi



%files
%defattr(-,root,root)
%ACSBINdir/acs_acad
%ACSBINdir/msdls
%ACSBINdir/msdls.sh
%ACSLIB64dir/libacs_aca.so.3.5.2
%ACSBINdir/acs_aca_clc
%ACSCONFdir/c_AxeDataRecord_imm_classes.xml
%ACSCONFdir/o_AxeDataRecordInstances_imm_objects.xml
%ACSCONFdir/ha_acs_aca_objects.xml
%model_path/AxeDataRecord_mp.xml


%changelog
* Mon Oct 28 2013 - harika.bavna (at) tcs.com
-Modified ACA Lib version
* Tue Oct 15 2013 - harika.bavna (at) tcs.com
-Modified ACA Lib version
* Thu Aug 05 2013 - harika.bavna (at) tcs.com
-Modified ACA Lib version
* Mon Jul 29 2013 - harika.bavna (at) tcs.com
-Modified ACA Lib version
* Fri Jun 21 2013 - harika.bavna (at) tcs.com
-Modified ACA Lib version
* Tue May 14 2013 - harika.bavna (at) tcs.com
-Modified ACA Lib version
* Tue Apr 02 2013 - harika.bavna (at) tcs.com
-Modified ACA Lib version
* Wed Dec 05 2012 - harika.bavna (at) tcs.com
-added clc script
* Fri Nov 16 2012 - tsameer.chandra (at) tcs.com
-Improvements
* Wed Oct 10 2012 - harika.bavana (at) tcs.com
-Initial Implementation
