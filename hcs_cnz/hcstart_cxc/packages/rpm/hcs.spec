#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#

Name:      %{_name}
Summary:   Installation package for HCStart BIN.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Application
BuildRoot: %_tmppath

Requires: APOS_OSCONFBIN
%define acs_hcscxc_path %{_cxcdir}
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation package for HCSBIN.

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
if [ ! -d $RPM_BUILD_ROOT%ACSCONFdir ]
then
	mkdir $RPM_BUILD_ROOT%ACSCONFdir
fi
if [ ! -d $RPM_BUILD_ROOT/cluster/etc/userinfo ]
then
	mkdir -p $RPM_BUILD_ROOT/cluster/etc/
fi

cp %acs_hcscxc_path/bin/acs_hcstart_common $RPM_BUILD_ROOT%ACSBINdir/acs_hcstart_common
cp %acs_hcscxc_path/bin/acs_hcs_hcstart.sh $RPM_BUILD_ROOT%ACSBINdir/acs_hcs_hcstart.sh
cp %acs_hcscxc_path/bin/acs_hcstart_reqid $RPM_BUILD_ROOT%ACSBINdir/acs_hcstart_reqid
cp %acs_hcscxc_path/bin/hcstart $RPM_BUILD_ROOT%ACSBINdir/hcstart
cp %acs_hcscxc_path/conf/acs_hcstart_reqid_list.conf $RPM_BUILD_ROOT%ACSCONFdir/acs_hcstart_reqid_list.conf

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
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_hcstart_common 
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_hcs_hcstart.sh
chmod +x $RPM_BUILD_ROOT%ACSBINdir/acs_hcstart_reqid 
chmod +x $RPM_BUILD_ROOT%ACSBINdir/hcstart 

#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_cstart_common /usr/bin/acs_hcstart_common
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_hcs_hcstartbin.sh /usr/bin/hcstart
#ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_hcstart_reqid /usr/bin/acs_hcstart_reqid
ln -sf $RPM_BUILD_ROOT%ACSBINdir/hcstart /usr/bin/hcstart

%preun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

rm -f /usr/bin/hcstart
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

rm -f %ACSBINdir/acs_hcs_common
rm -f %ACSBINdir/acs_hcs_hcstart.sh
rm -f %ACSBINdir/acs_hcs_reqid
rm -f %ACSCONFdir/acs_hcs_reqid_list.conf
rm -f %ACSBINdir/hcstart
fi

if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi
#%posttrans
#        echo "This is the %{_name} package %{_rel} post-trans phase"
#	mv $RPM_BUILD_ROOT%ACSBINdir/acs_hcs_hcstartbin.sh $RPM_BUILD_ROOT%ACSBINdir/acs_hcs_hcstart.sh 2>/dev/dull
#	mv $RPM_BUILD_ROOT%ACSBINdir/hcstartbin $RPM_BUILD_ROOT%ACSBINdir/hcstart 2>/dev/dull

%files
%defattr(-,root,root)
%attr(0755,root,root) %ACSBINdir/acs_hcstart_common
%attr(0755,root,root) %ACSBINdir/acs_hcs_hcstart.sh
%attr(0755,root,root) %ACSBINdir/acs_hcstart_reqid
%attr(0755,root,root) %ACSCONFdir/acs_hcstart_reqid_list.conf
%attr(0755,root,root) %ACSBINdir/hcstart

%changelog
* Wed Apr 16 2015 - Divya Chakkilam (XCHADIV)
-  Name change defect in R1A01/AXE16A resolved
* Wed Jul 07 2011 - sonali.nanda (at) tcs.com
- Initial implementation

