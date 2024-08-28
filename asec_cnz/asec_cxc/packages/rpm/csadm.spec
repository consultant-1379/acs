#
# spec file for configuration of package ACS_ASECBIN
#
# Copyright  (c)  2015  Ericsson LM
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# please send bugfixes or comments to paolo.palmieri@ericsson.com
#

Name:      %{_name}
Summary:   Installation Package for ACS_ASECBIN
Version:   %{_prNr}
Release:   %{_rel}
License:   Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group:     Application
BuildRoot: %_tmppath

Requires: APOS_OSCONFBIN
%define aseccxc_conf_path %{_cxcdir}/conf
%define aseccxc_bin_path %{_cxcdir}/bin

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build

%description
Installation Package for ACS_ASECBIN

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

cp %aseccxc_conf_path/acs_asec_csadm_template.conf  $RPM_BUILD_ROOT%ACSCONFdir/acs_asec_csadm_template.conf
cp %aseccxc_conf_path/asec_imm_classes.xml          $RPM_BUILD_ROOT%ACSCONFdir/asec_imm_classes.xml
cp %aseccxc_conf_path/asec_imm_objects.xml          $RPM_BUILD_ROOT%ACSCONFdir/asec_imm_objects.xml
cp %aseccxc_conf_path/ha_acs_asec_csadm_objects.xml $RPM_BUILD_ROOT%ACSCONFdir/ha_acs_asec_csadm_objects.xml
cp %aseccxc_conf_path/acs_asec_sshcbc.conf          $RPM_BUILD_ROOT%ACSCONFdir/acs_asec_sshcbc.conf

cp %aseccxc_bin_path/acs_asec_csadm_operations      $RPM_BUILD_ROOT%ACSBINdir/acs_asec_csadm_operations
cp %aseccxc_bin_path/acs_asec_csadmd                $RPM_BUILD_ROOT%ACSBINdir/acs_asec_csadmd
cp %aseccxc_bin_path/acs_asec_ipsec_upgrade         $RPM_BUILD_ROOT%ACSBINdir/acs_asec_ipsec_upgrade
cp %aseccxc_bin_path/csadm                          $RPM_BUILD_ROOT%ACSBINdir/csadm
cp %aseccxc_bin_path/csadm.sh                       $RPM_BUILD_ROOT%ACSBINdir/csadm.sh
cp %aseccxc_bin_path/acs_asec_csadm_clc             $RPM_BUILD_ROOT%ACSBINdir/acs_asec_csadm_clc 

cp %aseccxc_conf_path/psk.txt                       $RPM_BUILD_ROOT%ACSCONFdir/psk.txt.original
cp %aseccxc_conf_path/ikeversion                    $RPM_BUILD_ROOT%ACSCONFdir/ikeversion.original
cp %aseccxc_conf_path/connections		    $RPM_BUILD_ROOT%ACSCONFdir/connections.original
cp %aseccxc_conf_path/racoon.conf                   $RPM_BUILD_ROOT%ACSCONFdir/racoon.conf.original
cp %aseccxc_conf_path/setkey.conf                   $RPM_BUILD_ROOT%ACSCONFdir/setkey.conf.original

cp %aseccxc_bin_path/ipsec_commons                  $RPM_BUILD_ROOT%ACSBINdir/ipsec_commons
cp %aseccxc_bin_path/ipsecdef                       $RPM_BUILD_ROOT%ACSBINdir/ipsecdef
cp %aseccxc_bin_path/ipsecdef.sh                    $RPM_BUILD_ROOT%ACSBINdir/ipsecdef.sh
cp %aseccxc_bin_path/ipsecls                        $RPM_BUILD_ROOT%ACSBINdir/ipsecls
cp %aseccxc_bin_path/ipsecls.sh                     $RPM_BUILD_ROOT%ACSBINdir/ipsecls.sh
cp %aseccxc_bin_path/ipsecls_strongswan		    $RPM_BUILD_ROOT%ACSBINdir/ipsecls_strongswan
cp %aseccxc_bin_path/ipsecrm                        $RPM_BUILD_ROOT%ACSBINdir/ipsecrm
cp %aseccxc_bin_path/ipsecrm.sh                     $RPM_BUILD_ROOT%ACSBINdir/ipsecrm.sh
cp %aseccxc_bin_path/ipsecdef_racoon                $RPM_BUILD_ROOT%ACSBINdir/ipsecdef_racoon
cp %aseccxc_bin_path/ipsecdef_strongswan            $RPM_BUILD_ROOT%ACSBINdir/ipsecdef_strongswan
cp %aseccxc_bin_path/wssadm                         $RPM_BUILD_ROOT%ACSBINdir/wssadm
cp %aseccxc_bin_path/wssadm.sh                      $RPM_BUILD_ROOT%ACSBINdir/wssadm.sh

%post
if [ $1 == 1 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during installation phase"
fi
if [ $1 == 2 ]
then
        echo "This is the %{_name} package %{_rel} post-install script during upgrade phase"
fi

ln -sf  $RPM_BUILD_ROOT%ACSBINdir/csadm.sh /usr/bin/csadm


ln -sf  $RPM_BUILD_ROOT%ACSBINdir/ipsecdef.sh /usr/bin/ipsecdef
ln -sf  $RPM_BUILD_ROOT%ACSBINdir/ipsecls.sh /usr/bin/ipsecls
ln -sf  $RPM_BUILD_ROOT%ACSBINdir/ipsecrm.sh /usr/bin/ipsecrm

ln -sf  $RPM_BUILD_ROOT%ACSBINdir/wssadm.sh /usr/bin/wssadm

echo "Waiting for peer node to finalize IPSec configuration..."
/usr/bin/lockfile -30 -r 10 -l 600 /cluster/storage/system/config/acs_asec_ipsec_config.lock || exit 1

echo "Deploying IPSec configuration files"
mkdir -p $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/
if [ ! -f $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/psk.txt ]
then
	echo "Deploying psk.txt"
	cp $RPM_BUILD_ROOT%ACSCONFdir/psk.txt.original $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/psk.txt
else
	echo "Skipping deploy of psk.txt"
fi
chmod 400 $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/psk.txt

if [ ! -f $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/racoon.conf ]
then
	echo "Deploying racoon.conf"
	cp $RPM_BUILD_ROOT%ACSCONFdir/racoon.conf.original $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/racoon.conf
else
	echo "Skipping deploy of racoon.conf"
fi
chmod 444 $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/racoon.conf

if [ ! -f $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/setkey.conf ]
then
	echo "Deploying setkey.conf"
	cp $RPM_BUILD_ROOT%ACSCONFdir/setkey.conf.original $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/setkey.conf
else
	echo "Skipping deploy of setkey.conf"
fi
chmod 444 $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec/setkey.conf

if [ ! -f $RPM_BUILD_ROOT/cluster/storage/system/config/asec/sshcbc_state.conf ]
then
	echo "Deploying cipher.conf"
	cp $RPM_BUILD_ROOT%ACSCONFdir/acs_asec_sshcbc.conf $RPM_BUILD_ROOT/cluster/storage/system/config/asec/sshcbc_state.conf
else
	echo "Skipping deploy of cipher.conf"
fi
chmod 644 $RPM_BUILD_ROOT/cluster/storage/system/config/asec/sshcbc_state.conf

if [ -d $RPM_BUILD_ROOT/etc/racoon ]
then
	rm -rf $RPM_BUILD_ROOT/etc/racoon
fi
ln -sf $RPM_BUILD_ROOT/cluster/storage/system/config/asec/ipsec /etc/racoon

#if [ $1 == 2 ]
#then
#	echo "Upgrading IPSec configuration files"
#	$RPM_BUILD_ROOT%ACSBINdir/acs_asec_ipsec_upgrade || echo "Failed to upgrade IPSec configuration files"
#fi

rm -f /cluster/storage/system/config/acs_asec_ipsec_config.lock

echo "Deploying Strongswan configuration files"
mkdir -p $RPM_BUILD_ROOT/cluster/storage/system/config/asec/strongswan

if [ ! -f $RPM_BUILD_ROOT/cluster/storage/system/config/asec/strongswan/ikeversion ]
then
	echo "Deploying ikeversion"
	cp $RPM_BUILD_ROOT%ACSCONFdir/ikeversion.original $RPM_BUILD_ROOT/cluster/storage/system/config/asec/strongswan/ikeversion
	cp $RPM_BUILD_ROOT%ACSCONFdir/ikeversion.original $RPM_BUILD_ROOT/cluster/etc/ikeversion
else
	echo "Skipping deploy of ikeversion"
fi
chmod 755 $RPM_BUILD_ROOT/cluster/etc/ikeversion

if [ ! -f $RPM_BUILD_ROOT/cluster/storage/system/config/asec/strongswan/connections ]
then
        echo "Deploying connections"
        cp $RPM_BUILD_ROOT%ACSCONFdir/connections.original $RPM_BUILD_ROOT/cluster/storage/system/config/asec/strongswan/connections
else
        echo "Skipping deploy of connections"
fi
chmod 755 $RPM_BUILD_ROOT/cluster/storage/system/config/asec/strongswan/connections

%preun
if [ $1 == 0 ]
then
	echo "This is the %{_name} package %{_rel} pre-uninstall script during uninstall phase"

	rm -f /usr/bin/csadm
	rm -f /usr/bin/ipsecdef
	rm -f /usr/bin/ipsecls
	rm -f /usr/bin/ipsecrm
	rm -f /usr/bin/wssadm
fi

if [ $1 == 1 ]
then
	echo "This is the %{_name} package %{_rel} pre-uninstall script during upgrade phase"
fi

%postun
if [ $1 == 0 ]
then
	echo "This is the %{_name} package %{_rel} post-uninstall script during uninstall phase"

	rm -f %ACSCONFdir/acs_asec_csadm_template.conf
	rm -f %ACSCONFdir/acs_asec_sshcbc.conf
	rm -f %ACSCONFdir/asec_imm_classes.xml
	rm -f %ACSCONFdir/asec_imm_objects.xml
	rm -f %ACSCONFdir/ha_acs_asec_csadm_objects.xml
	rm -f %ACSBINdir/acs_asec_csadm_operations
	rm -f %ACSBINdir/acs_asec_csadmd
	rm -f %ACSBINdir/acs_asec_ipsec_upgrade
	rm -f %ACSBINdir/csadm
	rm -f %ACSBINdir/csadm.sh
	rm -f %ACSBINdir/acs_asec_csadm_clc
	rm -f %ACSCONFdir/psk.txt.original
	rm -f %ACSCONFdir/racoon.conf.original
	rm -f %ACSCONFdir/setkey.conf.original
	rm -f %ACSCONFdir/ikeversion.original
	rm -f %ACSCONFdir/connections.original

	echo "Stopping IPSec racoon daemon"
	if [ "$(/opt/ap/apos/bin/servicemgmt/servicemgmt stop racoon.service)" != "0" ]
	then
		echo "Failed to stop IPSec racoon daemon!"
	else
		echo "IPSec racoon daemon stopped"
	fi
	setkey -DPF

	if [ -d $RPM_BUILD_ROOT/etc/racoon ]
	then
		rm -rf $RPM_BUILD_ROOT/etc/racoon
	fi
	echo "Stopping IPSec strongswan daemon"
	if [ "$(/opt/ap/apos/bin/servicemgmt/servicemgmt stop strongswan.service)" != "0" ]
	then
		echo "Failed to stop IPSec strongswan daemon!"
	else
		echo "IPSec strongswan daemon stopped"
	fi

	if [ -d $RPM_BUILD_ROOT/etc/ipsec.d ]
	then
		rm -rf $RPM_BUILD_ROOT/etc/ipsec.d
	fi

fi

if [ $1 == 1 ]
then
	echo "This is the %{_name} package %{_rel} post-uninstall script during upgrade phase"
fi


%files
%defattr(-,root,root)
%attr(0755,root,root) %ACSCONFdir/acs_asec_csadm_template.conf
%attr(0755,root,root) %ACSCONFdir/asec_imm_classes.xml
%attr(0755,root,root) %ACSCONFdir/asec_imm_objects.xml
%attr(0755,root,root) %ACSCONFdir/ha_acs_asec_csadm_objects.xml
%attr(0755,root,root) %ACSCONFdir/acs_asec_sshcbc.conf
%attr(0755,root,root) %ACSBINdir/acs_asec_csadm_operations
%attr(0755,root,root) %ACSBINdir/acs_asec_csadmd
%attr(0755,root,root) %ACSBINdir/acs_asec_ipsec_upgrade
%attr(0755,root,root) %ACSBINdir/csadm
%attr(0755,root,root) %ACSBINdir/csadm.sh
%attr(0755,root,root) %ACSBINdir/acs_asec_csadm_clc

%attr(0755,root,root) %ACSBINdir/ipsec_commons
%attr(0755,root,root) %ACSBINdir/ipsecdef
%attr(0755,root,root) %ACSBINdir/ipsecdef.sh
%attr(0755,root,root) %ACSBINdir/ipsecdef_racoon
%attr(0755,root,root) %ACSBINdir/ipsecdef_strongswan
%attr(0755,root,root) %ACSBINdir/ipsecls_strongswan
%attr(0755,root,root) %ACSBINdir/ipsecls
%attr(0755,root,root) %ACSBINdir/ipsecls.sh
%attr(0755,root,root) %ACSBINdir/ipsecrm
%attr(0755,root,root) %ACSBINdir/ipsecrm.sh
%attr(0400,root,root) %ACSCONFdir/psk.txt.original
%attr(0444,root,root) %ACSCONFdir/racoon.conf.original
%attr(0444,root,root) %ACSCONFdir/setkey.conf.original
%attr(0755,root,root) %ACSCONFdir/ikeversion.original
%attr(0755,root,root) %ACSCONFdir/connections.original
%attr(0755,root,root) %ACSBINdir/wssadm
%attr(0755,root,root) %ACSBINdir/wssadm.sh

%changelog
* Fri Jun  07 2019 - rajeshwari (at) tcs.com
- ANSSI MMF4 feature
* Wed Jan  23 2019 - pardhasaradhi (at) tcs.com
- Added Strongswan configuraiton files
* Tue Mar  13 2015 - furquan.ullah (at) tcs.com
- Added sshcbc related files to the implementation
* Fri Oct  3  2014 - fabrizio.paglia (at) dektech.com.au
- Added wssadm to the implementation
* Wed Sep 24  2014 - fabrizio.paglia (at) dektech.com.au
- Improved handling of concurrent access to /cluster,
  in order to avoid issues during upgrade from Rel. 1 (TR HS98027)
* Thu May 22  2014 - fabrizio.paglia (at) dektech.com.au
- Fixed to avoid concurrent access to /cluster during restore.
- Added script to upgrade the format of IPSec configuration files.
* Thu Oct 02  2013 - fabrizio.paglia (at) dektech.com.au
- Fixed racoon restart after installation and stop after uninstallation.
- Fixed handling of racoon configuration files during uninstallation.
* Wed Oct 01  2013 - alessandro.mortari (at) dektech.com.au
- Fixed overwriting of racoon cnfiguration files. Overwrite is done only if files do not exist
* Wed Sep 25  2013 - fabrizio.paglia (at) dektech.com.au
- Added IPSec functionality to the implementation
* Fri Nov 28  2010 - sonali nanda (at) tcs.com
- Added csadm functionality to the implementation
-

