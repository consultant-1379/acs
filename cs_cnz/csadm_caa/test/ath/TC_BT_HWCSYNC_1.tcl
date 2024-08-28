#!/bin/csh
#\
exec ath $0

# The line(s) above enable this script to be executed from a file manager
# on Unix. They must end with LF (not by CRLF).

###########################################################################
# This is a Tcl/Expect test case script

###########################################################################
# Copyright 2009 Ericsson AB
# All rights reserved.

###########################################################################
# :Title        Command hwcsync
#         
# -------------------------------------------------------------------------
# :TestAct      FT        # One of FT, ST, BT, IT,....
# :SubSys       ACS       # Mandatory for FT, optional (leave empty) if ST
# :TestObj      FCH       # Block name if FT. Rob, Char, Stab or Maint if ST
# :Prepared     EAB/FTE/RT Name of script author
# :Approved     EAB/FTE/RT (My Boss)
# :Date         yyyy-mm-dd
# :Req          <comma-separated list of requirements tested>    # Optional
# :Ref          <Optional reference to a TC in an external TS or TI>
#               # Example: Char TS 2/15241-14/FCPW10197Uen PA2, TC 6.5.1.1.
# :TCLTCS       nnnnnnnn  # Optional - ref. to TCL2000 TCS
# :TCLTCI       nnnnnnnn  # Optional - ref. to TCL2000 TCI
# :RegTest      Yes/No    # Recommended for regression test
# :Scope        <Design Based, Requirement Based or Miscellanexpectedeous>
# :OS           Linux
# :APZ          Yes/No or comma-separated: 2123x, 21240
# :NrOfAPGs     1         # The number of APGs used in target
# :NrOfBCs      0         # The number of billing centre computers used in target


###########################################################################
# :Revision Information
# ---------------------
# When    By       Description
# yymmdd  uabxxxx  ...


###########################################################################
# PRECONDITIONS:
# CS is installed
###########################################################################

###########################################################################
# :Test Case Specification
# ------------------------
# Test hwcsync command
#

package require ATH

#**************************************************************************
ath_display "Preparations"
#**************************************************************************

print "\n\nACTION: Check that test plant specific variables have been set in the config file.\n\n"
# If not, prompt for variable values.
ath_check_parameter "SUT::APG1"      "Enter the name or IP-address of APG1: "
ath_check_parameter "SUT::APG1A"     "Enter the name or IP-address of APG1 node A: "
ath_check_parameter "SUT::APG1B"     "Enter the name or IP-address of APG1 node B: "
ath_check_parameter "SUT::APG1_USER" "Enter the user ID for APG1: "
ath_check_parameter "SUT::APG1_PW"   "Enter the password for APG1: "
ath_check_parameter "SUT::PROTOCOL" "Telnet or SSH: "

#**************************************************************************
ath_display "Execution"
#**************************************************************************
append proc_file $ATH::SCRIPTPATH "CS_Common.lib"
ath_source $proc_file

print "\n\nACTION: Connect a Telnet or SSH terminal session to the SUT\n\n"
cs_sut_connect

# Save the value of spawn_id that is associated with the spawned telnet process,
# just in case we'll swap between different spawned processes (e.g. telnet
# sessions) in this test case.
set TelnetSession1 $spawn_id

AutoFail OFF
 
#define all data needed for HWC table ########################################

set Mag "2.4.0.5"
set SlotSCB "0"
set SlotCPU "10"
set MAC_A "00:01:02:03:04:1f"
set MAC_B "00:01:02:03:04:20"
set NewMAC_A "05:04:03:02:01:01"
set NewMAC_B "05:04:03:02:01:02"
set SCB_RP_TEST_HOSTNAME "scb_rp_for_test"

#Patterns for hwcls for all cards to be added

set PatternSCB ":
HARDWARE CONFIGURATION TABLE
:
$Mag $SlotSCB * * SCB-RP * *
:"

set PatternCPU ":
HARDWARE CONFIGURATION TABLE
:
$Mag $SlotCPU CP * CPUB **
:"

set DHCPPatternSCB "host [cs_get_dhcp_host_name $Mag $SlotSCB "A"] {
  dynamic;
  uid **
  fixed-address **
}
host [cs_get_dhcp_host_name $Mag $SlotSCB "B"] {"


set DHCPPatternCPU "host [cs_get_dhcp_host_name $Mag $SlotCPU "A"] {
  dynamic;
  hardware ethernet $MAC_A**
**
**
**
}
host [cs_get_dhcp_host_name $Mag $SlotCPU "B"] {
  dynamic;
  hardware ethernet $MAC_B**"


set DHCPPatternCPUNewMAC "host [cs_get_dhcp_host_name $Mag $SlotCPU "A"] {
  dynamic;
  hardware ethernet $NewMAC_A**
**
**
**
}
**
**
**
**
host [cs_get_dhcp_host_name $Mag $SlotCPU "B"] {
  dynamic;
  hardware ethernet $NewMAC_B**"



set DHCPPatternDeletedCPUA "host [cs_get_dhcp_host_name $Mag $SlotCPU "A"] {
  dynamic;
  deleted;
}"


set DHCPPatternDeletedCPUB "host [cs_get_dhcp_host_name $Mag $SlotCPU "B"] {
  dynamic;
  deleted;
}"

# end define all data needed for HWC table ########################################


# PREPARATION #####################################################################

# Call preparation routine
cs_preparation

# Start daemon
cs_start_service

# Update hostfile with SCB-RP address
cs_update_host_file $SCB_RP_TEST_HOSTNAME $SUT::SCB_RP_ADDRESS

# PREPARATION  END ################################################################

# Verify that all needed commands are available
cs_command_must_exist "hwcadd"
cs_command_must_exist "hwcls"
cs_command_must_exist "hwcrm"
cs_command_must_exist "hwcsync -x"

set target_modified 1;  # Inform the CleanUp procedure that it has to clean up

###########################################################################
# Testcase start
###########################################################################

ath_send "grep --silent $SCB_RP_TEST_HOSTNAME /etc/hosts"

if {$ATH::ReturnCode != 0} {
# Missing hostname for the simulated SCB-RP
	ath_tc_failed "SCB-RP hostname not set"
}

# Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#Verify the dhcpd.leases file contains no leases. Otherwise fail.
set exp_num_leases 0
set lease_pattern "host **"
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $lease_pattern $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "Wrong number of leases in dhcpd.leases."
}


#Add SCB-RP board to the HW table with dhcp client
if {[ath_send "hwcadd -m $Mag -s $SlotSCB -n SCB-RP -d client"]} {
    ath_tc_failed "hwcadd: addition of SCB-RP board failed"
}


#Wait 5 seconds 
print "\nWaiting 5 seconds... \n"
after 5000


#Verify that SCB-RP board is present in HWC table
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternSCB $LastResponse]} {
    ath_tc_failed "hwcls: SCB-RP board is missing in mag: $Mag, slot $SlotSCB"
}


#Add CPU board to the HWC table with dhcp set to normal
if {[ath_send "hwcadd -m $Mag -s $SlotCPU -n CPUB -t CP -y 1 -e B -d normal"]} {
    ath_tc_failed "hwcadd: addition of CPU board failed"
}


#Wait 5 seconds 
print "\nWaiting 5 seconds... \n"
after 5000


#Verify that CPU board is present in HWC table
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPU $LastResponse]} {
    ath_tc_failed "hwcls: CPU board is missing in mag: $Mag, slot $SlotCPU \n"
}


#Verify the dhcpd.leases file has been updated and contains SCB-RP entry both for A and B backplane. Otherwise fail.
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternSCB $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of leases found for SCB-RP board mag: $Mag, slot $SlotSCB"
}


#Verify the dhcpd.leases file has been updated and contains CPU entry both for A and B backplane with old MAC addresses. Otherwise fail.
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternCPU $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of leases found for CPU board with old MAC addresses; mag: $Mag, slot $SlotCPU"
}


# Verify that no entries have been deleted
set exp_num_leases 0
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternDeletedCPUA $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of removed leases found for CPU board mag: $Mag, slot $SlotCPU"
}


set exp_num_leases 0
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternDeletedCPUB $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of removed leases found for CPU board mag: $Mag, slot $SlotCPU"
}


# Change MAC address in SNMP agent configuration file
cs_change_mac_in_snmpd_conf


# Run hwcsync
if {[ath_send "hwcsync"]} {
    ath_tc_failed "hwcsync failed"
}


#Wait 5 seconds 
print "\nWaiting 5 seconds... \n"
after 5000


#Verify the dhcpd.leases file has been updated and contains CPU entry both for A and B backplane with old MAC addresses. Otherwise fail.
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternCPU $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of leases found for CPU board; mag: $Mag, slot $SlotCPU"
}


#Verify that old MACs entries have been removed
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternDeletedCPUA $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of removed leases found for CPUA board mag: $Mag, slot $SlotCPU"
}
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternDeletedCPUB $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of removed leases found for CPUB board mag: $Mag, slot $SlotCPU"
}


# And that the new MACs are present
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternCPUNewMAC $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of leases found for CPU board; mag: $Mag, slot $SlotCPU"
}


#Check and stop CS service
print "\nChecking and stopping CS service...\n"
cs_check_and_stop_service


print "\n\nACTION: Disconnect the terminal session.\n\n"
ath_send "exit"


#**************************************************************************
# Report Test Case Result
#**************************************************************************

print "\n\nACTION: Report test case result \"PASSED\".\n\n"
ath_tc_passed


#**************************************************************************
# :CleanUp
#**************************************************************************
# The CleanUp procedure below is called by RunScript after this script has
# been executed, or after this script has been terminated by a call to
# 'exit' or an unhandled exception.
proc CleanUp args {
    global target_modified; # Allows us to directly access this global variable

    print "Cleaning up...\n"

    # Clean up the target only if any exchange data has been changed
    if {[info exists target_modified]} { # To avoid runtime error if the variable
					     # wasn't declared before this procedure
					     # was called.
        if {$target_modified} {
    	  print "\n\nACTION: Connect a Telnet or SSH terminal session to the SUT\n\n"
          cs_sut_connect
    
    	# Clean-up:
	    cs_cleanup
	    cs_restore_snmpd_conf
    	ath_send "exit"
        }
    }
    print "\n\nClean up done.\n"
}


###########################################################################
# :References
###########################################################################
# References to technical documents, such as IPs, RSs, technical
# reports, CODs,ath_tc_failed "BREAK!" PODs, manpages etc.
# No need to refer to any project planning documents such as test plans.
#
# [1] docname
#     docno
# [2] docname
#     docno

