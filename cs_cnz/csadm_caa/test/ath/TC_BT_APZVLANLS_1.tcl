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
# :Title        Command apzvlanls
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
# Test apzvlanls command
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
cs_command_must_exist "apzvlanls"
cs_command_must_exist "cpls"
cs_command_must_exist "cpch"                     
cs_command_must_exist "hwcls" 
cs_command_must_exist "hwcadd" 
cs_command_must_exist "hwcrm" 


# Make sure that the HWC table is empty
cs_hwctable_must_be_empty
# Make sure that the CP table is empty
cs_cptable_must_be_empty
# Make sure that the CP table is empty
cs_vlantable_must_be_empty

set target_modified 1;  # Inform the CleanUp procedure that it has to clean up
                        # the modified exchange data.

#define all data needed for boards

set Mag "2.4.0.5"
set SlotCP0 "9"
set SlotBC0 "10"
set SlotBC1 "11"
set AliasBC0 "TSC-B01"
set NewVLANName "NewVLAN"
set NewVLANIP "192.168.171.0"
set NewVLANAllocTableIP "192.168.171.*"
set NewVLANMask "255.255.255.0"
set NewVLANStack "TIP"

#hwcls printout
set PatternHWC ":

HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotCP0 CP 1 CPUB  192.168.169.* 192.168.170.*
$Mag $SlotBC0 BC 1 CPUB  192.168.169.* 192.168.170.*
$Mag $SlotBC1 BC 1 CPUB  192.168.169.* 192.168.170.*
:"


#cpls printout
set PatternCP ":

CP IDENTIFICATION TABLE

CPID CPNAME ALIAS APZSYS CPTYPE
0 BC0 $AliasBC0 - -
1 BC1 - - -
1001 CP1 - - -
:"

#apzvlanls printout

set PatternVLAN_all ":

VLAN TABLE

VLAN NETWORK NETMASK STACK
APZ-A 192.168.169.0 255.255.255.0 KIP
APZ-B 192.168.170.0 255.255.255.0 KIP

ALLOCATION TABLE

VLAN CPNAME ADDRESS
APZ-A $AliasBC0 192.168.169.*
 BC1 192.168.169.*
APZ-B $AliasBC0 192.168.170.*
 BC1 192.168.170.*
:
"

set PatternVLAN_NewVLAN_all ":

VLAN TABLE

VLAN NETWORK NETMASK STACK
APZ-A 192.168.169.0 255.255.255.0 KIP
APZ-B 192.168.170.0 255.255.255.0 KIP
$NewVLANName $NewVLANIP $NewVLANMask $NewVLANStack

ALLOCATION TABLE

VLAN CPNAME ADDRESS
APZ-A $AliasBC0 192.168.169.*
 BC1 192.168.169.*
APZ-B $AliasBC0 192.168.170.*
 BC1 192.168.170.*
$NewVLANName $AliasBC0 $NewVLANAllocTableIP
 BC1 $NewVLANAllocTableIP
:
"


set PatternVLAN_NewVLAN_alias ":

VLAN TABLE

VLAN NETWORK NETMASK STACK
APZ-A 192.168.169.0 255.255.255.0 KIP
APZ-B 192.168.170.0 255.255.255.0 KIP
$NewVLANName $NewVLANIP $NewVLANMask $NewVLANStack

ALLOCATION TABLE

VLAN CPNAME ADDRESS
APZ-A $AliasBC0 192.168.169.*
APZ-B $AliasBC0 192.168.170.*
$NewVLANName $AliasBC0 $NewVLANAllocTableIP
:
"


set PatternVLAN_NewVLAN_vlan ":

VLAN TABLE

VLAN NETWORK NETMASK STACK
$NewVLANName $NewVLANIP $NewVLANMask $NewVLANStack

ALLOCATION TABLE

VLAN CPNAME ADDRESS
$NewVLANName $AliasBC0 $NewVLANAllocTableIP
 BC1 $NewVLANAllocTableIP
:
"


set PatternVLAN_NewVLAN_stack ":

VLAN TABLE

VLAN NETWORK NETMASK STACK
$NewVLANName $NewVLANIP $NewVLANMask $NewVLANStack

ALLOCATION TABLE

VLAN CPNAME ADDRESS
$NewVLANName $AliasBC0 $NewVLANAllocTableIP
 BC1 $NewVLANAllocTableIP
:
"


set PatternVLAN_NewVLAN_empty ":

VLAN TABLE

VLAN NETWORK NETMASK STACK
APZ-A 192.168.169.0 255.255.255.0 KIP
APZ-B 192.168.170.0 255.255.255.0 KIP
$NewVLANName $NewVLANIP $NewVLANMask $NewVLANStack

ALLOCATION TABLE

VLAN CPNAME ADDRESS
APZ-A **
:
"

###########################################################################
# Testcase start
###########################################################################

AutoFail OFF

#Set the system to Multi-CP
ath_send "immcfg -a isMultipleCPSystem=1 CSId=1"


#Add CP0 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotCP0 -n CPUB -t CP -y 1 -e A -d normal"]} {
    ath_tc_failed "hwcadd: addition of CPU board failed"
}


#Add BC0 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotBC0 -n CPUB -t BC -y 1 -u 0 -d client"]} {
    ath_tc_failed "hwcadd: addition of CPU board failed"
}


#Add BC1 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotBC1 -n CPUB -t BC -y 1 -u 1 -d client"]} {
    ath_tc_failed "hwcadd: addition of CPU board failed"
}


#Verify that CP1, BC0 and BC1 boards have been added to the HWC table 
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternHWC $LastResponse]} {
    ath_tc_failed "hwcls: Printout does not match the expected one. \n"
}


#Modify alias for the BC0 board using cpch command
if {[ath_send "cpch -a $AliasBC0 0"]} {
    ath_tc_failed "cpch: alias modification of BC board failed"
}


#Verify that BC0 board alias has been modified  
ath_send "cpls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCP $LastResponse]} {
    ath_tc_failed "cpls: Printout does not match the expected one. \n"
}


#Verify the apzvlanls printout  
ath_send "apzvlanls"
set LastResponse $ATH::LastResponse

if {[ath_match $PatternVLAN_all $LastResponse]} {
    ath_tc_failed "apzvlanls: Printout does not match the expected one. \n"
}


#Add the new VLAN table entry using IMM commands
cs_create_new_VLAN $NewVLANName $NewVLANIP $NewVLANMask $NewVLANStack


#Verify the apzvlanls printout  
ath_send "apzvlanls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternVLAN_NewVLAN_all $LastResponse]} {
    ath_tc_failed "apzvlanls: Printout does not match the expected one. \n"
}


#Verify the apzvlanls printout, filtered by the CP alias 
ath_send "apzvlanls -n $AliasBC0"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternVLAN_NewVLAN_alias $LastResponse]} {
    ath_tc_failed "apzvlanls: Printout does not match the expected one. \n"
}


#Verify the apzvlanls printout, filtered by VLAN name 
ath_send "apzvlanls -v $NewVLANName"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternVLAN_NewVLAN_vlan $LastResponse]} {
    ath_tc_failed "apzvlanls: Printout does not match the expected one. \n"
}


#Verify the apzvlanls printout, filtered by stack of the NewVLAN 
ath_send "apzvlanls -s $NewVLANStack"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternVLAN_NewVLAN_stack $LastResponse]} {
    ath_tc_failed "apzvlanls: Printout does not match the expected one. \n"
}


#Remove all added boards
if {[ath_send "hwcrm -m $Mag -s $SlotBC1 -f"]} {
    ath_tc_failed "hwcrm: removal of BC1 board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotBC0 -f"]} {
    ath_tc_failed "hwcrm: removal of BC0 board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotCP0 -f"]} {
    ath_tc_failed "hwcrm: removal of CP0 board failed"
}


# Make sure that the HWC table is empty
cs_hwctable_must_be_empty


# Make sure that the CP table is empty
cs_cptable_must_be_empty


#Verify that allocation table is empty using apzvlanls 
ath_send "apzvlanls"
set LastResponse $ATH::LastResponse
if {![ath_match $PatternVLAN_NewVLAN_empty $LastResponse]} {
    ath_tc_failed "apzvlanls: Printout does not match the expected one. \n"
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

