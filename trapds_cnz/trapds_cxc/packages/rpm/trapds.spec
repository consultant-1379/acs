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
Summary:   Installation package for TRAPDS.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires: APOS_OSCONFBIN

%define acs_trapds_cxc_path %{_cxcdir}

#BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%define _TRAPDS_LIB_NAME libacs_trapdsapi.so.1.1.3
%define _TRAPDS_LIB_LINKNAME libacs_trapdsapi.so.1
%define _TRAPDS_LIB_SONAME libacs_trapdsapi.so


%description
Installation package for TRAPDS.

%pre


%install
echo "Installing ACS_TRAPDS package"

mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/bin
mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/lib64
mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/conf

cp %acs_trapds_cxc_path/bin/lib_ext/%_TRAPDS_LIB_NAME $RPM_BUILD_ROOT/opt/ap/acs/lib64/%_TRAPDS_LIB_NAME
cp %acs_trapds_cxc_path/bin/acs_trapdsd	$RPM_BUILD_ROOT/opt/ap/acs/bin/acs_trapdsd
cp %acs_trapds_cxc_path/script/acs_trapds_clc $RPM_BUILD_ROOT/opt/ap/acs/bin/
cp -af %acs_trapds_cxc_path/conf/APZIM_TrapDispatcherService_imm_classes.xml	$RPM_BUILD_ROOT/opt/ap/acs/conf/
cp -af %acs_trapds_cxc_path/conf/APZIM_TrapDispatcherService_imm_objects.xml	$RPM_BUILD_ROOT/opt/ap/acs/conf/
#cp %acs_trapds_cxc_path/conf/ha_acs_trapds_objects.xml $RPM_BUILD_ROOT/opt/ap/ntacs/conf/ha_acs_trapds_objects.xml

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

echo "Finalizing ACS_TRAPDS package installation"
ln -sf /opt/ap/acs/lib64/%_TRAPDS_LIB_NAME $RPM_BUILD_ROOT/usr/lib64/%_TRAPDS_LIB_LINKNAME
ln -sf %_TRAPDS_LIB_LINKNAME $RPM_BUILD_ROOT/usr/lib64/%_TRAPDS_LIB_SONAME
ln -sf /opt/ap/acs/bin/acs_trapdsd $RPM_BUILD_ROOT/usr/bin/acs_trapdsd
rm -Rf /cluster/ACS/TRAPDS/

mkdir -p $RPM_BUILD_ROOT/var/run/ap

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"		
		rm -f /usr/bin/acs_trapdsd
		rm -f /usr/lib64/%_TRAPDS_LIB_LINKNAME
		rm -f /usr/lib64/%_TRAPDS_LIB_SONAME
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"	
		rm -f /opt/ap/acs/lib64/%_TRAPDS_LIB_NAME
		rm -f /opt/ap/acs/lib64/%_TRAPDS_LIB_LINKNAME
		rm -f /opt/ap/acs/lib64/%_TRAPDS_LIB_SONAME
		rm -f /opt/ap/acs/bin/acs_trapdsd
		rm -f /opt/ap/acs/bin/acs_trapds_clc
		rm -Rf /cluster/etc/ap/acs/trapds/
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi


%files
%defattr(-,root,root)
/opt/ap/acs/lib64/%_TRAPDS_LIB_NAME
%attr(555,root,root) /opt/ap/acs/bin/acs_trapdsd
%attr(555,root,root) /opt/ap/acs/bin/acs_trapds_clc
/opt/ap/acs/conf/APZIM_TrapDispatcherService_imm_classes.xml
/opt/ap/acs/conf/APZIM_TrapDispatcherService_imm_objects.xml
#/opt/ap/ntacs/conf/ha_acs_trapds_objects.xml

%changelog
* Wed Jul 07 2010 - nicola.muto (at) its.na.it
- Initial implementation
