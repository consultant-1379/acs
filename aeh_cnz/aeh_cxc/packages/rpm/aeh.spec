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
Summary:   Installation package for AEH.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires: APOS_OSCONFBIN

%define aeh_bin_path %{_cxcdir}/bin


#BuildRoot: %{_tmppath}/%{name}_%{version}_%{release}-build

%description
Installation package for AEH.

%pre
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} preinstall script during installation phase"
fi

if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} preinstall script during upgrade phase --> start"
echo "remove link /usr/lib64/libacs_aeh.so"
rm -f /usr/lib64/libacs_aeh.so
echo "remove link /usr/lib64/libacs_aeh.so.2"
rm -f /usr/lib64/libacs_aeh.so.2
echo "remove command link /usr/bin/aehls"
rm -f /usr/bin/aehls
echo "remove command link /usr/bin/aehevls"
rm -f /usr/bin/aehevls
echo "This is the %{_name} package %{_rel} preinstall script during upgrade phase --> end"
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


cp %aeh_bin_path/lib_ext/libacs_aeh.so.2.7.1	$RPM_BUILD_ROOT%ACSLIB64dir/libacs_aeh.so.2.7.1
cp %aeh_bin_path/aehls		$RPM_BUILD_ROOT%ACSBINdir/aehls
cp %aeh_bin_path/aehevls	$RPM_BUILD_ROOT%ACSBINdir/aehevls
cp %aeh_bin_path/aehls.sh		$RPM_BUILD_ROOT%ACSBINdir/aehls.sh
cp %aeh_bin_path/aehevls.sh	$RPM_BUILD_ROOT%ACSBINdir/aehevls.sh


# comment will be delete when the AEH commands will be ready 
# cp %aeh_bin_path/aehevls		$RPM_BUILD_ROOT%ACSBINdir/aehevls


%post
if [ $1 == 1 ] 
then
echo "This is the %{_name} package %{_rel} postinstall script during installation phase --> start"
#echo "create fifo /var/run/ap/aehfifo"
#mkfifo /var/run/ap/aehfifo
echo "create link libacs_aeh.so.2-->libacs_aeh.so.2.7.1"
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_aeh.so.2.7.1 /usr/lib64/libacs_aeh.so.2
echo "create link libacs_aeh.so.2-->libacs_aeh.so"
ln -sf /usr/lib64/libacs_aeh.so.2 /usr/lib64/libacs_aeh.so
echo "create link aehls-->/usr/bin/aehls.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/aehls.sh /usr/bin/aehls
echo "create link aehevls-->/usr/bin/aehevls.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/aehevls.sh /usr/bin/aehevls
echo "This is the %{_name} package %{_rel} postinstall script during installation phase --> end"
fi

if [ $1 == 2 ] 
then
echo "This is the %{_name} package %{_rel} postinstall script during upgrade phase --> start"
echo "create link libacs_aeh.so.2-->libacs_aeh.so.2.7.1"
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_aeh.so.2.7.1 /usr/lib64/libacs_aeh.so.2
echo "create link libacs_aeh.so.2-->libacs_aeh.so"
ln -sf /usr/lib64/libacs_aeh.so.2 /usr/lib64/libacs_aeh.so
echo "create link aehls-->/usr/bin/aehls.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/aehls.sh /usr/bin/aehls
echo "create link aehevls-->/usr/bin/aehevls.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/aehevls.sh /usr/bin/aehevls
echo "This is the %{_name} package %{_rel} postinstall script during upgrade phase --> end"
fi


# comment will be delete when the AEH commands will be ready 
# ln -sf $RPM_BUILD_ROOT%ACSBINdir/aehevls /usr/bin/aehevls

%preun
if [ $1 == 0 ] 
then
echo "This is the %{_name} package %{_rel} preun script during unistall phase"
fi

if [ $1 == 1 ] 
then
echo "This is the %{_name} package %{_rel} preun script during upgrade phase"
fi

%postun
if [ $1 == 0 ] 
then
echo "This is the %{_name} package %{_rel} postun script during unistall phase --> start"
echo "remove link /usr/lib64/libacs_aeh.so"
rm -f /usr/lib64/libacs_aeh.so
echo "remove link /usr/lib64/libacs_aeh.so.2"
rm -f /usr/lib64/libacs_aeh.so.2
echo "remove library libacs_aeh.so.2.7.1"
rm -f %ACSLIB64dir/libacs_aeh.so.2.7.1
echo "remove command link /usr/bin/aehls"
rm -f /usr/bin/aehls
echo "remove command link /usr/bin/aehevls"
rm -f /usr/bin/aehevls
#echo "remove fifo /var/run/ap/aehfifo"
#unlink /var/run/ap/aehfifo
echo "This is the %{_name} package %{_rel} postun script during unistall phase --> end"
fi


if [ $1 == 1 ] 
then
echo "This is the %{_name} package %{_rel} postun script during upgrade phase"
fi

# comment will be delete when the AEH commands will be ready 
# rm -f %ACSBINdir/aehevls

%files
%defattr(-,root,root)
%ACSLIB64dir/libacs_aeh.so.2.7.1
%ACSBINdir/aehls
%ACSBINdir/aehevls
%ACSBINdir/aehls.sh
%ACSBINdir/aehevls.sh


# comment will be delete when the AEH commands will be ready 
# %ACSBINdir/acs_aehevls

%changelog
* Wed Jul 07 2010 - fabio.ronca (at) its.na.it
- Initial implementation
