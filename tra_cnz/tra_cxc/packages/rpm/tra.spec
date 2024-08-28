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
Summary:   Installation package for TRA.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires:  APOS_OSCONFBIN

%define traapi_caa_path %{_cxcdir}/bin
%define log4cplus_path /app/APG43L/SDK/3pp/log4cplus/4_0/log4cplus/lib
%define tracmd_caa_path %{_cxcdir}/bin
%define traconf_caa_path %{_cxcdir}/conf

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build
%description
Installation package for TRA.

%pre
if [ $1 -eq 1 ]
then
echo "This is the TRA package preinstall script Install"
fi

if [ $1 -eq 2 ]
then
echo "This is the TRA package preinstall script Upgrade"
rm -f /usr/lib64/libacs_tra.so
rm -f /usr/lib64/libacs_tra.so.3
rm -f /usr/lib64/liblog4cplus.so
rm -f /usr/lib64/liblog4cplus-1.0.so.4
rm -f /usr/bin/trautil
fi


%install
if [ ! -d $RPM_BUILD_ROOT%rootdir ]
then
    mkdir -p $RPM_BUILD_ROOT%rootdir
fi
if [ ! -d $RPM_BUILD_ROOT%APdir ]
then
    mkdir -p $RPM_BUILD_ROOT%APdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSdir ]
then
    mkdir -p $RPM_BUILD_ROOT%ACSdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSBINdir ]
then
    mkdir -p $RPM_BUILD_ROOT%ACSBINdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSLIB64dir ]
then
    mkdir -p $RPM_BUILD_ROOT%ACSLIB64dir
fi
#node_path='/etc/cluster/nodes/this/id'

#node_id=$(cat $node_path)
#if [ $node_id == 1 ]; then
#    if [ ! -d $RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf ]
#    then
#        mkdir -p $RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf
#    fi
#fi
if [ ! -d $RPM_BUILD_ROOT/var/log/acs/tra/logging ]
then
    mkdir -p $RPM_BUILD_ROOT/var/log/acs/tra/logging
fi


install -m 755 %traapi_caa_path/lib_ext/libacs_tra.so.3.7.1	$RPM_BUILD_ROOT%ACSLIB64dir/libacs_tra.so.3.7.1
install -m 755 %log4cplus_path/liblog4cplus-1.0.so.4.0.0	$RPM_BUILD_ROOT%ACSLIB64dir/liblog4cplus-1.0.so.4.0.0
cp %tracmd_caa_path/trautil		$RPM_BUILD_ROOT%ACSBINdir/trautil
cp %tracmd_caa_path/trautil.sh		$RPM_BUILD_ROOT%ACSBINdir/trautil.sh
cp %traconf_caa_path/log4cplus.properties	$RPM_BUILD_ROOT/var/log/log4cplus.properties
cp %traconf_caa_path/Trace_Status	$RPM_BUILD_ROOT/var/log/Trace_Status
cp %traconf_caa_path/Memory_Size	$RPM_BUILD_ROOT/var/log/Memory_Size

%post
echo "This is the TRA package postinstall script"

ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_tra.so.3.7.1 $RPM_BUILD_ROOT/usr/lib64/libacs_tra.so.3
ln -sf $RPM_BUILD_ROOT/usr/lib64/libacs_tra.so.3 $RPM_BUILD_ROOT/usr/lib64/libacs_tra.so
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/liblog4cplus-1.0.so.4.0.0 $RPM_BUILD_ROOT/usr/lib64/liblog4cplus-1.0.so.4
ln -sf $RPM_BUILD_ROOT/usr/lib64/liblog4cplus-1.0.so.4 $RPM_BUILD_ROOT/usr/lib64/liblog4cplus.so
ln -sf $RPM_BUILD_ROOT%ACSBINdir/trautil.sh /usr/bin/trautil


#echo "BEGIN: TRA Mutual Exclusive session"
echo "Checking for node ID"
#if [ -d /cluster/etc/ap/acs ]; then
#  echo "ACS cluster path exists"
#	if [ ! -f /cluster/etc/ap/acs/tra.lock ]; then
#    /usr/bin/lockfile -15 -r 22 -l 300 /cluster/etc/ap/acs/tra.lock 
#    if [ ! -d $RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf/ ]; then
#      mkdir -p $RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf/ 
#    fi
#  fi
#fi
#Creating files in /cluster only from one node. On second node wait till files created.
node_path='/etc/cluster/nodes/this/id'

node_id=$(cat $node_path)
if [ $node_id == 1 ]; then
    if [ ! -d $RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf ]
    then
        mkdir -p $RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf
    fi
    mv $RPM_BUILD_ROOT/var/log/log4cplus.properties	$RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf/log4cplus.properties
    chmod 664 $RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf/log4cplus.properties
    mv $RPM_BUILD_ROOT/var/log/Trace_Status	$RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf/Trace_Status
    mv $RPM_BUILD_ROOT/var/log/Memory_Size	$RPM_BUILD_ROOT/cluster/etc/ap/acs/tra/conf/Memory_Size
fi
if [ $node_id == 2 ]; then
   count=0
   while [[ ! -d /cluster/etc/ap/acs/tra/conf  && ! -f /cluster/etc/ap/acs/tra/conf/log4cplus.properties && ! -f /cluster/etc/ap/acs/tra/conf/Trace_Status && ! -f /cluster/etc/ap/acs/tra/conf/Memory_Size &&  $count -le 120  ]]
   do
      echo "node id is 2, sleeping for a while until /cluster/etc/ap/acs/tra/conf created"
      sleep 1
      count=`expr $count + 1`
   done
 fi
 
#assigning tsgroup as groupowner to log4cplus.properties file in case it does not have the tsgroup as groupowner
log4_path='/cluster/etc/ap/acs/tra/conf'
log4_file="$log4_path/log4cplus.properties"
if [ "$(stat -c %G $log4_file)" != "tsgroup" ]; then
    echo 'applying tsgroup: '
    chgrp tsgroup $log4_file
    if [ $? -eq 0 ]; then
        echo 'success'
    else 
        echo 'failed'
    fi
fi 

rm -f $RPM_BUILD_ROOT/var/log/log4cplus.properties
rm -f $RPM_BUILD_ROOT/var/log/Trace_Status
rm -f $RPM_BUILD_ROOT/var/log/Memory_Size
#rm -f /cluster/etc/ap/acs/tra.lock
#echo "END: TRA Mutual Exclusive session" 

%preun
echo "This is the TRA package pre uninstall script"

%postun
if [ $1 -eq 1 ]
then
echo "This is the TRA package post uninstall script Upgrade"
#rm -f %ACSLIB64dir/libacs_tra.so.3.7.1
fi

if [ $1 -eq 0 ]
then
echo "This is the TRA package post uninstall script Install"
rm -f /usr/lib64/libacs_tra.so
rm -f /usr/lib64/libacs_tra.so.3
rm -f /usr/lib64/liblog4cplus.so
rm -f /usr/lib64/liblog4cplus-1.0.so.4
rm -f %ACSLIB64dir/liblog4cplus-1.0.so.4.0.0
rm -f %ACSBINdir/trautil
rm -f %ACSBINdir/trautil.sh
rm -f /usr/bin/trautil
rm -f /cluster/etc/ap/acs/tra/conf/log4cplus.properties
rm -f /cluster/etc/ap/acs/tra/conf/Trace_Status
rm -f /cluster/etc/ap/acs/tra/conf/Memory_Size
fi

%files
%defattr(-,root,root)
%ACSLIB64dir/libacs_tra.so.3.7.1
%ACSLIB64dir/liblog4cplus-1.0.so.4.0.0
/var/log/log4cplus.properties
/var/log/Trace_Status
/var/log/Memory_Size
/var/log/acs/tra/logging
%ACSBINdir/trautil
%ACSBINdir/trautil.sh

%changelog
* Mon Jan 27 2020 - rajeshwari.p@tcs.com
* Wed Jul 07 2010 - giovanni.papale (at) its.na.it
- Initial implementation
