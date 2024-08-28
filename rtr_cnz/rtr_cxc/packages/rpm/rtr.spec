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
%define rtr_cxc_path %{_cxcdir}
%define model_path /opt/com/etc/model

Requires: APOS_OSCONFBIN

Name:      %{_name} 
Summary:   Installation package for ACS RTR. 
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson LM 
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 

%description
Installation package for ACS RTR

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

cp %rtr_cxc_path/bin/acs_rtrd 		$RPM_BUILD_ROOT%ACSBINdir/acs_rtrd
cp %rtr_cxc_path/bin/rtrdef           $RPM_BUILD_ROOT%ACSBINdir/rtrdef
cp %rtr_cxc_path/bin/rtrdef.sh           $RPM_BUILD_ROOT%ACSBINdir/rtrdef.sh
cp %rtr_cxc_path/bin/rtrls           $RPM_BUILD_ROOT%ACSBINdir/rtrls
cp %rtr_cxc_path/bin/rtrls.sh           $RPM_BUILD_ROOT%ACSBINdir/rtrls.sh
cp %rtr_cxc_path/bin/rtrch           $RPM_BUILD_ROOT%ACSBINdir/rtrch
cp %rtr_cxc_path/bin/rtrch.sh           $RPM_BUILD_ROOT%ACSBINdir/rtrch.sh
cp %rtr_cxc_path/bin/rtrfe           $RPM_BUILD_ROOT%ACSBINdir/rtrfe
cp %rtr_cxc_path/bin/rtrfe.sh           $RPM_BUILD_ROOT%ACSBINdir/rtrfe.sh
cp %rtr_cxc_path/bin/rtrrm           $RPM_BUILD_ROOT%ACSBINdir/rtrrm
cp %rtr_cxc_path/bin/rtrrm.sh           $RPM_BUILD_ROOT%ACSBINdir/rtrrm.sh
cp %rtr_cxc_path/bin/lib_int/libacs_rtr.so.2.1.0 $RPM_BUILD_ROOT%ACSLIB64dir/libacs_rtr.so.2.1.0
cp %rtr_cxc_path/bin/acs_rtr_clc 		$RPM_BUILD_ROOT%ACSBINdir/acs_rtr_clc

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_rtrd
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrdef
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrdef.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrls
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrls.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrch
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrch.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrfe
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrfe.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrrm
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rtrrm.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_rtr_clc
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_rtr.so.2.1.0 $RPM_BUILD_ROOT%{_lib64dir}/libacs_rtr.so.2
ln -sf $RPM_BUILD_ROOT%{_lib64dir}/libacs_rtr.so.2 $RPM_BUILD_ROOT%{_lib64dir}/libacs_rtr.so	
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_rtrd $RPM_BUILD_ROOT%{_bindir}/acs_rtrd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/rtrdef.sh $RPM_BUILD_ROOT%{_bindir}/rtrdef
ln -sf $RPM_BUILD_ROOT%ACSBINdir/rtrls.sh $RPM_BUILD_ROOT%{_bindir}/rtrls
ln -sf $RPM_BUILD_ROOT%ACSBINdir/rtrch.sh $RPM_BUILD_ROOT%{_bindir}/rtrch
ln -sf $RPM_BUILD_ROOT%ACSBINdir/rtrfe.sh $RPM_BUILD_ROOT%{_bindir}/rtrfe
ln -sf $RPM_BUILD_ROOT%ACSBINdir/rtrrm.sh $RPM_BUILD_ROOT%{_bindir}/rtrrm
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_rtr_clc $RPM_BUILD_ROOT%{_bindir}/acs_rtr_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_bindir}/acs_rtrd
	rm -f %{_bindir}/rtrdef
	rm -f %{_bindir}/rtrls
	rm -f %{_bindir}/rtrch
	rm -f %{_bindir}/rtrfe
	rm -f %{_bindir}/rtrrm
	rm -f %{_lib64dir}/libacs_rtr.so.2
        rm -f %{_lib64dir}/libacs_rtr.so
	rm -f %{_lib64dir}/libacs_rtr.so.2
        rm -f %{_lib64dir}/libacs_rtr.so
	rm -f %{_bindir}/acs_rtr_clc
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/acs_rtrd
	rm -f %ACSBINdir/rtrdef
	rm -f %ACSBINdir/rtrdef.sh
	rm -f %ACSBINdir/rtrls
	rm -f %ACSBINdir/rtrls.sh
	rm -f %ACSBINdir/rtrch
	rm -f %ACSBINdir/rtrch.sh
	rm -f %ACSBINdir/rtrfe
	rm -f %ACSBINdir/rtrfe.sh
	rm -f %ACSBINdir/rtrrm
	rm -f %ACSBINdir/rtrrm.sh
	rm -f %ACSLIB64dir/libacs_rtr.so.2.1.0
	rm -f %ACSBINdir/acs_rtr_clc
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/acs_rtrd
%ACSBINdir/rtrdef
%ACSBINdir/rtrdef.sh
%ACSBINdir/rtrls
%ACSBINdir/rtrls.sh
%ACSBINdir/rtrch
%ACSBINdir/rtrch.sh
%ACSBINdir/rtrfe
%ACSBINdir/rtrfe.sh
%ACSBINdir/rtrrm
%ACSBINdir/rtrrm.sh
%ACSLIB64dir/libacs_rtr.so.2.1.0
%ACSBINdir/acs_rtr_clc

%changelog
* Fri Oct 25 2013 - bavana.harika (at) tcs.com
- Changing libacs_rtr.so.2.0.2 version
* Thu Aug 22 2013 - bavana.harika (at) tcs.com
- Adding commands
* Mon Jul 29 2013 - bavana.harika (at) tcs.com
- Changing libacs_rtr.so.2.0.1 version
* Fri Jun 21 2013 - bavana.harika (at) tcs.com
- Changing libacs_rtr.so.2.0.0 version
* Wed Jun 13 2013 - bavana.harika (at) tcs.com
- Changing libacs_rtr.so.1.2.0 version
* Tue May 13 2013 - bavana.harika (at) tcs.com
- Changing libacs_rtr.so.1.1.1 version
* Mon Jan 7 2013 - tsameer.chandra (at) tcs.com
-Initial Implementation

