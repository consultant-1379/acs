#
# spec file for configuration of package alog
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#                                     giovanni.gambardella@ericsson.com
#

Name:      %{_name}
Summary:   Installation package for ALOG.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %{_tmppath}
Requires:  APOS_OSCONFBIN

%define alog_bin_path %{_cxcdir}/bin

#BuildRoot: %{_tmppath}/%{_name}_%{_prNr}_%{_rel}-build

%description
Installation package for ALOG.

%pre
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} preinstall script during installation phase"

#echo "make folder /data/acs/data/"
#if [ -d /data/acs/data/ ]
#then
#	if [ ! -d /data/acs/data/alog/log/ ]
#	then 
#		mkdir -p /data/acs/data/alog/log/
#	fi
#fi

echo "make folder /var/log/acs/"
if [ ! -d $RPM_BUILD_ROOT/var/log/acs/alog/ ]
then 
	mkdir -p $RPM_BUILD_ROOT/var/log/acs/alog/
fi

fi

if [ $1 == 2 ]
then
echo " This is the %{_name} package %{_rel} preinstall script during upgrade phase --> start"

echo "remove link /usr/lib64/libacs_alog.so"
rm -f /usr/lib64/libacs_alog.so
echo "remove link /usr/lib64/libacs_alog.so.1"
rm -f /usr/lib64/libacs_alog.so.1
echo "remove command link /usr/bin/alogfind"
rm -f /usr/bin/alogfind
echo "remove command link /usr/bin/alogset"
rm -f /usr/bin/alogset
echo "remove command link /usr/bin/alogpchg"
rm -f /usr/bin/alogpchg
echo "remove command link /usr/bin/alogpls"
rm -f /usr/bin/alogpls
echo "remove command link /usr/bin/aloglist"
rm -f /usr/bin/aloglist
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

cp %alog_bin_path/lib_ext/libacs_alog.so.1.8.4 $RPM_BUILD_ROOT%ACSLIB64dir/libacs_alog.so.1.8.4
cp %alog_bin_path/acs_alogmaind $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %alog_bin_path/acs_alog_clc $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp %alog_bin_path/alogfind $RPM_BUILD_ROOT%ACSBINdir/alogfind
cp %alog_bin_path/alogset $RPM_BUILD_ROOT%ACSBINdir/alogset
cp %alog_bin_path/alogpchg $RPM_BUILD_ROOT%ACSBINdir/alogpchg
cp %alog_bin_path/alogpls $RPM_BUILD_ROOT%ACSBINdir/alogpls
cp %alog_bin_path/aloglist $RPM_BUILD_ROOT%ACSBINdir/aloglist
cp %alog_bin_path/alogset.sh $RPM_BUILD_ROOT%ACSBINdir/alogset.sh
cp %alog_bin_path/alogfind.sh $RPM_BUILD_ROOT%ACSBINdir/alogfind.sh
cp %alog_bin_path/alogpls.sh $RPM_BUILD_ROOT%ACSBINdir/alogpls.sh
cp %alog_bin_path/alogpchg.sh $RPM_BUILD_ROOT%ACSBINdir/alogpchg.sh
cp %alog_bin_path/aloglist.sh $RPM_BUILD_ROOT%ACSBINdir/aloglist.sh

%post
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} postinstall script during installation phase --> start"
echo "create link libacs_alog.so.1-->libacs_alog.so.1.8.4"
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_alog.so.1.8.4 $RPM_BUILD_ROOT/usr/lib64/libacs_alog.so.1
echo "create link libacs_alog.so --> libacs_alog.so.1"
ln -sf /usr/lib64/libacs_alog.so.1 $RPM_BUILD_ROOT/usr/lib64/libacs_alog.so
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_alogmaind $RPM_BUILD_ROOT/usr/bin/acs_alogmaind
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_alog_clc $RPM_BUILD_ROOT/usr/bin/acs_alog_clc
echo "create link alogfind-->/usr/bin/alogfind.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/alogfind.sh /usr/bin/alogfind
echo "create link alogset-->/usr/bin/alogset.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/alogset.sh /usr/bin/alogset
echo "create link alogpchg-->/usr/bin/alogpchg.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/alogpchg.sh /usr/bin/alogpchg
echo "create link alogpls-->/usr/bin/alogpls.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/alogpls.sh /usr/bin/alogpls
echo "create link aloglist-->/usr/bin/aloglist.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/aloglist.sh /usr/bin/aloglist
echo "This is the %{_name} package %{_rel} postinstall script during installation phase --> end"
fi

if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} postinstall script during upgrade phase --> start"
echo "create link libacs_alog.so.1-->libacs_alog.so.1.8.4"
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_alog.so.1.8.4 $RPM_BUILD_ROOT/usr/lib64/libacs_alog.so.1
echo "create link libacs_alog.so --> libacs_alog.so.1"
ln -sf /usr/lib64/libacs_alog.so.1 $RPM_BUILD_ROOT/usr/lib64/libacs_alog.so
echo "create link alogfind-->/usr/bin/alogfind.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/alogfind.sh /usr/bin/alogfind
echo "create link alogset-->/usr/bin/alogset.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/alogset.sh /usr/bin/alogset
echo "create link alogpchg-->/usr/bin/alogpchg.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/alogpchg.sh /usr/bin/alogpchg
echo "create link alogpls-->/usr/bin/alogpls.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/alogpls.sh /usr/bin/alogpls
echo "create link aloglist-->/usr/bin/aloglist.sh"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/aloglist.sh /usr/bin/aloglist
echo "remove old API pipe if exist"
if [ -e "/var/run/ap/ALOG_API_pipe"]
then
rm -f /var/run/ap/ALOG_API_pipe
fi
echo "This is the %{_name} package %{_rel} postinstall script during upgrade phase --> end"
fi



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
echo "This is the %{_name} package %{_rel} postun script during uninstall phase --> start"
echo "remove link /usr/lib64/libacs_alog.so"
rm -f /usr/lib64/libacs_alog.so
echo "remove library libacs_alog.so.1.8.4"
rm -f %ACSLIB64dir/libacs_alog.so.1.8.4
echo "remove link /usr/lib64/libacs_alog.so.1"
rm -f /usr/lib64/libacs_alog.so.1
rm -f /usr/bin/acs_alogmaind
rm -f $RPM_BUILD_ROOT%ACSBINdir/acs_alogmaind
rm -f /usr/bin/acs_alog_clc
rm -f $RPM_BUILD_ROOT%ACSBINdir/acs_alog_clc
echo "remove command link /usr/bin/alogfind"
rm -f /usr/bin/alogfind
echo "remove command link /usr/bin/alogset"
rm -f /usr/bin/alogset
echo "remove command link /usr/bin/alogpchg"
rm -f /usr/bin/alogpchg
echo "remove command link /usr/bin/alogpls"
rm -f /usr/bin/alogpls
echo "remove command link /usr/bin/aloglist"
rm -f /usr/bin/aloglist
echo "This is the %{_name} package %{_rel} postun script during uninstall phase --> end"
fi

if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} postun script upgrade phase"
fi


%files
%defattr(-,root,root)
%ACSLIB64dir/libacs_alog.so.1.8.4
%attr(555,root,root) %ACSBINdir/acs_alogmaind
%attr(555,root,root) %ACSBINdir/acs_alog_clc
%attr(555,root,root) %ACSBINdir/alogfind
%attr(555,root,root) %ACSBINdir/alogset
%attr(555,root,root) %ACSBINdir/alogpchg
%attr(555,root,root) %ACSBINdir/alogpls
%attr(555,root,root) %ACSBINdir/aloglist
%attr(555,root,root) %ACSBINdir/aloglist.sh
%attr(555,root,root) %ACSBINdir/alogset.sh
%attr(555,root,root) %ACSBINdir/alogpls.sh
%attr(555,root,root) %ACSBINdir/alogpchg.sh
%attr(555,root,root) %ACSBINdir/alogfind.sh

%changelog
* Thu Jan 21 2021 - t.sravanthi@tcs.com
* Wed Jun 08 2011 - gennaro.colantuono (at) its.na.it 
- Initial implementation
