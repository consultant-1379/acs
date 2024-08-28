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
# :Title        Command hwcadd, hwcls, hwcrm
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
# Test hwcadd, hwcls, hwcrm commands
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
cs_command_must_exist "hwcadd"
cs_command_must_exist "hwcls"
cs_command_must_exist "hwcrm"

set target_modified 1;  # Inform the CleanUp procedure that it has to clean up
                        # the modified exchange data.
                        
# Make sure that the HWC table is empty
cs_hwctable_must_be_empty


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


set PatternHWC_FBN ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotCPU CP 1 CPUB 192.168.169.* 192.168.170.*
:"


set PatternHWC_mag ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag2 $SlotDVD AP 1 DVD - -
:"


set PatternHWC_side ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotAPU AP 1 APUB 192.168.169.* 192.168.170.*
$Mag2 $SlotDVD AP 1 DVD - -
:"


set PatternHWC_slot ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotCPU CP 1 CPUB 192.168.169.* 192.168.170.*
$Mag2 $SlotDVD AP 1 DVD - -
:"


set PatternHWC_systype ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotAPU AP 1 APUB 192.168.169.* 192.168.170.*
$Mag2 $SlotDVD AP 1 DVD - -
:"


set PatternHWC_sysnum ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotCPU CP 1 CPUB 192.168.169.* 192.168.170.*
$Mag $SlotAPU AP 1 APUB 192.168.169.* 192.168.170.*
$Mag2 $SlotDVD AP 1 DVD - -
:"

set PatternHWC_DHCP_none ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotAPU AP 1 APUB 192.168.169.* 192.168.170.*
$Mag2 $SlotDVD AP 1 DVD - -
:"

set PatternHWC_DHCP_client ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotSCB - - SCB-RP 192.168.169.* 192.168.170.*
:"

set PatternHWC_DHCP_normal ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotCPU CP 1 CPUB 192.168.169.* 192.168.170.*
:"

set PatternHWC_long ":
HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN SIDE SEQNUM
$Mag $SlotSCB  - - SCB-RP - -

IPA IPB ALIASA MASKA
192.168.169.* 192.168.170.* - -

ALIASB MASKB DHCP
- - Client

MAG SLOT SYSTYPE SYSNUM FBN SIDE SEQNUM
$Mag $SlotCPU CP 1 CPUB A -

IPA IPB ALIASA MASKA
192.168.169.* 192.168.170.* 10.11.12.13 255.255.255.0

ALIASB MASKB DHCP
10.11.13.13 255.255.255.0 Normal

MAG SLOT SYSTYPE SYSNUM FBN SIDE SEQNUM
$Mag $SlotAPU AP 1 APUB B -

IPA IPB ALIASA MASKA
192.168.169.* 192.168.170.* - -

ALIASB MASKB DHCP
- - None

MAG SLOT SYSTYPE SYSNUM FBN SIDE SEQNUM
$Mag2 $SlotDVD AP 1 DVD B -

IPA IPB ALIASA MASKA
- - - -

ALIASB MASKB DHCP
- - None
:"


set PatternAPU ":
HARDWARE CONFIGURATION TABLE
:
$Mag $SlotAPU AP * APUB **
:"
set PatternDVD ":
HARDWARE CONFIGURATION TABLE
:
$Mag $SlotDVD AP * DVD **
:"


###########################################################################
# Testcase start
###########################################################################

#Add SCB-RP board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotSCB -n SCB-RP -d client"]} {
    ath_tc_failed "hwcadd: addition  of SCB-RP board failed"
}


#Add CPU board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotCPU -n CPUB -t CP -y 1 -e A -a 10.11.12.13,10.11.13.13 -b 255.255.255.0,255.255.255.0 -d normal"]} {
    ath_tc_failed "hwcadd: addition of CPU board failed"
}


#Add APU board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotAPU -n APUB -t AP -y 1 -e B"]} {
    ath_tc_failed "hwcadd: addition of APU board failed"
}


#Add AP DVD board to the HWC table
if {[ath_send "hwcadd -m $Mag2 -s $SlotDVD -n DVD -t AP -y 1 -e B"]} {
    ath_tc_failed "hwcadd: addition of DVD board failed"
}


#Verify hwcls command. View all table.
ath_send "hwcls"
if {[ath_match $PatternHWC_all $ATH::LastResponse]} {
    ath_tc_failed "hwcls: Printout does not match the expected one."
}


#Verify hwcls -n option. List CPUB board.
ath_send "hwcls -n CPUB"
if {[ath_match $PatternHWC_FBN $ATH::LastResponse]} {
    ath_tc_failed "hwcls -n: Printout does not match the expected one."
}


#Verify hwcls -m option. List magazine.
ath_send "hwcls -m $Mag2"
if {[ath_match $PatternHWC_mag $ATH::LastResponse]} {
    ath_tc_failed "hwcls -m: Printout does not match the expected one."
}


#Verify hwcls -e option. List side.
ath_send "hwcls -e B"
if {[ath_match $PatternHWC_side $ATH::LastResponse]} {
    ath_tc_failed "hwcls -e: Printout does not match the expected one."
}


#Verify hwcls -s option. List side.
ath_send "hwcls -s $SlotCPU"
if {[ath_match $PatternHWC_slot $ATH::LastResponse]} {
    ath_tc_failed "hwcls -s: Printout does not match the expected one."
}


#Verify hwcls -t option. List system type.
ath_send "hwcls -t AP"
if {[ath_match $PatternHWC_systype $ATH::LastResponse]} {
    ath_tc_failed "hwcls -t: Printout does not match the expected one."
}


#Verify hwcls -y option. List system number.
ath_send "hwcls -y 1"
if {[ath_match $PatternHWC_sysnum $ATH::LastResponse]} {
    ath_tc_failed "hwcls -y: Printout does not match the expected one."
}


#Verify hwcls -d option. List boards with dhcp none.
ath_send "hwcls -d none"
if {[ath_match $PatternHWC_DHCP_none $ATH::LastResponse]} {
    ath_tc_failed "hwcls -d none: Printout does not match the expected one."
}


#Verify hwcls -d option. List boards with dhcp client.
ath_send "hwcls -d client"
if {[ath_match $PatternHWC_DHCP_client $ATH::LastResponse]} {
    ath_tc_failed "hwcls -d client: Printout does not match the expected one."
}


#Verify hwcls -d option. List boards with dhcp none.
ath_send "hwcls -d normal"
if {[ath_match $PatternHWC_DHCP_normal $ATH::LastResponse]} {
    ath_tc_failed "hwcls -d normal: Printout does not match the expected one."
}


#Verify hwcls -x long option. List long info.
ath_send "hwcls -x long"
if {[ath_match $PatternHWC_long $ATH::LastResponse]} {
    ath_tc_failed "hwcls -x long: Printout does not match the expected one."
}


#Remove all added boards
if {[ath_send "hwcrm -m $Mag -s $SlotSCB -f"]} {
    ath_tc_failed "hwcrm: removal of SCB-RB board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotCPU -f"]} {
    ath_tc_failed "hwcrm: removal of CPU board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotAPU -f"]} {
    ath_tc_failed "hwcrm: removal of APU board failed"
}
if {[ath_send "hwcrm -m $Mag2 -s $SlotDVD -f"]} {
    ath_tc_failed "hwcrm: removal of AP board failed"
}


# Make sure that the HWC table is empty
cs_hwctable_must_be_empty


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

