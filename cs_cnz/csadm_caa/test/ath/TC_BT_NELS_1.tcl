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
# :Title        Command nels
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
###########################################################################

###########################################################################
# :Test Case Specification
# ------------------------
# Test nels command
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

###########################################################################
# Test preparation
###########################################################################
set target_modified 1;  # Inform the CleanUp procedure that it has to clean up
                        # the modified exchange data.
                        
# Call preparation routine
cs_preparation

# Start daemon
cs_start_service

# Verify that all needed commands are available
cs_command_must_exist "nels"
cs_command_must_exist "hwcls"
cs_command_must_exist "hwcadd"



set Mag "2.4.0.5"
set SlotAPU "10"
set systype ""

set PatternAPU ":
HARDWARE CONFIGURATION TABLE
:
$Mag $SlotAPU AP * APUB **
:"

#Pattern for nels output
set PatternNEMulti ":
DERIVED DATA
System Type: Multi-CP System
APT Type: *
Number of Dual-Sided CPs: *
Number of Single-Sided CPs: *
Number of APs: 0
Front AP: *
Alarm Master CP: *
Clock Master CP: *

OPERATOR CHANGEABLE DATA
Network Element Identity: **
OaM Profile: **
APZ Profile: **
APT Profile: **
:"


set PatternNEMulti_APU ":
DERIVED DATA
System Type: Multi-CP System
APT Type: *
Number of Dual-Sided CPs: *
Number of Single-Sided CPs: *
Number of APs: 1
Front AP: *
Alarm Master CP: *
Clock Master CP: *

OPERATOR CHANGEABLE DATA
Network Element Identity: **
OaM Profile: **
APZ Profile: **
APT Profile: **
:"


#Pattern for nels output
set PatternNESingle ":
System Type: Single-CP System
APT Type: *
:"

###########################################################################
# Testcase start
###########################################################################
 
AutoFail OFF

#Set the system to Multi-CP
ath_send "immcfg -a isMultipleCPSystem=1 CSId=1"

# Make sure that the HWC table is empty
cs_hwctable_must_be_empty


#Verify that nels printout format meets the expected pattern for Multi-CP system, containing 0 AP boards
ath_send "nels"
set LastResponse $ATH::LastResponse

if {[ath_match $PatternNEMulti $LastResponse]} {
   ath_tc_failed "nels Multi-CP: Not expected printout format"
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


#Verify that nels printout format meets the expected pattern for Multi-CP system, containing 1 AP board
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternNEMulti_APU $LastResponse]} {
   ath_tc_failed "nels Multi-CP: Not expected printout format"
}

#Set the system to Single-CP
ath_send "immcfg -a isMultipleCPSystem=0 CSId=1"


#Verify that nels printout format meets the expected pattern for Single-CP system
ath_send "nels"
set LastResponse $ATH::LastResponse

if {[ath_match $PatternNESingle $LastResponse]} {    
  ath_tc_failed "nels Single-CP: Not expected printout format"
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

