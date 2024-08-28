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
Summary:   Installation package for MKTR.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires:  APOS_OSCONFBIN

%define mktrcmd_caa_path %{_cxcdir}/bin

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for MKTR.

%pre
if [ $1 == 1 ] 
then
echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
fi
if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
rm -f /usr/bin/mktr
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
if [ ! -d $RPM_BUILD_ROOT/var ]
then
    mkdir $RPM_BUILD_ROOT/var
fi
if [ ! -d $RPM_BUILD_ROOT/var/log ]
then
    mkdir $RPM_BUILD_ROOT/var/log
fi
if [ ! -d $RPM_BUILD_ROOT/var/log/acs ]
then
    mkdir $RPM_BUILD_ROOT/var/log/acs
fi
if [ ! -d $RPM_BUILD_ROOT/var/log/acs/mktr ]
then
    mkdir $RPM_BUILD_ROOT/var/log/acs/mktr
fi

cp %mktrcmd_caa_path/mktr		$RPM_BUILD_ROOT%ACSBINdir/mktr
cp %mktrcmd_caa_path/mktr.sh		$RPM_BUILD_ROOT%ACSBINdir/mktr.sh
cp %mktrcmd_caa_path/acs_btscript.sh            $RPM_BUILD_ROOT%ACSBINdir/acs_btscript.sh

%post
echo "This is the %{_name} package %{_rel} post-install script"
ln -sf $RPM_BUILD_ROOT%ACSBINdir/mktr.sh /usr/bin/mktr

chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_btscript.sh


%preun
echo "This is the %{_name} package %{_rel} pre-uninstall script"


%postun
if [ $1 == 0 ]
then
echo "This is the %{_name} package %{_rel} post-uninstall script uninstall phase"
rm -f %ACSBINdir/mktr
rm -f %ACSBINdir/mktr.sh
rm -f %ACSBINdir/acs_btscript.sh
rm -f /usr/bin/mktr
fi
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} post-uninstall script update phase"
fi


%files
%defattr(-,root,root)
%ACSBINdir/mktr
%ACSBINdir/mktr.sh
%ACSBINdir/acs_btscript.sh
/var/log/acs/mktr

%changelog
* Wed Jul 07 2010 - giovanni.papale (at) its.na.it
- Initial implementation
