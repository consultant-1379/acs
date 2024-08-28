#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

Name:      %{_name}
Summary:   Installation package for ACS_USA.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Application
BuildRoot: %_tmppath
Requires:  APOS_OSCONFBIN

%define acs_usacxc_bin_path %{_cxcdir}/bin

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for ACS_USA.

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
cp %acs_usacxc_bin_path/acs_usa_genEvtLst $RPM_BUILD_ROOT%ACSBINdir/acs_usa_genEvtLst
cp %acs_usacxc_bin_path/acs_usad $RPM_BUILD_ROOT%ACSBINdir/acs_usad
cp %acs_usacxc_bin_path/acs_usa_clc $RPM_BUILD_ROOT%ACSBINdir/acs_usa_clc

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
	rm -f %ACSBINdir/usa.tmp
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

echo "Creating the symbolic link.."
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
	echo "cleaning up ACS_USA"

	rm -f %ACSBINdir/acs_usa_clc
	rm -f %ACSBINdir/acs_usad
	rm -f %ACSBINdir/acs_usa_genEvtLst
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%attr(0755,root,root) %ACSBINdir/acs_usad
%attr(0755,root,root) %ACSBINdir/acs_usa_genEvtLst
%attr(0755,root,root) %ACSBINdir/acs_usa_clc

%changelog
* Mon Mar 21 2016 - Nazeema Begum (at) tcs.com
- Added step to remove usa.tmp during post-install during upgrade phase.
* Tue Aug 02 2011 - s.malangsha (at) tcs.com
- Initial implementation
