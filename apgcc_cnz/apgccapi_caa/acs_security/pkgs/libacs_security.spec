#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#

%define packer %(finger -lp `echo "$USER"` | head -n 1 | cut -d: -f 3)
%define _topdir /var/tmp/%(echo "$USER")/rpms/libacs_security
%define _tmppath %_topdir/tmp

Name:      libacs_security
Summary:   Installation package for APGCC.
Version:   1.0
Release:   0.0
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath

#BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for APGCC.

%pre
echo "This is the libACS Security package preinstall script"

%install
mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/bin

cp %APOS_ROOT/ha_cnz/packages/libacs_secuirty/src/bin/ACS_Security_CommonAPIDemo $RPM_BUILD_ROOT/opt/ap/acs/bin/

%post
echo "exporting the path..."
export export PATH=$PATH:/opt/ap/acs/bin/
echo -e "done"

%preun
echo "This is the libACS Security package pre uninstall script"

%postun
echo "This is the libACS Security package post uninstall script"
rm -f /opt/AP/ACS/bin/ACS_Security_CommonAPIDemo

%files
%defattr(-,root,root)
%attr(0755,root,root) /opt/ap/acs/bin/ACS_Security_CommonAPIDemo

%changelog
* Wed Jul 07 2010 - fabio.ronca (at) its.na.it
- Initial implementation

