create_directories() {
   echo "Creating Directories"
   /bin/mkdir -p /data/acs/data
   /bin/mkdir -p /data/acs/data/chb
   /bin/mkdir -p /data/acs/data/alec
   /bin/mkdir -p /data/acs/data/mtz
   /bin/mkdir -p /data/acs/data/rtr
   /bin/mkdir -p /data/acs/data/ssu
   /bin/mkdir -p /data/acs/data/aca
   /bin/mkdir -p /data/ftpvol
   /bin/mkdir -p /data/images
   /bin/mkdir -p /data/acs/logs
   /bin/mkdir -p /data/aes
   /bin/mkdir -p /data/mas
   /bin/mkdir -p /data/mscs
   /bin/mkdir -p /data/sts
   /bin/mkdir -p /data/sgs
   /bin/mkdir -p /data/mcs
   /bin/mkdir -p /data/ocs
   /bin/mkdir -p /data/fms
   /bin/mkdir -p /data/apz
   /bin/mkdir -p /data/cps
   /bin/mkdir -p /data/cqs
   /bin/mkdir -p /data/ipt
   /bin/mkdir -p /data/ipt/data
   /bin/mkdir -p /data/ipt/logs
}


add_groups() {
   /usr/sbin/groupadd CHBUSRGRP
   /usr/sbin/groupadd ALECUSRGRP
   /usr/sbin/groupadd ACSUSRGRP
   /usr/sbin/groupadd MTZUSRGRP
   /usr/sbin/groupadd RTRUSRGRP
   /usr/sbin/groupadd SSUUSRGRP
   /usr/sbin/groupadd ACAUSRGRP
   /usr/sbin/groupadd STSUSRGRP
   /usr/sbin/groupadd SGSUSRGRP
   /usr/sbin/groupadd OCSUSRGRP
   /usr/sbin/groupadd MSCSUSRGRP
   /usr/sbin/groupadd MCSUSRGRP
   /usr/sbin/groupadd MASUSRGRP
   /usr/sbin/groupadd FMSUSRGRP
   /usr/sbin/groupadd CQSUSRGRP
   /usr/sbin/groupadd CPSUSRGRP
   /usr/sbin/groupadd APZUSRGRP
   /usr/sbin/groupadd AESUSRGRP
   /usr/sbin/groupadd IMAGESUSRGRP
   /usr/sbin/groupadd FTPVOLUSRGRP
   /usr/sbin/groupadd LOGSUSRGRP
   /usr/sbin/groupadd IPTUSRGRP
   /usr/sbin/groupadd IPTDATAUSRGRP
   /usr/sbin/groupadd IPTLOGSUSRGRP

}
setgroup_own(){

   /bin/chown -R root.ACSUSRGRP /data/acs/data
   /bin/chown -R root.FTPVOLUSRGRP /data/ftpvol
   /bin/chown -R root.CHBUSRGRP /data/acs/data/chb
   /bin/chown -R root.ALECUSRGRP /data/acs/data/alec
   /bin/chown -R root.MTZUSRGRP /data/acs/data/mtz
   /bin/chown -R root.RTRUSRGRP /data/acs/data/rtr
   /bin/chown -R root.SSUUSRGRP /data/acs/data/ssu
   /bin/chown -R root.ACAUSRGRP /data/acs/data/aca
   /bin/chown -R root.STSUSRGRP /data/sts
   /bin/chown -R root.SGSUSRGRP /data/sgs
   /bin/chown -R root.OCSUSRGRP /data/ocs
   /bin/chown -R root.MSCSUSRGRP /data/mscs
   /bin/chown -R root.MCSUSRGRP /data/mcs
   /bin/chown -R root.MASUSRGRP /data/mas
   /bin/chown -R root.FMSUSRGRP /data/fms
   /bin/chown -R root.CQSUSRGRP /data/cqs
   /bin/chown -R root.CPSUSRGRP /data/cps
   /bin/chown -R root.APZUSRGRP /data/apz
   /bin/chown -R root.AESUSRGRP /data/aes
   /bin/chown -R root.LOGSUSRGRP /data/acs/logs
   /bin/chown -R root.IMAGESUSRGRP /data/images
   /bin/chown -R root.IPTUSRGRP /data/ipt/data
   /bin/chown -R root.IPTDATAUSRGRP /data/ipt/data
   /bin/chown -R root.IPTLOGSUSRGRP /data/ipt/logs
   /bin/chmod 2775 /data/acs/data/chb
   /bin/chmod 2775 /data/acs/data/alec
   /bin/chmod 2775 /data/acs/data/mtz
   /bin/chmod 2775 /data/acs/data/rtr
   /bin/chmod 2775 /data/acs/data/ssu
   /bin/chmod 2775 /data/acs/data/aca
   /bin/chmod 2775 /data/sts
   /bin/chmod 2775 /data/sgs
   /bin/chmod 2775 /data/ocs
   /bin/chmod 2775 /data/mscs
   /bin/chmod 2775 /data/mcs
   /bin/chmod 2775 /data/mas
   /bin/chmod 2775 /data/fms
   /bin/chmod 2775 /data/cqs
   /bin/chmod 2775 /data/cps
   /bin/chmod 2775 /data/apz
   /bin/chmod 2775 /data/aes
   /bin/chmod 2775 /data/acs/data
   /bin/chmod 2775 /data/ftpvol
   /bin/chmod 2775 /data/images
   /bin/chmod 2775 /data/acs/logs
   /bin/chmod 2775 /data/ipt/
   /bin/chmod 2775 /data/ipt/data
   /bin/chmod 2775 /data/ipt/logs
}




config_quota() {
   echo "Start config Quota..."
   create_directories
   add_groups
   setgroup_own
   echo "Quota Config Complete"
}

config_quota

