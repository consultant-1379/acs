#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

Name:      %{_name}
Summary:   Installation package for ACS_USAFM.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Application
BuildRoot: %_tmppath
Requires:  APOS_OSCONFBIN


%define acs_usafmcxc_bin_path %{_cxcdir}/bin
%define acs_usafmcxc_lib_path %{_cxcdir}/bin/lib_int
%define acs_usacxc_bin_path %{_cxcdir}/../usa_cxc/bin
%define _storage_api /usr/share/pso/storage-paths/clear

%define _USAFM_LIB_NAME libacs_usa_fm.so.2.3.0
%define _USAFM_LINKNAME libacs_usa_fm.so.2
%define _USAFM_SONAME libacs_usa_fm.so

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for ACS_USAFM.

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
echo "copying the files required"

mkdir -p $RPM_BUILD_ROOT%ACSBINdir
mkdir -p $RPM_BUILD_ROOT/usr/lib64
mkdir -p $RPM_BUILD_ROOT%ACSCONFdir
mkdir -p $RPM_BUILD_ROOT%ACSLIB64dir




#copy usa_fm library here
cp %acs_usafmcxc_lib_path/%_USAFM_LIB_NAME $RPM_BUILD_ROOT%ACSLIB64dir/%_USAFM_LIB_NAME
cp %acs_usafmcxc_bin_path/bounce_com.sh $RPM_BUILD_ROOT%ACSBINdir/bounce_com.sh
cp %acs_usacxc_bin_path/eventGen $RPM_BUILD_ROOT%ACSBINdir/eventGen
cp %acs_usacxc_bin_path/eventCreate $RPM_BUILD_ROOT%ACSBINdir/eventCreate


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
ln -sf /opt/ap/acs/lib64/%_USAFM_LIB_NAME /opt/ap/acs/lib64/%_USAFM_LINKNAME
ln -sf /opt/ap/acs/lib64/%_USAFM_LINKNAME /opt/com/lib/comp/%_USAFM_SONAME
echo -n ".done"

echo "Applying SUGar on the xml.."
storage_path=$(cat %{_storage_api})
alarm_list_file="$storage_path/acs_usafm/active_fm_alarm_list.xml"
if [ ! -f $alarm_list_file ]; then
	touch $alarm_list_file
fi

if [ "$(stat -c %U $alarm_list_file)" != "com-core" ]; then
	echo 'applying com-core user: '
	chown com-core $alarm_list_file
	if [ $? -eq 0 ]; then
		echo 'success'
	else 
		echo 'failed'
	fi
fi

if [ "$(stat -c %G $alarm_list_file)" != "com-core" ]; then
	echo 'applying com-core group: '
	chgrp com-core $alarm_list_file
	if [ $? -eq 0 ]; then
		echo 'success'
	else 
		echo 'failed'
	fi
fi

if [ "$(stat -c %a $alarm_list_file)" != "660" ]; then
	echo 'applying access permissions: '
	chmod 660  $alarm_list_file
	if [ $? -eq 0 ]; then
		echo 'success'
	else 
		echo 'failed'
	fi
fi
	
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
	echo "cleaning up ACS_USAFM"

	rm -f %ACSLIB64dir/%_USAFM_LIB_NAME
	rm -f %ACSLIB64dir/%_USAFM_LINKNAME
	rm -f /opt/com/lib/comp/%_USAFM_SONAME
	rm -f %ACSBINdir/bounce_com.sh
	rm -f %ACSBINdir/eventGen
	rm -f %ACSBINdir/evantCreate
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%attr(0755,root,root) %ACSLIB64dir/%_USAFM_LIB_NAME
%attr(0755,root,root) %ACSBINdir/bounce_com.sh
%attr(0755,root,root) %ACSBINdir/eventGen
%attr(0755,root,root) %ACSBINdir/eventCreate

%changelog
* Tue Aug 02 2011 - s.malangsha (at) tcs.com
- Initial implementation


