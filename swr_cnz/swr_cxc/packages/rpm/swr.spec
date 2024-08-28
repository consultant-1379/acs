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
%define _topdir /var/tmp/%(echo "$USER")/rpms/
%define _tmppath %_topdir/tmp
%define swr_cxc_path %{_cxcdir}

Name:      %{_name} 
Summary:   Installation package for SWR.
Version:   %{_prNr} 
Release:   %{_rel} 
License:   Ericsson Proprietary 
Vendor:    Ericsson SWR
Packager:  %packer 
Group:     Library 
BuildRoot: %_tmppath 
Requires: APOS_OSCONFBIN

%description
Installation package for SWR

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

if [ ! -d $RPM_BUILD_ROOT%rootdir ]
then
        mkdir $RPM_BUILD_ROOT%rootdir
fi
if [ ! -d $RPM_BUILD_ROOT%APdir ]
then
        mkdir $RPM_BUILD_ROOT%APdir
fi
if  [ ! -d $RPM_BUILD_ROOT%ACSdir ]
then
        mkdir $RPM_BUILD_ROOT%ACSdir
fi
if [ ! -d  $RPM_BUILD_ROOT%ACSBINdir ]
then
        mkdir $RPM_BUILD_ROOT%ACSBINdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSCONFdir ]
then
        mkdir $RPM_BUILD_ROOT%ACSCONFdir
fi

cp %swr_cxc_path/bin/swrprint 			$RPM_BUILD_ROOT%ACSBINdir/swrprint
cp %swr_cxc_path/bin/rpswrprint 		$RPM_BUILD_ROOT%ACSBINdir/rpswrprint
cp %swr_cxc_path/bin/swrprint.sh 		$RPM_BUILD_ROOT%ACSBINdir/swrprint.sh
cp %swr_cxc_path/bin/rpswrprint.sh 		$RPM_BUILD_ROOT%ACSBINdir/rpswrprint.sh

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

chmod +x $RPM_BUILD_ROOT%ACSBINdir/swrprint
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rpswrprint
chmod +x $RPM_BUILD_ROOT%ACSBINdir/swrprint.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/rpswrprint.sh

ln -sf $RPM_BUILD_ROOT%ACSBINdir/swrprint.sh $RPM_BUILD_ROOT%{_bindir}/swrprint
ln -sf $RPM_BUILD_ROOT%ACSBINdir/rpswrprint.sh $RPM_BUILD_ROOT%{_bindir}/rpswrprint

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f %{_bindir}/swrprint
	rm -f %{_bindir}/rpswrprint
	rm -f %{_bindir}/swrprint.sh
	rm -f %{_bindir}/rpswrprint.sh
	
	
fi
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSBINdir/swrprint
	rm -f %ACSBINdir/rpswrprint
	rm -f %ACSBINdir/swrprint.sh
	rm -f %ACSBINdir/rpswrprint.sh
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/swrprint
%ACSBINdir/rpswrprint
%ACSBINdir/swrprint.sh
%ACSBINdir/rpswrprint.sh

%changelog
* Fri Jul 01 2022 - debdutta.c (at) tcs.com
- changes for rpswrprint command
* Fri May 11 2012 - tsameer.chandra (at) tcs.com
-Initial Implementation

