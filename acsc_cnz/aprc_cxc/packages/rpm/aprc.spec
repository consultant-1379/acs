#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
%define packer %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)
%define _topdir /var/tmp/%(echo "$USER")/rpms/
%define _tmppath %_topdir/tmp
%define aprc_cxc_path %{_cxcdir}
Requires: APOS_OSCONFBIN

Name:      %{_name}
Summary:   Installation package for ACS_APRC.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Application
BuildRoot: %_tmppath

%description
Installation package for ACS_APRC.

%pre
if [ $1 == 1 ]
then
	echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
fi
if [ $1 == 2 ]
then
	echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
        rm -f %ACSLIB64dir/libacs_aprc.so.1.1.1
fi

%install
echo "This is the %{_name} package %{_rel} install script"
echo "copying the files required"

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

#copy aprc library here
cp %aprc_cxc_path/bin/lib_ext/libacs_aprc.so.1.1.1 $RPM_BUILD_ROOT%ACSLIB64dir/libacs_aprc.so.1.1.1

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

echo "Creating the symbolic link.."
chmod +x $RPM_BUILD_ROOT%ACSLIB64dir/libacs_aprc.so.1.1.1
ln -sf /opt/ap/acs/lib64/libacs_aprc.so.1.1.1 /opt/ap/acs/lib64/libacs_aprc.so.1
ln -sf /opt/ap/acs/lib64/libacs_aprc.so.1 /opt/com/lib/comp/libacs_aprc.so
echo -n ".done"

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"
	echo "cleaning up ACS_APRC"

	rm -f %ACSLIB64dir/libacs_aprc.so.1.1.1
	rm -f %ACSLIB64dir/libacs_aprc.so.1
	rm -f /opt/com/lib/comp/libacs_aprc.so
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%attr(0755,root,root) %ACSLIB64dir/libacs_aprc.so.1.1.1

%changelog
* Tue Aug 13 2012 - ramakrishna.maturu (at) tcs.com
- Initial implementation


