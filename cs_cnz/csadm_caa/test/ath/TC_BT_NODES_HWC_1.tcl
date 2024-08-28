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
# :Title        Nodes: command hwcadd, hwcls, hwcrm
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
# Test nodes: hwcadd, hwcls, hwcrm commands
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
set SlotAPU "10"
set SlotDVD "5"

#Patterns for hwcls for all cards to be added
set PatternHWC_all ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotSCB - - SCB-RP 192.168.169.* 192.168.170.*
$Mag $SlotCPU CP 1 CPUB 192.168.169.* 192.168.170.*
$Mag $SlotAPU AP 1 APUB 192.168.169.* 192.168.170.*
$Mag2 $SlotDVD AP 1 DVD - -
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


##############################
# SUT -> SUT2 synchronisation
##############################


#SUT: Add SCB-RP board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotSCB -n SCB-RP -d client"]} {
    ath_tc_failed "SUT: hwcadd: addition  of SCB-RP board failed"
}


#SUT: Add CPU board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotCPU -n CPUB -t CP -y 1 -e A -a 10.11.12.13,10.11.13.13 -b 255.255.255.0,255.255.255.0 -d normal"]} {
    ath_tc_failed "SUT: hwcadd: addition of CPU board failed"
}


#SUT: Add APU board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotAPU -n APUB -t AP -y 1 -e B"]} {
    ath_tc_failed "SUT: hwcadd: addition of APU board failed"
}


#SUT: Add AP DVD board to the HWC table
if {[ath_send "hwcadd -m $Mag2 -s $SlotDVD -n DVD -t AP -y 1 -e B"]} {
    ath_tc_failed "SUT: hwcadd: addition of DVD board failed"
}


#SUT: Verify hwcls command. View all table.
ath_send "hwcls"
if {[ath_match $PatternHWC_all $ATH::LastResponse]} {
    ath_tc_failed "SUT: hwcls: Printout does not match the expected one."
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect

#Wait 10 seconds 
print "\nWaiting 10 seconds... \n"
after 10000

#SUT2: Verify hwcls command. View all table.
ath_send "hwcls"
if {[ath_match $PatternHWC_all $ATH::LastResponse]} {
    ath_tc_failed "SUT: hwcls: Printout does not match the expected one."
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
if {[ath_send "hwcrm -m $Mag -s $SlotAPU -f"]} {
    ath_tc_failed "SUT: hwcrm: removal of APU board failed"
}
if {[ath_send "hwcrm -m $Mag2 -s $SlotDVD -f"]} {
    ath_tc_failed "SUT: hwcrm: removal of AP board failed"
}


#SUT: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect

#Wait 10 seconds 
print "\nWaiting 10 seconds... \n"
after 10000

#SUT2: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


##############################
# SUT2 -> SUT synchronisation
##############################


#SUT2: Add SCB-RP board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotSCB -n SCB-RP -d client"]} {
    ath_tc_failed "SUT2: hwcadd: addition  of SCB-RP board failed"
}


#SUT2: Add CPU board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotCPU -n CPUB -t CP -y 1 -e A -a 10.11.12.13,10.11.13.13 -b 255.255.255.0,255.255.255.0 -d normal"]} {
    ath_tc_failed "SUT2: hwcadd: addition of CPU board failed"
}


#SUT2: Add APU board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotAPU -n APUB -t AP -y 1 -e B"]} {
    ath_tc_failed "SUT2: hwcadd: addition of APU board failed"
}


#SUT2: Add AP DVD board to the HWC table
if {[ath_send "hwcadd -m $Mag2 -s $SlotDVD -n DVD -t AP -y 1 -e B"]} {
    ath_tc_failed "SUT2: hwcadd: addition of DVD board failed"
}


#SUT2: Verify hwcls command. View all table.
ath_send "hwcls"
if {[ath_match $PatternHWC_all $ATH::LastResponse]} {
    ath_tc_failed "SUT2: hwcls: Printout does not match the expected one."
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Verify hwcls command. View all table.
ath_send "hwcls"
if {[ath_match $PatternHWC_all $ATH::LastResponse]} {
    ath_tc_failed "SUT: hwcls: Printout does not match the expected one."
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Remove all added boards
if {[ath_send "hwcrm -m $Mag -s $SlotSCB -f"]} {
    ath_tc_failed "SUT2: hwcrm: removal of SCB-RB board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotCPU -f"]} {
    ath_tc_failed "SUT2: hwcrm: removal of CPU board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotAPU -f"]} {
    ath_tc_failed "SUT2: hwcrm: removal of APU board failed"
}
if {[ath_send "hwcrm -m $Mag2 -s $SlotDVD -f"]} {
    ath_tc_failed "SUT2: hwcrm: removal of AP board failed"
}


#SUT2: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#SUT2: Check and stop CS service
print "\nChecking and stopping CS service...\n"
cs_check_and_stop_service


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#SUT: Check and stop CS service
print "\nChecking and stopping CS service...\n"
cs_check_and_stop_service


#Disconnect from SUT
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

