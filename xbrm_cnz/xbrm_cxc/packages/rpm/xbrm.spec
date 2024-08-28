#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#
# - 22 Jun 2023 - P S Soumya (ZPSXSOU)      First version

Name:      %{_name}
Summary:   Installation package for XBRM.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires: APOS_OSCONFBIN

%define xbrm_cxc_path %{_cxcdir}

#BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for XBRM.

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
echo "Installing ACS_XBRM package"
if [ ! -d $RPM_BUILD_ROOT/opt/ap/acs/bin ]
then
	mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/bin
fi

if [ ! -d $RPM_BUILD_ROOT/opt/ap/acs/conf ]
then
	mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/conf
fi

cp %xbrm_cxc_path/bin/acs_xbrmd    $RPM_BUILD_ROOT/opt/ap/acs/bin/acs_xbrmd
cp %xbrm_cxc_path/bin/acs_xbrm_2N_clc $RPM_BUILD_ROOT/opt/ap/acs/bin/acs_xbrm_2N_clc
cp %xbrm_cxc_path/conf/SystemBrM_imm_classes.xml $RPM_BUILD_ROOT%ACSdir/conf
cp %xbrm_cxc_path/conf/SystemBrM_mp.xml $RPM_BUILD_ROOT%ACSdir/conf
%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

#Start chmod
chmod +x /opt/ap/acs/bin/acs_xbrmd 
chmod +x /opt/ap/acs/bin/acs_xbrm_2N_clc 

ln -sf /opt/ap/acs/bin/acs_xbrmd $RPM_BUILD_ROOT/usr/bin/acs_xbrmd
ln -sf /opt/ap/acs/bin/acs_xbrm_2N_clc $RPM_BUILD_ROOT/usr/bin/acs_xbrm_2N_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"
		rm -f /usr/bin/acs_xbrmd
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi
 
%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"
		rm -f /opt/ap/acs/bin/acs_xbrmd
		rm -f /opt/ap/acs/bin/acs_xbrm_2N_clc
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%attr(755,root,root) /opt/ap/acs/bin/acs_xbrmd
%attr(755,root,root) /opt/ap/acs/bin/acs_xbrm_2N_clc

%changelog
* Tue Jul 11 2023 - zpsxsou
- Initial implementation

