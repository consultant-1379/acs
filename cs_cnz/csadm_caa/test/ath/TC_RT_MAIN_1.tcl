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
# :Title        Main regression test case
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
# TC_RT_PREPARATION_1.tcl has been run
###########################################################################

###########################################################################
# :Test Case Specification
# ------------------------
# Main regression test case
# NOTICE: cpch -r section is commented out due to a fault. Once it's fixed uncomment it!
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

#Set all necessary paths and variables for lib and bin
cs_set_path

ath_send "rm -f /tmp/acs_cs_trace.log"
ath_send "export DEBUG_CS=1"

AutoFail OFF

#define all data needed

set Mag "2.4.0.9"
set SlotSCB "0"
set SlotCPU "5"
set SlotAPU "10"
set SlotBC48 "11"
set SlotBC49 "12"
set BC48 "BC48"
set BC48ID "48"
set BC49 "BC49"
set BC49ID "49"
set CP "CP3"
set CPID "1003"
set Alias "TSC-CP3" 
set APZSys "APZ21255"
set CPType "21255"
set AP "AP3"
set Domain "MTAP"
set Service "CHS"
set GroupName "BCG1"
set NEId "NE1"
append ALLBC $BC48 "," $BC49

# uncomment once cpch -r fault is fixed
#append CPS $CP 
append CPS $Alias


set PatternCP ":

CP IDENTIFICATION TABLE

CPID CPNAME ALIAS APZSYS CPTYPE
:
$BC48ID $BC48 - - -
$BC49ID $BC49 - - -
:
$CPID $CP $Alias $APZSys $CPType
:"


set PatternCP_reset ":

CP IDENTIFICATION TABLE

CPID CPNAME ALIAS APZSYS CPTYPE
:
$BC48ID $BC48 - - -
$BC49ID $BC49 - - -
:
$CPID $CP - - -
:"


set PatternFD ":

FUNCTION DISTRIBUTION TABLE

AP DOMAIN SERVICE
:
$AP $Domain $Service
:"

set PatternFD_empty ":

FUNCTION DISTRIBUTION TABLE

AP DOMAIN SERVICE
:
$AP - -
:"

set PatternCPG ":
PREDEFINED CP GROUPS

ALL: **
:
ALLBC: **
:

OPERATOR DEFINED CP GROUPS

BCG1: $ALLBC
:
"

set Pattern_NE ":

DERIVED DATA
System Type: Multi-CP System
APT Type: *
Number of Dual-Sided CPs: **
Number of Single-Sided CPs: **
Number of APs: **
Front AP: AP1
Alarm Master CP: *
Clock Master CP: *

OPERATOR CHANGEABLE DATA
Network Element Identity: $NEId
OaM Profile: PROFILE **
APZ Profile: PROFILE **
APT Profile: PROFILE **
:"

set PatternVLAN ":

VLAN TABLE

VLAN NETWORK NETMASK STACK
APZ-A 192.168.169.0 255.255.255.0 KIP
APZ-B 192.168.170.0 255.255.255.0 KIP

ALLOCATION TABLE

VLAN CPNAME ADDRESS
:
"

###########################################################################
# Test case
###########################################################################

#Add SCB-RP board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotSCB -n SCB-RP -d client"]} {
    ath_tc_failed "hwcadd: addition  of SCB-RP board failed"
}


#Add CPU board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotCPU -n CPUB -t CP -y 3 -e A -d normal"]} {
    ath_tc_failed "hwcadd: addition of CP board failed"
}


#Add BC0 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotBC48 -n CPUB -t BC -y 3 -u $BC48ID -d normal"]} {
    ath_tc_failed "hwcadd: addition of BC48 board failed"
}


#Add BC1 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotBC49 -n CPUB -t BC -y 3 -u $BC49ID -d normal"]} {
    ath_tc_failed "hwcadd: addition of BC49 board failed"
}


#Add AP board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotAPU -n APUB -t AP -y 3 -e A"]} {
    ath_tc_failed "hwcadd: addition of APU board failed"
}


#Modify info of CP board using cpch command
if {[ath_send "cpch -a $Alias -s $APZSys -t $CPType $CPID"]} {
    ath_tc_failed "cpch: alias modification of CP board failed"
}


#Verify that CP board info has been modified  
ath_send "cpls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCP $LastResponse]} {
    ath_tc_failed "cpls set: Printout does not match the expected one. \n"
}


#Reset the CP board info using cpch command
if {[ath_send "cpch -r $CPID"]} {
    ath_tc_failed "cpch: reset of CP board failed"
}


#Verify that CP board info has been reset  
ath_send "cpls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCP_reset $LastResponse]} {
    ath_tc_failed "cpls reset: Printout does not match the expected one. \n"
}


#Define a function distribution for the AP board using fddef command
if {[ath_send "fddef -d $Domain -s $Service $AP"]} {
    ath_tc_failed "fddef: Defining function distribution for AP board failed"
}


#Verify that function distribution for AP board has been defined in FD table  
ath_send "fdls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternFD $LastResponse]} {
    ath_tc_failed "fdls function: Printout does not match the expected one. \n"
}


#Remove function distribution of the AP board using fdrm command
if {[ath_send "fdrm -d $Domain -s $Service $AP"]} {
    ath_tc_failed "fdrm: Defining function distribution for AP board failed"
}


#Verify that function distribution for AP board has been removed from FD table  
ath_send "fdls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternFD_empty $LastResponse]} {
    ath_tc_failed "fdls empty: Printout does not match the expected one. \n"
}


#Define a CP group containing CP board  
if {[ath_send "cpgdef -cp $ALLBC $GroupName"]} {
    ath_tc_failed "cpgdef: defining CP group failed"
}


#Verify that the group has been added to CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG $LastResponse]} {
    ath_tc_failed "cpgls group: Printout does not match the expected one. \n"
}


#Remove the CP group  
if {[ath_send "cpgrm $GroupName"]} {
    ath_tc_failed "cpgrm: removing CP group failed"
}


#Verify that the group has been removed from CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG $LastResponse] == 0} {
    ath_tc_failed "cpgls empty: Printout does not match the expected one. \n"
}


#Change NEID attribute
if {[ath_send "nech -i $NEId"]} {
    ath_tc_failed "nech: NEID change failed"
}


#Verify that NEID attribute has been changed 
ath_send "nels"
set LastResponse $ATH::LastResponse
if {[ath_match $Pattern_NE $LastResponse]} {
    ath_tc_failed "nels: NEID  attribute has not been changed"
}


#Verify the apzvlanls printout  
ath_send "apzvlanls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternVLAN $LastResponse]} {
    ath_tc_failed "apzvlanls: Printout does not match the expected one. \n"
}


# uncomment once hwcsync fault is fixed
# Run hwcsync (no result expected, though stability of CS service is tested by that call)
#if {[ath_send "hwcsync"]} {
#    ath_tc_failed "hwcsync failed"
#}


#Wait 5 seconds 
print "\nWaiting 5 seconds... \n"
after 5000


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
if {[ath_send "hwcrm -m $Mag -s $SlotBC48 -f"]} {
    ath_tc_failed "hwcrm: removal of BC48 board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotBC49 -f"]} {
    ath_tc_failed "hwcrm: removal of BC49 board failed"
}



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

