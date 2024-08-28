%define    _use_internal_dependency_generator 0

%define cs_cxc_path %{_cxcdir}
%define  __find_provides  %cs_cxc_path/packages/rpm/filter-provides.sh
%define  __find_requires  %cs_cxc_path/packages/rpm/filter-requires.sh

Name: %_name
Summary: Installation package for CS Service
Version: %_prNr
Release: %_rel
License: Ericsson Proprietary
Vendor:    Ericsson LM
Packager:  %packer
Group: Application
BuildRoot: %_tmppath
AutoReq: no
Requires: APOS_OSCONFBIN

%define _CS_LIB_NAME libacs_csapi.so.3.14.2
%define _CS_LIB_LINKNAME libacs_csapi.so.3
%define _CS_LIB_SONAME libacs_csapi.so


%description
Installation package for CS Service

%install
echo " This is the CS package install section"

mkdir -p $RPM_BUILD_ROOT%rootdir
mkdir -p $RPM_BUILD_ROOT%APDir
mkdir -p $RPM_BUILD_ROOT%ACSdir
mkdir -p $RPM_BUILD_ROOT%ACSLIB64dir
mkdir -p $RPM_BUILD_ROOT%ACSBINdir
mkdir -p $RPM_BUILD_ROOT%ACSdir/conf
mkdir -p $RPM_BUILD_ROOT%ACSdir/conf/si

cp %cs_cxc_path/bin/lib_ext/%_CS_LIB_NAME	$RPM_BUILD_ROOT%ACSLIB64dir
cp -af %cs_cxc_path/bin/acs_csd			$RPM_BUILD_ROOT%ACSBINdir
cp -af %cs_cxc_path/conf/c_AxeEquipment_imm_classes.xml	$RPM_BUILD_ROOT%ACSdir/conf
cp -af %cs_cxc_path/conf/o_AxeEquipmentInstances_imm_objects.xml	$RPM_BUILD_ROOT%ACSdir/conf
cp -af %cs_cxc_path/conf/AxeEquipment_mp.xml $RPM_BUILD_ROOT%ACSdir/conf

cp -af %cs_cxc_path/conf/c_AxeFunctionDistribution_imm_classes.xml  $RPM_BUILD_ROOT%ACSdir/conf
cp -af %cs_cxc_path/conf/o_AxeFunctionDistributionInstance_imm_objects.xml $RPM_BUILD_ROOT%ACSdir/conf
cp -af %cs_cxc_path/conf/AxeFunctionDistribution_mp.xml $RPM_BUILD_ROOT%ACSdir/conf

cp -af %cs_cxc_path/conf/ha_acs_cs_objects.xml    $RPM_BUILD_ROOT%ACSdir/conf

cp -af %cs_cxc_path/conf/si/apz_smart_image_exec.sh    $RPM_BUILD_ROOT%ACSdir/conf/si/apz_smart_image_exec.sh
cp -af %cs_cxc_path/conf/si/apz_smart_image_list.cfg    $RPM_BUILD_ROOT%ACSdir/conf/si/apz_smart_image_list.cfg
cp -af %cs_cxc_path/conf/si/apz_smart_image_netinfo_vmware.sh    $RPM_BUILD_ROOT%ACSdir/conf/si/apz_smart_image_netinfo_vmware.sh
cp -af %cs_cxc_path/conf/si/apz_smart_image_netinfo_openstack.sh    $RPM_BUILD_ROOT%ACSdir/conf/si/apz_smart_image_netinfo_openstack.sh

cp %cs_cxc_path/bin/hwcls		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/hwcls.sh		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/opensession		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/opensession.sh		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/cpls		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/cpls.sh		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/cpgls		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/cpgls.sh		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/script/acs_cs_clc   $RPM_BUILD_ROOT%ACSBINdir

cp %cs_cxc_path/bin/cs_vlan_conf.sh		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/cs_hidemodel.sh		$RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/cs_subscription.sh       $RPM_BUILD_ROOT%ACSBINdir
cp %cs_cxc_path/bin/cs_transportdn.sh	$RPM_BUILD_ROOT%ACSBINdir

%clean
rm -rf $RPM_BUILD_ROOT/*

%pre
echo " This is the CS package preinstall section"


%post
is_vm=""
if [ -f /cluster/storage/system/config/lde/csm/templates/config/initial/ldews.os/factoryparam.conf ];  then
  is_vm=$(cat /cluster/storage/system/config/lde/csm/templates/config/initial/ldews.os/factoryparam.conf | grep -i installation_hw | awk -F "=" '{print $2}')
fi
if [ $1 == 1 ]
then
	echo " This is the CS package post install section during installation phase"
	%ACSBINdir/cs_vlan_conf.sh
        if [ "$is_vm" != "VM" ];  then
	  %ACSBINdir/cs_hidemodel.sh
        fi
fi

if [ $1 == 2 ]
then
	echo " This is the CS package post install section during upgrade phase"
	%ACSBINdir/cs_transportdn.sh
    	%ACSBINdir/cs_vlan_conf.sh u
        if [ "$is_vm" != "VM" ];  then
	  %ACSBINdir/cs_hidemodel.sh
        fi
fi

echo " Creating CS links in %_lib64dir"
ln -sf %ACSLIB64dir/%_CS_LIB_NAME %_lib64dir/%_CS_LIB_LINKNAME
ln -sf %_lib64dir/%_CS_LIB_LINKNAME %_lib64dir/%_CS_LIB_SONAME

echo " Creating CS links in %_bindir"
ln -sf %ACSBINdir/acs_csd %_bindir/acs_csd
ln -sf %ACSBINdir/hwcls.sh %_bindir/hwcls
ln -sf %ACSBINdir/cpls.sh %_bindir/cpls
ln -sf %ACSBINdir/cpgls.sh %_bindir/cpgls
ln -sf %ACSBINdir/opensession.sh %_bindir/opensession
ln -sf %ACSBINdir/acs_cs_clc %_bindir/acs_cs_clc
chmod +xs %ACSBINdir/opensession
chmod +rx %ACSBINdir/opensession.sh

if ! grep -qP '^[[:space:]]*ExecStartPost\=\-\/opt\/ap\/acs\/bin\/cs_subscription\.sh$' /usr/lib/systemd/system/lde-iptables.service
then
  echo "subscription Iptables service restart"
  /opt/ap/apos/bin/servicemgmt/servicemgmt subscribe "lde-iptables.service" "StartPost" %ACSBINdir/cs_subscription.sh || echo "Failure while subscribing to lde-iptables.service."
fi

echo " Done with CS post install activities"

%preun
if [ $1 == 0 ]
then
	echo " This is the CS package pre uninstall section during uninstallation phase"
	echo " Removing CS links in %_lib64dir"
	rm -f %_lib64dir/%_CS_LIB_LINKNAME
	rm -f %_lib64dir/%_CS_LIB_SONAME
	
	echo " Removing CS links in %_bindir"
	rm -f %_bindir/acs_csd
	rm -f %_bindir/hwcls
	rm -f %_bindir/cpls
	rm -f %_bindir/cpgls
	rm -f %_bindir/opensession
	rm -f %_bindir/acs_cs_clc
fi
if [ $1 == 1 ]
then
    echo "This is the CS package pre uninstall section during upgrade phase"
fi


%postun
if [ $1 == 0 ]
then
	echo " This is the CS package post uninstall section during uninstall phase"

	echo " Removing CS library in %ACSLIB64dir"
	rm -f %ACSLIB64dir/%_CS_LIB_NAME
	
	echo " Removing CS binary in %ACSBINdir"
	rm -f %ACSBINdir/acs_csd
	rm -f %ACSBINdir/hwcls.sh
	rm -f %ACSBINdir/hwcls
	rm -f %ACSBINdir/cpls.sh
	rm -f %ACSBINdir/cpls
	rm -f %ACSBINdir/cpgls.sh
	rm -f %ACSBINdir/cpgls
	rm -f %ACSBINdir/opensession.sh
	rm -f %ACSBINdir/opensession
	rm -f %ACSBINdir/acs_cs_clc
	rm -f %ACSBINdir/cs_vlan_conf.sh
	rm -f %ACSBINdir/cs_hidemodel.sh
	rm -f %ACSBINdir/cs_subscription.sh
	rm -f %ACSBINdir/cs_transportdn.sh
fi
if [ $1 == 1 ]
then
    echo " This is the CS package post uninstall section during upgrade phase"
fi


echo " Done with CS post uninstall activities"

%files
%defattr(-,root,root,-)
%ACSLIB64dir/%_CS_LIB_NAME
%ACSBINdir/acs_csd
%ACSdir/conf/c_AxeEquipment_imm_classes.xml
%ACSdir/conf/o_AxeEquipmentInstances_imm_objects.xml
%ACSdir/conf/AxeEquipment_mp.xml

%ACSdir/conf/c_AxeFunctionDistribution_imm_classes.xml
%ACSdir/conf/o_AxeFunctionDistributionInstance_imm_objects.xml
%ACSdir/conf/AxeFunctionDistribution_mp.xml

%ACSdir/conf/ha_acs_cs_objects.xml

%ACSdir/conf/si/apz_smart_image_exec.sh
%ACSdir/conf/si/apz_smart_image_list.cfg
%ACSdir/conf/si/apz_smart_image_netinfo_vmware.sh
%ACSdir/conf/si/apz_smart_image_netinfo_openstack.sh

%ACSBINdir/hwcls
%ACSBINdir/hwcls.sh
%ACSBINdir/cpls
%ACSBINdir/cpls.sh
%ACSBINdir/cpgls
%ACSBINdir/cpgls.sh
%ACSBINdir/opensession
%ACSBINdir/opensession.sh
%ACSBINdir/acs_cs_clc
%ACSBINdir/cs_vlan_conf.sh
%ACSBINdir/cs_hidemodel.sh
%ACSBINdir/cs_subscription.sh
%ACSBINdir/cs_transportdn.sh


%changelog
* Thu Sep 02 2020 - sankara.jayanth (at) tcs.com
- Changed csapi library version for TR HY60399
* Thu Apr 18 2019 - sankara.jayanth (at) tcs.com
- Changed csapi library version for TR HX45316
* Wed May 16 2018 - mahitha.mandalapu (at) tcs.com
- Changed a result code for IPLB
* Fri Apr 27 2018 - mahitha.mandalapu (at) tcs.com
- Changed the csapi library version for TR HW53952
* Mon Dec 04 2017 - mahitha.mandalapu (at) tcs.com 
- Changed the csapi library version
* Wed Jul 19 2017 - harika.bavana (at) ericsson.com
- Added cs_transportdn.sh for TR HV90938
* Tue Jan 17 2017 - antonio.buonocunto (at) ericsson.com
- New handling for smart image 
* Tue Mar 1 2016 - andrea formica (at) tei.eri CXC1371495_5-R1C
- Added iptables subscription at service restart
* Tue Mar 1 2016 - claudio.elefante (at) itslab.it CXC1371495_5-R1A05
- Added template.xml file for Smart Image
* Mon Nov 30 2015 - rajeshwari.p (at) tcs.com CXC1371495_5-R1A02
- Changed the csapi library version
* Tue Sep 15 2015 - sabyasachi.jha (at) tcs.com CXC1371532-R1A08
- Added cs_hidemodel.sh file and script execution during installation phase
* Tue Sep 10 2013 - stefano.volpe (at) tei.eri CXC1371432-R1A29
- Added cs_vlan_conf.sh file and script execution during installation phase
* Wed Aug 21 2013 - andrea.formica (at) tei.eri CXC1371432-R1A28
- Added cpgls and cpgls files
* Fri May 10 2013 - andrea.formica (at) tei.eri CXC1371432-R1A18
- Added AxeFunctionDistribution model file
* Mon Apr 17 2013 - giuseppe.fusco (at) tei.eri CXC1371432-R1A16
- Added opensession.sh script file
* Mon Oct 17 2012 - stefano.volpe (at) tei.eri CXC1371330-R1E05
- Added hwcls.sh script file
* Mon May 9 2012 - magnus.lexhagen (at) cybercomgroup.com CXC1371330-R1C01
- Changed name on model xml files, removed hwcadd
* Wed Nov 9 2011 - magnus.lexhagen (at) cybercomgroup.com CXC1371330-P1B02
- Changed name on model xml files
* Fri Nov 4 2011 - magnus.lexhagen (at) cybercomgroup.com CXC1371330-P1B02
- Added new CS IMM functions library
* Fri Feb 18 2011 <piotr.krysicki@cybercomgroup.com> CXC1371330-R1A04
- IMM conf files permanently installed into conf dir
* Mon Jan 7 2011 - magnus.lexhagen (at) cybercomgroup.com CXC1371330-P1A03
- Changes according to new rpm design rules
* Wed Sep 21 2010  <piotr.krysicki@cybercomgroup.com> 
- csapi rpm
