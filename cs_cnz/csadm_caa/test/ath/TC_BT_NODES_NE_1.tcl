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
# :Title        Nodes: command nech, nels.
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
# Test nodes: ne, nels commands
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

#define all data needed for boards

set neid1 "NewId"
set neid2 "AB"
set Mag "2.4.0.5"
set SlotAPU "10"

set PatternAPU ":
HARDWARE CONFIGURATION TABLE
:
$Mag $SlotAPU AP * APUB **
:"

#Patterns for NE attributes to verify
set Pattern_nels_1 ":

DERIVED DATA
System Type: Multi-CP System
APT Type: MSC
Number of Dual-Sided CPs: 0
Number of Single-Sided CPs: 0
Number of APs: 0
Front AP: AP1
Alarm Master CP: *
Clock Master CP: *

OPERATOR CHANGEABLE DATA
Network Element Identity: $neid1
OaM Profile: PROFILE *
APZ Profile: PROFILE *
APT Profile: PROFILE *
:"


set Pattern_nels_2 ":

DERIVED DATA
System Type: Multi-CP System
APT Type: MSC
Number of Dual-Sided CPs: 0
Number of Single-Sided CPs: 0
Number of APs: 1
Front AP: AP1
Alarm Master CP: *
Clock Master CP: *

OPERATOR CHANGEABLE DATA
Network Element Identity: $neid2
OaM Profile: PROFILE *
APZ Profile: PROFILE *
APT Profile: PROFILE *
:"

set target_modified 1;  # Inform the CleanUp procedure that it has to clean up
                        # the modified exchange data.

# Connect to SUT2
cs_sut2_connect

#SUT2: Run preparation script for SUT2 (note: IMM cleanup should be automatically propagated to SUT2)
cs_node_preparation

#SUT2: Set the system to Multi-CP
ath_send "immcfg -a isMultipleCPSystem=1 CSId=1"

# Start daemon
cs_start_service_apn

#SUT2: Verify that all needed commands are available
cs_command_must_exist "nels"
cs_command_must_exist "nech"
cs_command_must_exist "hwcls"
cs_command_must_exist "hwcadd" 

#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect

#SUT: Run preparation script for SUT (note: IMM cleanup should automatically propagate to SUT2)
cs_preparation

#SUT: Set the system to Multi-CP
ath_send "immcfg -a isMultipleCPSystem=1 CSId=1"

# Start daemon
cs_start_service

#SUT: Verify that all needed commands are available
cs_command_must_exist "nels"
cs_command_must_exist "nech"
cs_command_must_exist "hwcls"
cs_command_must_exist "hwcadd"


###########################################################################
# Testcase start
###########################################################################


##############################
# SUT -> SUT2 synchronisation
##############################


#SUT: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#SUT: Set NEID attribute to $neid1
ath_send "nech -i $neid1"


#SUT: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 0 AP boards and $neid1 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_1 $LastResponse]} {
   ath_tc_failed "SUT: nels: Not expected printout format"
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#Wait 10 seconds 
print "\nWaiting 10 seconds... \n"
after 10000


#SUT2: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#SUT2: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 0 AP boards and $neid1 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_1 $LastResponse]} {
   ath_tc_failed "SUT2: nels: Not expected printout format"
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Add APU board to the HWC table without specifying dhcp
if {[ath_send "hwcadd -m $Mag -s $SlotAPU -n APUB -t AP -y 1 -e B"]} {
    ath_tc_failed "SUT: hwcadd: addition of APU board failed"
}


#SUT: Verify that APU board is present in HWC table
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternAPU $LastResponse]} {
   ath_tc_failed "SUT: hwcls: APU board is missing in mag: $Mag, slot $SlotAPU \n"
}


#SUT: Change NEID attribute to $neid2
ath_send "nech -i $neid2"


#SUT: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 1 AP board and $neid2 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_2 $LastResponse]} {
   ath_tc_failed "SUT: nels: Not expected printout format"
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect

#Wait 10 seconds 
print "\nWaiting 10 seconds... \n"
after 10000

#SUT2: Verify that APU board is present in HWC table
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternAPU $LastResponse]} {
   ath_tc_failed "SUT2: hwcls: APU board is missing in mag: $Mag, slot $SlotAPU \n"
}


#SUT2: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 1 AP board and $neid2 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_2 $LastResponse]} {
   ath_tc_failed "SUT2: nels: Not expected printout format"
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Change NEID attribute to $neid1
ath_send "nech -i $neid1"


#SUT: Remove AP board from HWC table
if {[ath_send "hwcrm -m $Mag -s $SlotAPU -f"]} {
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


#SUT2: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 0 AP boards and $neid1 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_1 $LastResponse]} {
   ath_tc_failed "SUT2: nels: Not expected printout format"
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#SUT: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 0 AP boards and $neid1 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_1 $LastResponse]} {
   ath_tc_failed "SUT: nels: Not expected printout format"
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Add APU board to the HWC table without specifying dhcp
if {[ath_send "hwcadd -m $Mag -s $SlotAPU -n APUB -t AP -y 1 -e B"]} {
    ath_tc_failed "SUT2: hwcadd: addition of APU board failed"
}


#SUT2: Verify that APU board is present in HWC table
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternAPU $LastResponse]} {
   ath_tc_failed "SUT2: hwcls: APU board is missing in mag: $Mag, slot $SlotAPU \n"
}


#SUT2: Change NEID attribute to $neid2
ath_send "nech -i $neid2"


#SUT2: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 1 AP board and $neid2 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_2 $LastResponse]} {
   ath_tc_failed "SUT2: nels: Not expected printout format"
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Verify that APU board is present in HWC table
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternAPU $LastResponse]} {
   ath_tc_failed "SUT: hwcls: APU board is missing in mag: $Mag, slot $SlotAPU \n"
}


#SUT: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 1 AP board and $neid2 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_2 $LastResponse]} {
   ath_tc_failed "SUT: nels: Not expected printout format"
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Change NEID attribute to $neid1
ath_send "nech -i $neid1"


#SUT2: Remove AP board from HWC table
if {[ath_send "hwcrm -m $Mag -s $SlotAPU -f"]} {
    ath_tc_failed "SUT2: hwcrm: removal of AP board failed"
}


#SUT2: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#SUT2: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 0 AP boards and $neid1 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_1 $LastResponse]} {
   ath_tc_failed "SUT2: nels: Not expected printout format"
}


#SUT2: Check and stop CS service
print "\nChecking and stopping CS service...\n"
cs_check_and_stop_service


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#SUT: Verify that nels printout format meets the expected pattern for Multi-CP system, containing 0 AP boards and $neid1 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_nels_1 $LastResponse]} {
   ath_tc_failed "SUT: nels: Not expected printout format"
}


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

