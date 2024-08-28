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
# :Title        Nodes: negative testing
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
# 2 nodes available, Config.ath modified - APG2 variables defined
###########################################################################

###########################################################################
# :Test Case Specification
# ------------------------
# Test nodes: negative testing
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
ath_check_parameter "SUT::APG2"      "Enter the name or IP-address of APG2: "
ath_check_parameter "SUT::APG2_USER" "Enter the user ID for APG2: "
ath_check_parameter "SUT::APG2_PW"   "Enter the password for APG2: "
ath_check_parameter "SUT::PROTOCOL" "Telnet or SSH: "

#**************************************************************************
ath_display "Execution"
#**************************************************************************
append proc_file $ATH::SCRIPTPATH "CS_Common.lib"
ath_source $proc_file

###########################################################################
# Test preparation
###########################################################################

#define all data needed for HW table

set Mag "2.4.0.5"
set Mag2 "2.4.0.6"
set SlotSCB "0"
set SlotCPU "5"


#Patterns for hwcls for all cards to be added
set PatternHWC_all ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotSCB - - SCB-RP 192.168.169.* 192.168.170.*
$Mag $SlotCPU CP 1 CPUB 192.168.169.* 192.168.170.*
:"

set PatternHWC_magazine ":

HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
2.4.0.5 0 - - SCB-RP 192.168.169.* 192.168.170.*
2.4.0.5 2 CP 1 RPBI-S 192.168.169.* 192.168.170.*
2.4.0.5 3 AP 1 GEA - -
2.4.0.5 5 CP 1 CPUB 192.168.169.* 192.168.170.*
2.4.0.5 7 CP 1 MAUB 192.168.169.* 192.168.170.*
2.4.0.5 8 CP 1 CPUB 192.168.169.* 192.168.170.*
2.4.0.5 12 AP 1 Disk - -
2.4.0.5 14 AP 1 APUB 192.168.169.* 192.168.170.*
2.4.0.5 16 AP 1 DVD - -
2.4.0.5 20 AP 1 APUB 192.168.169.* 192.168.170.*
2.4.0.5 22 AP 1 Disk - -
2.4.0.5 23 CP 1 RPBI-S 192.168.169.* 192.168.170.*
2.4.0.5 25 - - SCB-RP 192.168.169.* 192.168.170.*
:"

set target_modified 1;  # Inform the CleanUp procedure that it has to clean up
                        # the modified exchange data.

# Connect to SUT2
cs_sut2_connect

#SUT2: Run preparation script for SUT2 (note: IMM cleanup should be automatically propagated to SUT2)
cs_node_preparation

# Start daemon
cs_start_service_apn

#SUT2: Verify that all needed commands are available
cs_command_must_exist "hwcadd"
cs_command_must_exist "hwcls"
cs_command_must_exist "hwcrm"

#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect

#SUT: Run preparation script for SUT (note: IMM cleanup should automatically propagate to SUT2)
cs_preparation

# Start daemon
cs_start_service

#SUT: Verify that all needed commands are available
cs_command_must_exist "hwcadd"
cs_command_must_exist "hwcls"
cs_command_must_exist "hwcrm"


###########################################################################
# Testcase start
###########################################################################


#SUT: Make sure that the HWC table is empty
cs_hwctable_must_be_empty
    

#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


##########################################################
# Case 1. Try sync on SLAVE CS STARTUP. Master CS down.
##########################################################

print "\n\nCase 1. Try sync on SLAVE CS STARTUP. Master CS down.\n\n"


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Stop SLAVE CS daemon
cs_stop_service


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Add SCB-RP board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotSCB -n SCB-RP -d client"]} {
    ath_tc_failed "SUT: hwcadd: addition  of SCB-RP board failed"
}


#SUT: Add CPU board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotCPU -n CPUB -t CP -y 1 -e A -a 10.11.12.13,10.11.13.13 -b 255.255.255.0,255.255.255.0 -d normal"]} {
    ath_tc_failed "SUT: hwcadd: addition of CPU board failed"
}


#SUT: Verify that boards have been added to HWC table.
ath_send "hwcls"
if {[ath_match $PatternHWC_all $ATH::LastResponse]} {
    ath_tc_failed "Case 1: SUT: hwcls: Printout does not match the expected one."
}


#SUT: Stop MASTER CS daemon
cs_stop_service


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Start SLAVE CS daemon
cs_start_service_apn


#Wait 10 seconds
print "\nWaiting 10 seconds... \n"
after 10000


#SUT2: Verify that no synchronization has been done and that the HWC table is empty
cs_hwctable_must_be_empty


#SUT2: Stop Slave CS daemon
cs_stop_service


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Start Master CS daemon
cs_start_service


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Start SLAVE CS daemon
cs_start_service_apn


#Wait 10 seconds
print "\nWaiting 10 seconds... \n"
after 10000


#SUT2: Verify that HWC table has been synchronized and boards added.
ath_send "hwcls"
if {[ath_match $PatternHWC_all $ATH::LastResponse]} {
    ath_tc_failed "Case 1: SUT: hwcls: Printout does not match the expected one."
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


################################################################
# Case 2. Try to update HWC Table on SUT2 with Master CS down. 
################################################################

print "\n\nCase 2. Try to update HWC Table on SUT2 with Master CS down.\n\n"


#SUT: Stop MASTER CS daemon
cs_stop_service


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Verify that error code "Unable to connect to server" (Exit code: 117) is received when trying to remove a board
cs_send_cmd_exp_errcode "hwcrm -m $Mag -s $SlotSCB -f" 117


#SUT2: Verify that HWC table has not been synchronized and no board was removed.
ath_send "hwcls"
if {[ath_match $PatternHWC_all $ATH::LastResponse]} {
    ath_tc_failed "Case 2: SUT: hwcls: Printout does not match the expected one."
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Start Master CS daemon
cs_start_service


#SUT: Remove all added boards
if {[ath_send "hwcrm -m $Mag -s $SlotSCB -f"]} {
    ath_tc_failed "SUT: hwcrm: removal of SCB-RB board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotCPU -f"]} {
    ath_tc_failed "SUT: hwcrm: removal of CPU board failed"
}


#SUT: Verify that all boards have been removed
cs_hwctable_must_be_empty


#Wait 10 seconds to synchronise
print "\nWaiting 10 seconds... \n"
after 10000

##########################################################
# Case 3. Try sync on Master CS UPDATE. SLAVE CS down.
##########################################################

print "\n\nCase 3. Try sync on Master CS UPDATE. SLAVE CS down.\n\n"


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Stop SLAVE CS daemon
cs_stop_service


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Add SCB-RP board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotSCB -n SCB-RP -d client"]} {
    ath_tc_failed "SUT: hwcadd: addition  of SCB-RP board failed"
}


#SUT: Add CPU board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotCPU -n CPUB -t CP -y 1 -e A -a 10.11.12.13,10.11.13.13 -b 255.255.255.0,255.255.255.0 -d normal"]} {
    ath_tc_failed "SUT: hwcadd: addition of CPU board failed"
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#Wait 10 seconds
print "\nWaiting 10 seconds... \n"
after 10000


#SUT2: Verify that no synchronization has been done and that the HWC table is empty
cs_hwctable_must_be_empty


#SUT2: Start SLAVE CS daemon
cs_start_service_apn


#Wait 10 seconds
print "\nWaiting 10 seconds... \n"
after 10000


#SUT2: Verify that HWC table has been synchronized and boards added.
ath_send "hwcls"
if {[ath_match $PatternHWC_all $ATH::LastResponse]} {
    ath_tc_failed "Case 3: SUT: hwcls: Printout does not match the expected one."
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Remove all added boards
if {[ath_send "hwcrm -m $Mag -s $SlotSCB -f"]} {
    ath_tc_failed "SUT: hwcrm: removal of SCB-RB board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotCPU -f"]} {
    ath_tc_failed "SUT: hwcrm: removal of CPU board failed"
}


#SUT: Verify that all boards have been removed
cs_hwctable_must_be_empty


#Wait 10 seconds to synchronise
print "\nWaiting 10 seconds... \n"
after 10000


###############################################################################################
# Case 4. Try update HWC Table with a huge bulk of data and shutdown SLAVE CS while synchronization.
###############################################################################################

print "\n\nCase 4. Try update HWC Table with a huge bulk of data and shutdown SLAVE CS while synchronization.\n\n"


#SUT: add base magazine number 0
cs_fill_base_magazine 2.4.0.5 0


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT: Stop SLAVE CS daemon
cs_stop_service


#Wait 10 seconds
print "\nWaiting 10 seconds... \n"
after 10000


#SUT2: Verify that HWC table has NOT been synchronized completely.
ath_send "hwcls"
if {[ath_match $PatternHWC_magazine $ATH::LastResponse]} {
    ath_tc_failed "Case 4: SUT: hwcls: Printout does not match the expected one."
}


#SUT2: Start SLAVE CS daemon
cs_start_service_apn


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: empty magazine 2.4.0.5
cs_empty_base_magazine 2.4.0.5


#Wait 10 seconds to synchronise
print "\nWaiting 10 seconds... \n"
after 10000


############################################################
# Case 5. Try to add a board on SUT2 while Master CS is down.
############################################################

print "\n\nCase 5. Try to add a board on SUT2 while Master CS is down.\n\n"


#SUT: Stop Master CS daemon
cs_stop_service


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Verify that Exit Code 117 (Unable to connect to server) is issued when trying to add a board on Slave
cs_send_cmd_exp_errcode "hwcadd -m $Mag -s $SlotSCB -n SCB-RP -d client" 117


#SUT2: Verify that no board has been added
cs_hwctable_must_be_empty


#SUT2: Check if is running and if so, stop Slave CS
cs_check_and_stop_service


#Disconnect from SUT2
print "\n\nDisconnecting from SUT...\n\n"
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
           
            # Connect to SUT
            cs_sut_connect

            #SUT: Run Clean-up script for SUT (note: IMM cleanup should automatically propagate to SUT2)
            cs_cleanup

            #Disconnect from SUT and connect to SUT2
            print "\n\nDisconnecting from SUT...\n\n"
            ath_send "exit"
            cs_sut2_connect

            #SUT2: Run Clean-up script for SUT2 (note: IMM cleanup should be automatically propagated to SUT2)
            cs_node_cleanup

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

