#
# spec file for configuration of package apache
#
# Copyright  (c)  2010  Ericssonk LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#2

Name:      %{_name}
Summary:   Installation package for PRC.
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Library
BuildRoot: %_tmppath
Requires:  APOS_OSCONFBIN ACS_AEHBIN ACS_APGCCBIN ACS_CSBIN ACS_TRAUTLBIN ACS_DSDBIN

%define prc_bin_path %{_cxcdir}/bin

#BuildRoot: %{_tmppath}/%{name}_%{version}_%{release}-build

%description
Installation package for PRC.

%pre
if [ $1 == 1 ] 
then
echo "This is the %{_name} package %{_rel} pre-install script during installation phase"
#mkfifo /var/run/ap/prcisp
fi
if [ $1 == 2 ]
then
echo "This is the %{_name} package %{_rel} pre-install script during upgrade phase"
rm -f /usr/bin/ispprint
rm -f /usr/bin/acs_prcispd
rm -f /usr/bin/prcstate
rm -f /usr/bin/acs_prcmand
rm -f /usr/bin/prcboot
rm -f /usr/lib64/libacs_prc.so
rm -f /usr/lib64/libacs_prc.so.2
rm -f /opt/com/lib/comp/libacs_prc_evt.so
rm -f ACSLIB64dir/libacs_prc_evt.so.1
rm -f /opt/ap/acs/conf/res.conf

fi

%install
echo "This is the %{_name} package %{_rel} install script"
if [ ! -d $RPM_BUILD_ROOT/usr/lib/systemd/system/ ]
then
  echo "/usr/lib/systemd/system/ getting created...."
  mkdir -p $RPM_BUILD_ROOT/usr/lib/systemd/system/
fi
if [ ! -d $RPM_BUILD_ROOT/usr/lib/systemd/scripts/ ]
then 
  echo "/usr/lib/systemd/scripts/ getting created...."
  mkdir -p $RPM_BUILD_ROOT/usr/lib/systemd/scripts/
fi
if [ ! -d $RPM_BUILD_ROOT%rootdir ]
then
        echo "rootdir getting created...."
	mkdir -p $RPM_BUILD_ROOT%rootdir
fi
if [ ! -d $RPM_BUILD_ROOT%APdir ]
then
        echo "APdir getting created...."
	mkdir -p $RPM_BUILD_ROOT%APdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSdir ]
then
        echo "ACSdir getting created...."
	mkdir -p $RPM_BUILD_ROOT%ACSdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSBINdir ]
then
        echo "ACSBINdir getting created...."
	mkdir -p $RPM_BUILD_ROOT%ACSBINdir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSLIB64dir ]
then
        echo "ACSLIB64dir getting created...."
	mkdir -p $RPM_BUILD_ROOT%ACSLIB64dir
fi
if [ ! -d $RPM_BUILD_ROOT%ACSCONFdir ]
then
        echo "ACSCONFdir getting created...."
	mkdir -p $RPM_BUILD_ROOT%ACSCONFdir
fi

echo "prc_bin_path/ispprint getting copied...."
cp %prc_bin_path/ispprint		$RPM_BUILD_ROOT%ACSBINdir/ispprint

echo "prc_bin_path/acs_prcispd getting copied....."
cp %prc_bin_path/acs_prcispd		$RPM_BUILD_ROOT%ACSBINdir/acs_prcispd

echo "prc_bin_path/acs_prcmand getting copied....."
cp %prc_bin_path/acs_prcmand		$RPM_BUILD_ROOT%ACSBINdir/acs_prcmand

echo "prc_bin_path/prcstate getting copied....."
cp %prc_bin_path/prcstate		$RPM_BUILD_ROOT%ACSBINdir/prcstate

echo "prc_bin_path/prcboot getting copied....."
cp %prc_bin_path/prcboot		$RPM_BUILD_ROOT%ACSBINdir/prcboot

echo "prc_bin_path/lib_ext/libacs_prc.so.2.35.1 getting copied....."
cp %prc_bin_path/lib_ext/libacs_prc.so.2.35.1	$RPM_BUILD_ROOT%ACSLIB64dir/libacs_prc.so.2.35.1

echo "prc_bin_path/lib_int/libacs_prc_evt.so.1.0.2 getting copied....."
cp %prc_bin_path/lib_int/libacs_prc_evt.so.1.0.2	$RPM_BUILD_ROOT%ACSLIB64dir/libacs_prc_evt.so.1.0.2

echo "prc_bin_path/prcstate.sh getting copied....."
cp %prc_bin_path/prcstate.sh		$RPM_BUILD_ROOT%ACSBINdir/prcstate.sh

echo "prc_bin_path/prcboot.sh getting copied....."
cp %prc_bin_path/prcboot.sh		$RPM_BUILD_ROOT%ACSBINdir/prcboot.sh

echo "prc_bin_path/ispprint.sh getting copied....."
cp %prc_bin_path/ispprint.sh		$RPM_BUILD_ROOT%ACSBINdir/ispprint.sh

echo "prc_bin_path/res.conf getting copied....."
cp %prc_bin_path/res.conf	$RPM_BUILD_ROOT%ACSCONFdir/res.conf

echo "setting permissions....."
install -m 0644 %prc_bin_path/acs_prcmand.service $RPM_BUILD_ROOT/usr/lib/systemd/system/acs_prcmand.service
install -m 0644 %prc_bin_path/acs_prcispd.service $RPM_BUILD_ROOT/usr/lib/systemd/system/acs_prcispd.service
install -m 0644 %prc_bin_path/cyclic-boot-delay.service $RPM_BUILD_ROOT/usr/lib/systemd/system/cyclic-boot-delay.service
install -m 0755 %prc_bin_path/cyclic-boot-delay.sh $RPM_BUILD_ROOT/usr/lib/systemd/scripts/cyclic-boot-delay.sh
install -m 0755 %prc_bin_path/cyclic_reboot_delay_conf.sh	$RPM_BUILD_ROOT%ACSBINdir/cyclic_reboot_delay_conf.sh
install -m 0755 %prc_bin_path/cyclic_reboot_delay_config.sh	$RPM_BUILD_ROOT%ACSBINdir/cyclic_reboot_delay_config.sh


%post
echo "This is the %{_name} package %{_rel} post-install script"


#echo "checking node ID"
#node_path='/etc/cluster/nodes/this/id'
#node_id=$(cat $node_path)

#if [ $node_id == 2 ]; then
#   echo "node id is 2, sleeping for 5 seconds"
#   sleep 5
# fi


#if [ $node_id == 1 ]; then
#  if [ ! -d $RPM_BUILD_ROOT/cluster/etc/ap/acs/prc/conf ]
#  then
#    echo "cluster/etc/ap/acs/prc/conf getting created...."
#    mkdir -p $RPM_BUILD_ROOT/cluster/etc/ap/acs/prc/conf
#  fi
#:fi
chmod 400 /opt/ap/acs/conf/res.conf
SERVICE_NAME=cyclic-boot-delay

echo "creating link for libacs_prc_evt.so.1.0.2....."
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_prc_evt.so.1.0.2 $RPM_BUILD_ROOT%ACSLIB64dir/libacs_prc_evt.so.1

echo "creating link for libacs_prc_evt.so.1....."
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_prc_evt.so.1 /opt/com/lib/comp/libacs_prc_evt.so

echo "creating link for libacs_prc.so.2.35.1....."
ln -sf $RPM_BUILD_ROOT%ACSLIB64dir/libacs_prc.so.2.35.1 /usr/lib64/libacs_prc.so.2

echo "creating link for libacs_prc.so.2....."
ln -sf /usr/lib64/libacs_prc.so.2 /usr/lib64/libacs_prc.so

echo "creating link for ispprint....."
ln -sf $RPM_BUILD_ROOT%ACSBINdir/ispprint.sh /usr/bin/ispprint

echo "creating link for acs_prcispd....."
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_prcispd /usr/bin/acs_prcispd

echo "creating link for acs_prcmand....."
ln -sf $RPM_BUILD_ROOT%ACSBINdir/acs_prcmand /usr/bin/acs_prcmand

echo "creating link for prcstate....."
ln -sf $RPM_BUILD_ROOT%ACSBINdir/prcstate.sh /usr/bin/prcstate

echo "creating link for prcboot.sh....."
ln -sf $RPM_BUILD_ROOT%ACSBINdir/prcboot.sh /usr/bin/prcboot

echo "creating link for cyclic_reboot_delay_conf.sh....."
ln -sf $RPM_BUILD_ROOT%ACSBINdir/cyclic_reboot_delay_conf.sh /usr/bin/cyclic_reboot_delay_conf

echo "enabling, reloading and restarting services....."
/opt/ap/apos/bin/servicemgmt/servicemgmt enable cyclic-boot-delay.service
/opt/ap/apos/bin/servicemgmt/servicemgmt enable acs_prcmand.service
/opt/ap/apos/bin/servicemgmt/servicemgmt enable acs_prcispd.service
/opt/ap/apos/bin/servicemgmt/servicemgmt reload acs_prcmand.service --type=service
/opt/ap/apos/bin/servicemgmt/servicemgmt restart acs_prcmand.service
/opt/ap/apos/bin/servicemgmt/servicemgmt restart acs_prcispd.service

%preun
echo "This is the %{_name} package %{_rel} pre-uninstall script"

%postun
if [ $1 == 0 ]
then
echo "This is the %{_name} package %{_rel} post-uninstall script uninstall phase"
rm -f %ACSBINdir/ispprint
rm -f %ACSBINdir/ispprint.sh
rm -f %ACSBINdir/acs_prcispd
rm -f %ACSBINdir/prcstate
rm -f %ACSBINdir/prcstate.sh
rm -f %ACSBINdir/acs_prcmand
rm -f %ACSBINdir/prcboot
rm -f %ACSBINdir/prcboot.sh
rm -f %ACSBINdir/cyclic_reboot_delay_conf.sh
rm -f %ACSBINdir/cyclic_reboot_delay_config.sh
rm -f /usr/bin/cyclic_reboot_delay_conf
rm -f /usr/bin/ispprint
rm -f /usr/bin/acs_prcispd
rm -f /usr/bin/prcstate
rm -f /usr/bin/acs_prcmand
rm -f /usr/bin/prcboot
rm -f /usr/lib64/libacs_prc.so
rm -f /usr/lib64/libacs_prc.so.2
rm -f %ACSLIB64dir/libacs_prc.so.2.35.1
rm -f /opt/com/lib/comp/libacs_prc_evt.so
rm -f %ACSLIB64dir/libacs_prc_evt.so.1
rm -f %ACSLIB64dir/libacs_prc_evt.so.1.0.2
rm -f /opt/ap/acs/conf/res.conf
#unlink /var/run/ap/prcisp
fi
if [ $1 == 1 ]
then
echo "This is the %{_name} package %{_rel} post-uninstall script update phase"
fi

%files
%defattr(-,root,root)
%ACSBINdir/ispprint
%ACSBINdir/ispprint.sh
%ACSBINdir/acs_prcispd
%ACSBINdir/prcstate
%ACSBINdir/prcstate.sh
%ACSBINdir/acs_prcmand
%ACSBINdir/prcboot
%ACSBINdir/prcboot.sh
%ACSBINdir/cyclic_reboot_delay_conf.sh
%ACSBINdir/cyclic_reboot_delay_config.sh
%ACSLIB64dir/libacs_prc.so.2.35.1
%ACSLIB64dir/libacs_prc_evt.so.1.0.2
#/cluster/etc/ap/acs/prc/conf
/usr/lib/systemd/system/acs_prcmand.service
/usr/lib/systemd/system/acs_prcispd.service
/usr/lib/systemd/system/cyclic-boot-delay.service
/usr/lib/systemd/scripts/cyclic-boot-delay.sh
/opt/ap/acs/conf/res.conf
