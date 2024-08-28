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
# :Title        DHCP configuration       
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
# :Scope        <Design Based, Requirement Based or Miscellaneous>
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
# Test the proper DHCP configuration by the standalone test application
#

package require ATH

#**************************************************************************
ath_display "Preparations"
#**************************************************************************
append proc_file $ATH::SCRIPTPATH "CS_Common.lib"
ath_source $proc_file

print "\n\nACTION: Check that test plant specific variables have been set in the config file.\n\n"
# If not, prompt for variable values.
ath_check_parameter "SUT::APG1"      "Enter the name or IP-address of APG1: "
ath_check_parameter "SUT::APG1A"     "Enter the name or IP-address of APG1 node A: "
ath_check_parameter "SUT::APG1B"     "Enter the name or IP-address of APG1 node B: "
ath_check_parameter "SUT::APG1_USER" "Enter the user ID for APG1: "
ath_check_parameter "SUT::APG1_PW"   "Enter the password for APG1: "
ath_check_parameter "SUT::PROTOCOL" "Telnet or SSH: "
ath_check_parameter "SUT::SCB_RP_ADDRESS" "Enter the IP address for the simulated SCB-RP board: "


#**************************************************************************
ath_display "Execution"
#**************************************************************************

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
set SlotAPU "10"
set SlotCPU "1"
set SCB_RP_TEST_HOSTNAME "scb_rp_for_test"

#Patterns for hwcls for all cards to be added
set PatternSCB ":
HARDWARE CONFIGURATION TABLE
:
$Mag $SlotSCB * * SCB-RP * *
:"
set PatternAPU ":
HARDWARE CONFIGURATION TABLE
:
$Mag $SlotAPU AP * APUB **
:"
set PatternCPU ":
HARDWARE CONFIGURATION TABLE
:
$Mag $SlotCPU CP * CPUB **
:"

set DHCPPatternSCB "host [cs_get_dhcp_host_name $Mag $SlotSCB "A"] {
  dynamic;
**
**
}
host [cs_get_dhcp_host_name $Mag $SlotSCB "B"] {"

set DHCPPatternAPU "host [cs_get_dhcp_host_name $Mag $SlotAPU "A"] **"

set DHCPPatternCPU "host [cs_get_dhcp_host_name $Mag $SlotCPU "A"] {
  dynamic;
**
**
**
**
}
host [cs_get_dhcp_host_name $Mag $SlotCPU "B"] {"

set DHCPPatternDeletedSCB "host [cs_get_dhcp_host_name $Mag $SlotSCB "A"] {
  dynamic;
  deleted;
}
host [cs_get_dhcp_host_name $Mag $SlotSCB "B"] {
  dynamic;
  deleted;
}"

set DHCPPatternDeletedAPU "host [cs_get_dhcp_host_name $Mag $SlotAPU "A"] {
  dynamic;
  deleted;
}
host [cs_get_dhcp_host_name $Mag $SlotAPU "B"] {
  dynamic;
  deleted;
}"

set DHCPPatternDeletedCPU "host [cs_get_dhcp_host_name $Mag $SlotCPU "A"] {
  dynamic;
  deleted;
}
host [cs_get_dhcp_host_name $Mag $SlotCPU "B"] {
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

set target_modified 1;  # Inform the CleanUp procedure that it has to clean up

###########################################################################
# Testcase start
###########################################################################

# Verify /etc/hosts file
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


#Add SCB-RP board to the HWC table with dhcp client
if {[ath_send "hwcadd -m $Mag -s $SlotSCB -n SCB-RP -d client"]} {
    ath_tc_failed "hwcadd: addition of SCB-RP board failed"
}


#Verify that SCB-RP board is present in HWC table
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternSCB $LastResponse]} {
    ath_tc_failed "hwcls: SCB-RP board is missing in mag: $Mag, slot $SlotSCB"
}


#Wait 5 seconds 
print "\nWaiting 5 seconds... \n"
after 5000


#Verify the dhcpd.leases file contains SCB-RP entry both for A and B backplane. Otherwise fail.
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternSCB $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of leases found for SCB-RP board mag: $Mag, slot $SlotSCB"
}


#Add APU board to the HWC table without specifying dhcp
if {[ath_send "hwcadd -m $Mag -s $SlotAPU -n APUB -t AP -y 1 -e B"]} {
    ath_tc_failed "hwcadd: addition of APU board failed"
}


#Verify that APU board is present in HWC table
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternAPU $LastResponse]} {
   ath_tc_failed "hwcls: APU board is missing in mag: $Mag, slot $SlotAPU \n"
}


#Wait 5 seconds 
print "\nWaiting 5 seconds... \n"
after 5000


#Verify the dhcpd.leases file contains no APU entry. Otherwise fail.
set exp_num_leases 0
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternAPU $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of leases found for APU board mag: $Mag, slot $SlotAPU"
}


#Add CPU board to the HWC table with dhcp normal
if {[ath_send "hwcadd -m $Mag -s $SlotCPU -n CPUB -t CP -y 1 -e B -d normal"]} {
    ath_tc_failed "hwcadd: addition of CP board failed"
}


#Verify that CP board is present in HWC table
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPU $LastResponse]} {
    ath_tc_failed "hwcls: CP board is missing in mag: $Mag, slot $SlotSCB"
}


#Wait 5 seconds 
print "\nWaiting 5 seconds... \n"
after 5000


#Verify the dhcpd.leases file contains CP entry both for A and B backplane. Otherwise fail.
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternCPU $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of leases found for CPU board; mag: $Mag, slot $SlotCPU"
}


#Remove all added boards
if {[ath_send "hwcrm -m $Mag -s $SlotSCB -f"]} {
    ath_tc_failed "hwcrm: removal of SCB-RB board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotAPU -f"]} {
    ath_tc_failed "hwcrm: removal of APU board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotCPU -f"]} {
    ath_tc_failed "hwcrm: removal of CP board failed"
}


# Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#Wait 5 seconds 
print "\nWaiting 5 seconds... \n"
after 5000


#Verify the dhcpd.leases file contains SCB entries for deleted board, both for A and B backplane. Otherwise fail.
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternDeletedSCB $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of deleted leases found for SCB board; mag: $Mag, slot $SlotSCB"
}


#Verify the dhcpd.leases file contains no APU entries for deleted board. Otherwise fail.
set exp_num_leases 0
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternDeletedAPU $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of deleted leases found for APU board; mag: $Mag, slot $SlotAPU"
}


#Verify the dhcpd.leases file contains CPU entries for deleted board, both for A and B backplane. Otherwise fail.
set exp_num_leases 1
ath_send "cat /var/lib/dhcp/db/dhcpd.leases"
if {[ath_extract $DHCPPatternDeletedCPU $ATH::LastResponse] != $exp_num_leases} {
    ath_tc_failed "dhcpd.leases: Wrong number of deleted leases found for CPU board; mag: $Mag, slot $SlotCPU"
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
            ath_send "exit"
        }
    }
    print "\n\nClean up done.\n"
}


###########################################################################
# :References
###########################################################################
# References to technical documents, such as IPs, RSs, technical
# reports, CODs, PODs, manpages etc.
# No need to refer to any project planning documents such as test plans.
#
# [1] docname
#     docno
# [2] docname
#     docno

