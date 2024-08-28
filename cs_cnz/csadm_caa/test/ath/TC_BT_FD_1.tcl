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
# :Title        Command fdch, fdls, fdrm.
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
# Test fdch, fdls, fdrm commands
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

# Run preparation script
cs_preparation

# Start daemon
cs_start_service

# Verify that all needed commands are available
cs_command_must_exist "fddef" 
cs_command_must_exist "fdls"
cs_command_must_exist "fdrm"                     
cs_command_must_exist "hwcls" 
cs_command_must_exist "hwcadd" 
cs_command_must_exist "hwcrm" 

# Make sure that the HWC table is empty
cs_hwctable_must_be_empty
# Make sure that the FD table is empty
cs_fdtable_must_be_empty

set target_modified 1;  # Inform the CleanUp procedure that it has to clean up
                        # the modified exchange data.

#define all data needed for boards

set MagAP "2.4.0.5"
set SlotAP "9"
set AP "AP1"
set Domain "MTAP"
set Service "CHS"

#hwcls printout
set PatternHWC ":

HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$MagAP $SlotAP AP 1 APUB 192.168.169.* 192.168.170.*
:"


set PatternFD_Clear ":

FUNCTION DISTRIBUTION TABLE

AP DOMAIN SERVICE
$AP - -
:"


set PatternFD_Function ":

FUNCTION DISTRIBUTION TABLE

AP DOMAIN SERVICE
$AP $Domain $Service
:"


###########################################################################
# Testcase start
###########################################################################

AutoFail OFF

#Set the system to Multi-CP
ath_send "immcfg -a isMultipleCPSystem=1 CSId=1"


#Add AP board to the HWC table
if {[ath_send "hwcadd -m $MagAP -s $SlotAP -n APUB -t AP -y 1"]} {
    ath_tc_failed "hwcadd: addition of CPU board failed"
}


#Verify that AP board has been added to the HWC table 
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternHWC $LastResponse]} {
    ath_tc_failed "hwcls: Printout does not match the expected one. \n"
}


#Verify that AP board has been added to FD table  
ath_send "fdls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternFD_Clear $LastResponse]} {
    ath_tc_failed "fdls: Printout does not match the expected one. \n"
}


#Define a function distribution for the AP board using fddef command
if {[ath_send "fddef -d $Domain -s $Service $AP"]} {
    ath_tc_failed "fddef: Defining function distribution for AP board failed"
}


#Verify that function distribution for AP board has been defined in FD table  
ath_send "fdls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternFD_Function $LastResponse]} {
    ath_tc_failed "fdls: Printout does not match the expected one. \n"
}


#Remove function distribution of the AP board using fdrm command
if {[ath_send "fdrm -d $Domain -s $Service $AP"]} {
    ath_tc_failed "fdrm: Defining function distribution for AP board failed"
}


#Verify that function distribution for AP board has been removed from FD table  
ath_send "fdls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternFD_Clear $LastResponse]} {
    ath_tc_failed "fdls: Printout does not match the expected one. \n"
}


#Remove AP board from HWC table
if {[ath_send "hwcrm -m $MagAP -s $SlotAP -f"]} {
    ath_tc_failed "hwcrm: removal of AP board failed"
}


# Make sure that the HWC table is empty
cs_hwctable_must_be_empty


# Make sure that the FD table is empty
cs_fdtable_must_be_empty


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

