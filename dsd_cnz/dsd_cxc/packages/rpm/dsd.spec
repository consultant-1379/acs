#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#

Name:      %{_name}
Summary:   Installation package for DSD.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
Requires:  APOS_OSCONFBIN
BuildRoot: %_tmppath

%define acs_dsdcxc_path %{_cxcdir}
#BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for DSD.

%pre
if [ $1 -eq 1 ]
then
echo "This is the %{_name} package %{_rel} preinstall script during installation phase"
fi

if [ $1 -eq 2 ]
then
echo "This is the %{_name} package %{_rel} preinstall script during upgrade phase --> start"
echo "removing links"
rm -f /usr/lib64/libacs_dsd.so*
rm -f /usr/bin/acs_dsdd
rm -f /usr/bin/dsdls
echo "This is the %{_name} package %{_rel} preinstall script during upgrade phase --> end"
fi

%install
echo "Installing ACS_DSD package"

mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/bin
mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/lib64

cp %acs_dsdcxc_path/bin/lib_ext/libacs_dsd.so.2.17.1 $RPM_BUILD_ROOT/opt/ap/acs/lib64/
cp %acs_dsdcxc_path/bin/acs_dsdd	$RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_dsdcxc_path/bin/dsdls $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_dsdcxc_path/bin/dsdls.sh $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %acs_dsdcxc_path/bin/acs_dsd_clc $RPM_BUILD_ROOT/opt/ap/acs/bin/

%post
echo "Finalizing ACS_DSD package installation"
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_dsd.so.2.17.1 $RPM_BUILD_ROOT/usr/lib64/libacs_dsd.so.2
ln -sf libacs_dsd.so.2 $RPM_BUILD_ROOT/usr/lib64/libacs_dsd.so
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_dsdd $RPM_BUILD_ROOT/usr/bin/acs_dsdd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/dsdls.sh $RPM_BUILD_ROOT/usr/bin/dsdls
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_dsd_clc $RPM_BUILD_ROOT/usr/bin/acs_dsd_clc

%preun
echo "Uninstalling ACS_DSD package"

%postun
if [ $1 -eq 0 ] 
then
echo "This is the %{_name} package %{_rel} postun script during unistall phase --> start"
echo "remove links"
rm -f /usr/bin/acs_dsdd
rm -f /usr/bin/dsdls
rm -f /usr/bin/acs_dsd_clc
rm -f /usr/lib64/libacs_dsd.so*

rm -f $RPM_BUILD_ROOT%ACSLIB64dir/libacs_dsd.so*
rm -f $RPM_BUILD_ROOT%ACSBINdir/acs_dsdd
rm -f $RPM_BUILD_ROOT%ACSBINdir/dsdls
rm -f $RPM_BUILD_ROOT%ACSBINdir/dsdls.sh
rm -f $RPM_BUILD_ROOT%ACSBINdir/acs_dsd_clc
rm -f /var/run/ap/acs_dsdd*

fi


if [ $1 -eq 1 ] 
then
echo "This is the %{_name} package %{_rel} postun script during upgrade phase"
fi


%files
%defattr(-,root,root)
%ACSLIB64dir/libacs_dsd.so.2.17.1
%attr(555,root,root) %ACSBINdir/acs_dsdd
%attr(555,root,root) %ACSBINdir/dsdls
%attr(555,root,root) %ACSBINdir/dsdls.sh
%attr(555,root,root) %ACSBINdir/acs_dsd_clc

%changelog
* Wed Jul 07 2010 - nicola.muto (at) its.na.it
- Initial implementation
