#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
# Modified 	25-11-2010	xmalsha (malangsha.shaik@ericsson.com)
#


Name:      %{_name}
Summary:   Installation package for APGCC.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath

Requires: APOS_OSCONFBIN

%define apgcc_cxc_path  %{_cxcdir}
%define apgccapi_caa_path %apgcc_cxc_path/bin/lib_ext 
%define apgccapi_ha_path %apgcc_cxc_path/../apgccapi_caa/ha
%define apgccapi_conf_path %apgcc_cxc_path/conf

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%define _APGCC_LIB_NAME libacs_apgcc.so.5.16.2
%define _APGCC_LIB_LINKNAME libacs_apgcc.so.5
%define _APGCC_LIB_SONAME libacs_apgcc.so
%define _APGCC_AUTHZ_CACHE_CONF_FILE acs_apgcc_authorization_cache_validity_time

%description
Installation package for APGCC.

%pre
if [ $1 -eq 1 ]
then
echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
fi

if [ $1 -eq 2 ]
then
echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase - Begin"
echo "Removing /usr/lib64/%_APGCC_LIB_SONAME"
rm -f /usr/lib64/%_APGCC_LIB_SONAME
echo "Removing /usr/lib64/%APGCC_LIB_LINKNAME"
rm -f /usr/lib64/%APGCC_LIB_LINKNAME
echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase - End"
fi


%install
echo "This is the %{_name} package %{_rel} install script - Begin"
if [ ! -d $RPM_BUILD_ROOT%rootdir ]
then
	echo "Creating dir $RPM_BUILD_ROOT%rootdir"
	mkdir $RPM_BUILD_ROOT%rootdir
fi
if [ ! -d $RPM_BUILD_ROOT%APdir ]
then
	echo "Creating dir $RPM_BUILD_ROOT%APdir"
	mkdir $RPM_BUILD_ROOT%APdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSdir ]
then
	echo "Creating dir $RPM_BUILD_ROOT%ACSdir"
	mkdir $RPM_BUILD_ROOT%ACSdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSBINdir ]
then
	echo "Creating dir $RPM_BUILD_ROOT%ACSBINdir"
	mkdir $RPM_BUILD_ROOT%ACSBINdir
fi
if [ ! -d $RPM_BUILD_ROOT%{ACSCONFdir} ]
then
	echo "Creating dir $RPM_BUILD_ROOT%{ACSCONFdir}"
	mkdir $RPM_BUILD_ROOT%{ACSCONFdir}
fi
if [ ! -d $RPM_BUILD_ROOT%ACSLIB64dir ]
then
	echo "Creating dir $RPM_BUILD_ROOT%ACSLIB64dir"
	mkdir $RPM_BUILD_ROOT%ACSLIB64dir
fi
if [ ! -d $RPM_BUILD_ROOT%APOSBINdir ]
then
	echo "Creating dir $RPM_BUILD_ROOT%APOSBINdir"
	mkdir -p $RPM_BUILD_ROOT%APOSBINdir
fi

echo "Copy %apgccapi_caa_path/%_APGCC_LIB_NAME in $RPM_BUILD_ROOT%ACSLIB64dir/%_APGCC_LIB_NAME"
cp %apgccapi_caa_path/%_APGCC_LIB_NAME	$RPM_BUILD_ROOT%ACSLIB64dir/%_APGCC_LIB_NAME
echo "Copy %{apgccapi_conf_path}/%{_APGCC_AUTHZ_CACHE_CONF_FILE} in $RPM_BUILD_ROOT%{ACSCONFdir}/%{_APGCC_AUTHZ_CACHE_CONF_FILE}"
cp %{apgccapi_conf_path}/%{_APGCC_AUTHZ_CACHE_CONF_FILE} $RPM_BUILD_ROOT%{ACSCONFdir}/%{_APGCC_AUTHZ_CACHE_CONF_FILE}
echo "Copy %apgccapi_ha_path/external_api/src/apg_passive_node_reboot.sh in $RPM_BUILD_ROOT%APOSBINdir/apg_passive_node_reboot.sh"
cp %apgccapi_ha_path/external_api/src/apg_passive_node_reboot.sh $RPM_BUILD_ROOT%APOSBINdir/apg_passive_node_reboot.sh
echo "Copy %apgccapi_ha_path/external_api/src/apg_service_admin_operations.sh in $RPM_BUILD_ROOT%APOSBINdir/apg_service_admin_operations.sh"
cp %apgccapi_ha_path/external_api/src/apg_service_admin_operations.sh $RPM_BUILD_ROOT%APOSBINdir/apg_service_admin_operations.sh

echo "This is the %{_name} package %{_rel} install script - End"

%post
echo "This is the %{_name} package %{_rel} post-install script - Begin"
echo "Creating symbolic link from /usr/lib64/%_APGCC_LIB_LINKNAME to $RPM_BUILD_ROOT%ACSLIB64dir/%_APGCC_LIB_NAME"
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/%_APGCC_LIB_NAME /usr/lib64/%_APGCC_LIB_LINKNAME
echo "Creating symbolic link from /usr/lib64/%_APGCC_LIB_SONAME to /usr/lib64/%_APGCC_LIB_LINKNAME"
ln -sf /usr/lib64/%_APGCC_LIB_LINKNAME /usr/lib64/%_APGCC_LIB_SONAME
echo "This is the %{_name} package %{_rel} post-install script - End"
#/usr/sbin/dmidecode -s baseboard-product-name > /var/log/HWVer
#if [ -e /var/run/ap/dsdapi_protocol_sap ]
#then
#	chown apgusersvc /var/run/ap/dsdapi_protocol_sap
#fi

%preun
echo "This is the %{_name} package %{_rel} pre uninstall script"

%postun
if [ $1 -eq 1 ]
then
echo "This is the %{_name} package %{_rel} post uninstall script for Upgrade"
fi

if [ $1 -eq 0 ]
then
echo "This is the %{_name} package %{_rel} post uninstall script for Install - Begin"
echo "Removing /usr/lib64/%_APGCC_LIB_SONAME"
rm -f /usr/lib64/%_APGCC_LIB_SONAME
echo "Removing /usr/lib64/%_APGCC_LIB_LINKNAME"
rm -f /usr/lib64/%_APGCC_LIB_LINKNAME
echo "Removing %ACSLIB64dir/%_APGCC_LIB_NAME"
rm -f %ACSLIB64dir/%_APGCC_LIB_NAME
echo "Removing %ACSCONFdir/%{_APGCC_AUTHZ_CACHE_CONF_FILE}"
rm -f %ACSCONFdir/%{_APGCC_AUTHZ_CACHE_CONF_FILE}
echo "Removing %APOSBINdir/apg_passive_node_reboot.sh"
rm -f %APOSBINdir/apg_passive_node_reboot.sh
echo "Removing %APOSBINdir/apg_service_admin_operations.sh"
rm -f %APOSBINdir/apg_service_admin_operations.sh
echo "This is the %{_name} package %{_rel} post uninstall script for Install - End"
fi

%files
%defattr(-,root,root)
%ACSLIB64dir/%_APGCC_LIB_NAME
%ACSCONFdir/%{_APGCC_AUTHZ_CACHE_CONF_FILE}
%APOSBINdir/apg_passive_node_reboot.sh
%APOSBINdir/apg_service_admin_operations.sh

%changelog
* Mon Oct 17 2022 - p soumya at tcs
- Changed apgcc lib version. 
* Tue Aug 05 2014 - stefano.volpe (at) tei.eri
- Added variables for LIBNAME
* Wed Jul 07 2010 - fabio.ronca (at) its.na.it
- Initial implementation

