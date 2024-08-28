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
Summary:   Installation package for SCH.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires: APOS_OSCONFBIN

%define sch_cxc_path %{_cxcdir}

#BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for SCH.

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
echo "Installing ACS_SCH package"
if [ ! -d $RPM_BUILD_ROOT/opt/ap/acs/bin ]
then
	mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/bin
fi

if [ ! -d $RPM_BUILD_ROOT/opt/ap/acs/conf ]
then
	mkdir -p $RPM_BUILD_ROOT/opt/ap/acs/conf
fi

cp %sch_cxc_path/bin/acs_schd    $RPM_BUILD_ROOT/opt/ap/acs/bin/acs_schd
cp %sch_cxc_path/bin/acs_sch_2N_clc $RPM_BUILD_ROOT/opt/ap/acs/bin/acs_sch_2N_clc
cp %sch_cxc_path/conf/AxeScalingM_imm_classes.xml $RPM_BUILD_ROOT%ACSdir/conf
cp %sch_cxc_path/conf/AxeScalingM_mp.xml $RPM_BUILD_ROOT%ACSdir/conf
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
chmod +x /opt/ap/acs/bin/acs_schd 
chmod +x /opt/ap/acs/bin/acs_sch_2N_clc 

ln -sf /opt/ap/acs/bin/acs_schd $RPM_BUILD_ROOT/usr/bin/acs_schd
ln -sf /opt/ap/acs/bin/acs_sch_2N_clc $RPM_BUILD_ROOT/usr/bin/acs_sch_2N_clc

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"
		rm -f /usr/bin/acs_schd
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi
 
%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"
		rm -f /opt/ap/acs/bin/acs_schd
		rm -f /opt/ap/acs/bin/acs_sch_2N_clc
		rm -Rf /cluster/etc/ap/acs/sch/
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%attr(755,root,root) /opt/ap/acs/bin/acs_schd
%attr(755,root,root) /opt/ap/acs/bin/acs_sch_2N_clc

%changelog
* Wed Jul 07 2010 - nicola.muto (at) its.na.it
- Initial implementation

