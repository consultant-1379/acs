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
%define apjtp_cxc_path %{_cxcdir}
Requires: APOS_OSCONFBIN

Name:      %{_name}
Summary:   Installation package for ACS APJTP library.
Version:   %{_prNr}
Release:   %{_rel} 
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath

%description
Installation package for APJTP API.

%pre
if [ $1 == 1 ]
then
	echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
fi
if [ $1 == 2 ]
then
	echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
	rm -f %ACSLIB64dir/libacs_jtp.so.2.6.0 
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
if [ ! -d $RPM_BUILD_ROOT%ACSLIB64dir ]
then
        mkdir $RPM_BUILD_ROOT%ACSLIB64dir
fi

cp %apjtp_cxc_path/bin/lib_ext/libacs_jtp.so.2.6.0  $RPM_BUILD_ROOT%ACSLIB64dir/libacs_jtp.so.2.6.0

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSLIB64dir/libacs_jtp.so.2.6.0
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_jtp.so.2.6.0  $RPM_BUILD_ROOT%{_lib64dir}/libacs_jtp.so.2
ln -sf $RPM_BUILD_ROOT%{_lib64dir}/libacs_jtp.so.2  $RPM_BUILD_ROOT%{_lib64dir}/libacs_jtp.so

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"
	rm -f %{_lib64dir}/libacs_jtp.so.2
	rm -f %{_lib64dir}/libacs_jtp.so
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"
	rm -f %ACSLIB64dir/libacs_jtp.so.2.6.0
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSLIB64dir/libacs_jtp.so.2.6.0

%changelog
* Fri Apr 12 2013 - Ramakrishna Maturu (at) tcs.com
* Thu Jul 14 2011 - Harika Bavana (at) tcs.com
* Tue Apr 19 2011 - tanu aggarwal () ericsson.com
* Wed Mar 09 2011 - tanu aggarwal () ericsson.com
- Updated spec file according to PA5 of AP Directory Structure.
- Added macros for Name, Version and Release
- Modified spec file according to new DR

