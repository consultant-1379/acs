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
%define lm_cxc_path %{_cxcdir}
Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for LM. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for LM

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

cp %lm_cxc_path/bin/acs_lmserverd 		$RPM_BUILD_ROOT%ACSBINdir/acs_lmserverd
cp %lm_cxc_path/bin/license_install 		$RPM_BUILD_ROOT%ACSBINdir/license_install
cp %lm_cxc_path/bin/license_install.sh 		$RPM_BUILD_ROOT%ACSBINdir/license_install.sh
cp %lm_cxc_path/bin/acs_lm_server_clc 	        $RPM_BUILD_ROOT%ACSBINdir/acs_lm_server_clc
cp %lm_cxc_path/bin/show_licenses               $RPM_BUILD_ROOT%ACSBINdir/show_licenses
cp %lm_cxc_path/bin/show_licenses.sh            $RPM_BUILD_ROOT%ACSBINdir/show_licenses.sh
cp %lm_cxc_path/bin/lmtestact                   $RPM_BUILD_ROOT%ACSBINdir/lmtestact
cp %lm_cxc_path/bin/lmtestact.sh                $RPM_BUILD_ROOT%ACSBINdir/lmtestact.sh
cp %lm_cxc_path/bin/lmtestadd                   $RPM_BUILD_ROOT%ACSBINdir/lmtestadd
cp %lm_cxc_path/bin/lmtestadd.sh                $RPM_BUILD_ROOT%ACSBINdir/lmtestadd.sh
cp %lm_cxc_path/bin/lmtestls                    $RPM_BUILD_ROOT%ACSBINdir/lmtestls
cp %lm_cxc_path/bin/lmtestls.sh                 $RPM_BUILD_ROOT%ACSBINdir/lmtestls.sh
cp %lm_cxc_path/bin/lmtestrm                    $RPM_BUILD_ROOT%ACSBINdir/lmtestrm
cp %lm_cxc_path/bin/lmtestrm.sh                 $RPM_BUILD_ROOT%ACSBINdir/lmtestrm.sh
cp %lm_cxc_path/bin/lmlkmapls			$RPM_BUILD_ROOT%ACSBINdir/lmlkmapls
cp %lm_cxc_path/bin/lmlkmapls.sh                $RPM_BUILD_ROOT%ACSBINdir/lmlkmapls.sh
cp %lm_cxc_path/conf/AxeLicenseManagement_imm_classes.xml 	$RPM_BUILD_ROOT%ACSCONFdir/AxeLicenseManagement_imm_classes.xml
cp %lm_cxc_path/conf/AxeLicenseManagement_imm_objects.xml 	$RPM_BUILD_ROOT%ACSCONFdir/AxeLicenseManagement_imm_objects.xml
cp %lm_cxc_path/conf/ha_acs_lm_server_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_lm_server_objects.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_lmserverd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/license_install
chmod +x $RPM_BUILD_ROOT%ACSBINdir/license_install.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_lm_server_clc
chmod +x $RPM_BUILD_ROOT%ACSBINdir/show_licenses
chmod +x $RPM_BUILD_ROOT%ACSBINdir/show_licenses.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmtestact
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmtestact.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmtestadd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmtestadd.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmtestls
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmtestls.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmtestrm
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmtestrm.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmlkmapls
chmod +x $RPM_BUILD_ROOT%ACSBINdir/lmlkmapls.sh
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/AxeLicenseManagement_imm_classes.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/AxeLicenseManagement_imm_objects.xml
chmod +x $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_lm_server_objects.xml
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lmserverd $RPM_BUILD_ROOT%{_bindir}/acs_lmserverd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/license_install.sh $RPM_BUILD_ROOT%{_bindir}/license_install
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_lm_server_clc $RPM_BUILD_ROOT%{_bindir}/acs_lm_server_clc
ln -sf $RPM_BUILD_ROOT%ACSBINdir/show_licenses.sh $RPM_BUILD_ROOT%{_bindir}/show_licenses
ln -sf $RPM_BUILD_ROOT%ACSBINdir/lmtestact.sh $RPM_BUILD_ROOT%{_bindir}/lmtestact
ln -sf $RPM_BUILD_ROOT%ACSBINdir/lmtestadd.sh $RPM_BUILD_ROOT%{_bindir}/lmtestadd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/lmtestls.sh $RPM_BUILD_ROOT%{_bindir}/lmtestls
ln -sf $RPM_BUILD_ROOT%ACSBINdir/lmtestrm.sh $RPM_BUILD_ROOT%{_bindir}/lmtestrm
ln -sf $RPM_BUILD_ROOT%ACSBINdir/lmlkmapls.sh $RPM_BUILD_ROOT%{_bindir}/lmlkmapls

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_bindir}/acs_lmserverd
	rm -f %{_bindir}/license_install
	rm -f %{_bindir}/acs_lm_server_clc
	rm -f %{_bindir}/show_licenses
	rm -f %{_bindir}/lmtestact
	rm -f %{_bindir}/lmtestadd
	rm -f %{_bindir}/lmtestls
	rm -f %{_bindir}/lmtestrm
	rm -f %{_bindir}/lmlkmapls
	
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/acs_lmserverd
	rm -f %ACSBINdir/license_install
	rm -f %ACSBINdir/license_install.sh
	rm -f %ACSBINdir/acs_lm_server_clc
	rm -f %ACSBINdir/show_licenses
	rm -f %ACSBINdir/show_licenses.sh
	rm -f %ACSBINdir/lmtestact
	rm -f %ACSBINdir/lmtestact.sh
        rm -f %ACSBINdir/lmtestadd
        rm -f %ACSBINdir/lmtestadd.sh
	rm -f %ACSBINdir/lmtestls
        rm -f %ACSBINdir/lmtestls.sh
	rm -f %ACSBINdir/lmtestrm
        rm -f %ACSBINdir/lmtestrm.sh
	rm -f %ACSBINdir/lmlkmapls
	rm -f %ACSBINdir/lmlkmapls.sh
	rm -f %ACSCONFdir/AxeLicenseManagement_imm_classes.xml
	rm -f %ACSCONFdir/AxeLicenseManagement_imm_objects.xml
	rm -f %ACSCONFdir/ha_acs_lm_server_objects.xml	
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/acs_lmserverd
%ACSBINdir/license_install
%ACSBINdir/license_install.sh
%ACSBINdir/acs_lm_server_clc
%ACSBINdir/show_licenses
%ACSBINdir/show_licenses.sh
%ACSBINdir/lmtestact
%ACSBINdir/lmtestact.sh
%ACSBINdir/lmtestadd
%ACSBINdir/lmtestadd.sh
%ACSBINdir/lmtestls
%ACSBINdir/lmtestls.sh
%ACSBINdir/lmtestrm
%ACSBINdir/lmtestrm.sh
%ACSBINdir/lmlkmapls
%ACSBINdir/lmlkmapls.sh
%ACSCONFdir/AxeLicenseManagement_imm_classes.xml
%ACSCONFdir/AxeLicenseManagement_imm_objects.xml
%ACSCONFdir/ha_acs_lm_server_objects.xml

%changelog
* Wed Jul  12 2012 - Shyam Chirania (at) tcs.com
-Removed lmgenlockcode
* Fri Nov 11 2011 - prabhu.tsk (at) tcs.com
-Added lmgenlockcode
* Fri Nov 4 2011 - prabhu.tsk (at) tcs.com
-Initial Implementation
 

