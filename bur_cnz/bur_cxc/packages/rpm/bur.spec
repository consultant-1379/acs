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
Summary:   Installation package for BUR.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %{_tmppath}
Requires: APOS_OSCONFBIN

#%define alog_bin_path /vobs/cm4ap/ntacs/alog_cnz/alog_cxc/bin
#%define bur_bin_path /vobs/cm4ap/ntacs/bur_cnz/bur_cxc/bin
#%define bur_bin_path /home/egimarr/workspace1/bur_cnz/bur_cxc/bin
%define bur_bin_path %{_cxcdir}/bin
#BuildRoot: %{_tmppath}/%{_name}_%{_prNr}_%{_rel}-build

%description
Installation package for BUR.
%pre
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} pre-install script during installation phase"

#echo "make folder /data/acs/data/"
#if [ -d /data/acs/data/ ]
#then
#	if [ ! -d /data/acs/data/bur/ ]
#	then 
#		mkdir -p /data/acs/data/bur/
#	fi
#fi

#echo "make folder /var/log/acs/"
#if [ ! -d $RPM_BUILD_ROOT/var/log/acs/alog/ ]
#then 
#	mkdir -p $RPM_BUILD_ROOT/var/log/acs/alog/
#fi

fi

if [ $1 == 2 ]
then
echo " This is the %{_name} package %{_rel} pre-install script during upgrade phase"

#echo "remove link /usr/lib64/libacs_alog.so"
#rm -f /usr/lib64/libacs_alog.so
#rkspace

#echo "remove link /usr/lib64/libacs_alog.so.1"
#rm -f /usr/lib64/libacs_alog.so.1
#echo "This is the %{_name} package %{_rel} preinstall script during upgrade phase --> end"
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

#if [ ! -d $RPM_BUILD_ROOT/cluster/etc/ap/acs/bur/conf/ ]
#then 
#	mkdir -p $RPM_BUILD_ROOT/cluster/etc/ap/acs/bur/conf/
#fi

cp %bur_bin_path/burbackup $RPM_BUILD_ROOT%ACSBINdir/burbackup
cp %bur_bin_path/burbackup.sh $RPM_BUILD_ROOT%ACSBINdir/burbackup.sh
cp %bur_bin_path/burrestore $RPM_BUILD_ROOT%ACSBINdir/burrestore
cp %bur_bin_path/burrestore.sh $RPM_BUILD_ROOT%ACSBINdir/burrestore.sh
#
#cp /vobs/cm4ap/ntacs/bur_cnz/bur_cxc/conf/ha_acs_alog_objects.xml $RPM_BUILD_ROOT/cluster/etc/ap/acs/bur/conf/
#cp /vobs/cm4ap/ntacs/bur_cnz/bur_cxc/conf/Backup_Restore_imm_classes.xml $RPM_BUILD_ROOT/cluster/etc/ap/acs/alog/conf/
#cp /vobs/cm4ap/ntacs/bur_cnz/bur_cxc/conf/Backup_Restore_imm_objects.xml $RPM_BUILD_ROOT/cluster/etc/ap/acs/alog/conf/
#cp /vobs/cm4ap/ntacs/bur_cnz/bur_cxc/conf/Backup_Restore_mp.xml $RPM_BUILD_ROOT/cluster/etc/ap/acs/alog/conf/



%post
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} post-install script during installation phase --> start"
#echo "create link libacs_alog.so.1-->libacs_alog.so.1.2.3"
#ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_alog.so.1.2.3 $RPM_BUILD_ROOT/usr/lib64/libacs_alog.so.1
#echo "create link libacs_alog.so --> libacs_alog.so.1"
#ln -sf /usr/lib64/libacs_alog.so.1 $RPM_BUILD_ROOT/usr/lib64/libacs_alog.so

#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_alogmaind $RPM_BUILD_ROOT/usr/bin/acs_alogmaind
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_alog_clc $RPM_BUILD_ROOT/usr/bin/acs_alog_clc
#echo "create link alogfind-->/usr/bin/alogfind"
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/brs_demo /usr/bin/brs_demo
ln -sf $RPM_BUILD_ROOT%ACSBINdir/burbackup.sh /usr/bin/burbackup
ln -sf $RPM_BUILD_ROOT%ACSBINdir/burrestore.sh /usr/bin/burrestore
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/brmfake /usr/bin/brmfake
echo "This is the %{_name} package %{_rel} post-install script during installation phase --> end"
fi


if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} post-install script during upgrade phase --> start"
#echo "create link libacs_alog.so.1-->libacs_alog.so.1.2.3"
#ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_alog.so.1.2.3 $RPM_BUILD_ROOT/usr/lib64/libacs_alog.so.1
#echo "create link libacs_alog.so --> libacs_alog.so.1"
#ln -sf /usr/lib64/libacs_alog.so.1 $RPM_BUILD_ROOT/usr/lib64/libacs_alog.so
#echo "create link alogfind-->/usr/bin/alogfind"
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/brs_demo /usr/bin/brs_demo
ln -sf $RPM_BUILD_ROOT%ACSBINdir/burbackup.sh /usr/bin/burbackup
ln -sf $RPM_BUILD_ROOT%ACSBINdir/burrestore.sh /usr/bin/burrestore
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/burbackup /usr/bin/brmfake
echo "This is the %{_name} package %{_rel} post-install script during upgrade phase --> end"
fi



%preun
if [ $1 == 0 ]
then
echo "This is the %{_name} package %{_rel} pre-uninstall script during unistall phase"
fi

if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase --> start"
#echo "remove link /usr/lib64/libacs_alog.so"
#rm -f /usr/lib64/libacs_alog.so
rm -f /usr/bin/burbackup
rm -f /usr/bin/burrestore
#rm -f /usr/bin/brmfake
#rm -rf $RPM_BUILD_ROOT/cluster/etc/ap/acs/alog/
echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase --> end"
fi

if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} post-uninstall script upgrade phase"
fi


%files
%defattr(-,root,root)
#%ACSLIB64dir/libacs_alog.so.1.2.3
%ACSBINdir/burbackup
%ACSBINdir/burbackup.sh
%ACSBINdir/burrestore
%ACSBINdir/burrestore.sh
#/cluster/etc/ap/acs/bur/conf/ha_acs_alog_objects.xml
#/cluster/etc/ap/acs/bur/conf/Backup_Restore_imm_classes.xml
#/cluster/etc/ap/acs/bur/conf/Backup_Restore_imm_objects.xml
#/cluster/etc/ap/acs/bur/conf/Backup_Restore_mp.xml

%changelog
* Fri Feb 14 2014 - venkatangaeshg.jami (at) tcs.com
- Modified the messages in the section pre, post, preun and postun
* Wed Jun 08 2011 - giovanni.marrese,gianluigi.crispino (at) ericsson.com 
- Initial implementation
* Mon Feb 27 2011 - giovanni.marrese (at) ericsson.com
- Deleted /data/acs/data/bur folder in Pre_build 
