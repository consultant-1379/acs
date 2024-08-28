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
Summary:   Installation package for ALH.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires: APOS_OSCONFBIN

%define alh_bin_path %{_cxcdir}/bin

#BuildRoot: %{_tmppath}/%{name}_%{version}_%{release}-build

%description
Installation package for ALH.

%pre
if [ $1 == 1 ] 
then
echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
#mkfifo /var/run/ap/alhfifo
fi
if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
rm -f /usr/bin/acs_alhd
rm -f /usr/bin/alist
rm -f /usr/bin/acease
fi

%install
echo "This is the %{_name} package %{_rel} install script"

mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/bin
mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/lib64
mkdir -p $RPM_BUILD_ROOT/cluster/etc/ap/acs/alh/conf

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
if [ ! -d $RPM_BUILD_ROOT%ACSLIB64dir ]
then
	mkdir $RPM_BUILD_ROOT%ACSLIB64dir
fi



cp %alh_bin_path/acs_alhd	$RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %alh_bin_path/acs_alh_clc $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %alh_bin_path/alist $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %alh_bin_path/acease $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %alh_bin_path/alist.sh $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %alh_bin_path/acease.sh $RPM_BUILD_ROOT/opt/ap/acs/bin/


%post
echo "This is the %{_name} package %{_rel} post-install script"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_alhd $RPM_BUILD_ROOT/usr/bin/acs_alhd
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acease.sh $RPM_BUILD_ROOT/usr/bin/acease
ln -sf $RPM_BUILD_ROOT%ACSBINdir/alist.sh $RPM_BUILD_ROOT/usr/bin/alist
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_alh_clc $RPM_BUILD_ROOT/usr/bin/acs_alh_clc


%preun
echo "This is the %{_name} package %{_rel} pre-uninstall script"


%postun
if [ $1 -eq 0 ] 
then
echo "This is the %{_name} package %{_rel} postun script during unistall phase --> start"
echo "remove links"
rm -f /usr/bin/acs_alhd
rm -f /usr/bin/acease
rm -f /usr/bin/alist
rm -f /usr/bin/acs_alh_clc

rm -f $RPM_BUILD_ROOT%ACSBINdir/acs_alhd
rm -f $RPM_BUILD_ROOT%ACSBINdir/acease
rm -f $RPM_BUILD_ROOT%ACSBINdir/alist
rm -f $RPM_BUILD_ROOT%ACSBINdir/acease.sh
rm -f $RPM_BUILD_ROOT%ACSBINdir/alist.sh
rm -f $RPM_BUILD_ROOT%ACSBINdir/acs_alh_clc

rm -rf $RPM_BUILD_ROOT/cluster/etc/ap/acs/alh/
fi


if [ $1 -eq 1 ] 
then
echo "This is the %{_name} package %{_rel} postun script during upgrade phase"
fi


%files
%defattr(-,root,root)
%attr(555,root,root) %ACSBINdir/acs_alhd

%attr(555,root,root) %ACSBINdir/acease
%attr(555,root,root) %ACSBINdir/alist
%attr(555,root,root) %ACSBINdir/acease.sh
%attr(555,root,root) %ACSBINdir/alist.sh
%attr(555,root,root) %ACSBINdir/acs_alh_clc
#/cluster/etc/ap/acs/alh/conf/Alh_imm_classes.xml
#/cluster/etc/ap/acs/alh/conf/Alh_imm_objects.xml
#/cluster/etc/ap/acs/alh/conf/ha_acs_alh_objects.xml


%changelog
* Wed Jul 07 2010 - fabio.ronca (at) ericsson.com
- Initial implementation
