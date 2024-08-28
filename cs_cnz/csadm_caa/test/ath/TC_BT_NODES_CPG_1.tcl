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
# :Title        Nodes: command cpgls, cpgdef, cpgrm
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
# Test nodes: cpgls, cpgdef, cpgrm commands
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

set Mag "2.4.0.5"
set SlotCP0 "9"
set SlotBC0 "10"
set SlotBC1 "11"
set CP0ID "1001"
set BC0ID "1"
set BC1ID "2"
set CP0Name "CP0"
set BC0Name "BC0"
set BC1Name "BC1"
set GroupName "BCG1"
append ALLBC_Group $BC0Name "," $BC1Name
append ALL_Group $ALLBC_Group "," $CP0Name 

#hwcls printout

set PatternHWC_all ":

HARDWARE CONFIGURATION TABLE

MAG SLOT SYSTYPE SYSNUM FBN IPA IPB
$Mag $SlotCP0 CP 0 CPUB  192.168.169.* 192.168.170.*
$Mag $SlotBC0 BC 1 CPUB  192.168.169.* 192.168.170.*
$Mag $SlotBC1 BC 1 CPUB  192.168.169.* 192.168.170.*
:"


set PatternCPG_all_nogroup ":
PREDEFINED CP GROUPS

ALL: $ALL_Group
ALLBC: $ALLBC_Group
:
"

set PatternCPG_all_group ":
PREDEFINED CP GROUPS

ALL: $ALL_Group
ALLBC: $ALLBC_Group

OPERATOR DEFINED CP GROUPS

BCG1: $ALLBC_Group
:
"

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
cs_command_must_exist "cpgdef" 
cs_command_must_exist "cpgls"  
cs_command_must_exist "cpgrm"                   
cs_command_must_exist "hwcls" 
cs_command_must_exist "hwcadd" 
cs_command_must_exist "hwcrm" 

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
cs_command_must_exist "cpgdef" 
cs_command_must_exist "cpgls"  
cs_command_must_exist "cpgrm"                   
cs_command_must_exist "hwcls" 
cs_command_must_exist "hwcadd" 
cs_command_must_exist "hwcrm" 


###########################################################################
# Testcase start
###########################################################################


#SUT: Make sure that the HWC table is empty
cs_hwctable_must_be_empty

#SUT:  Make sure that the CP table is empty
cs_cptable_must_be_empty

#SUT:  Make sure that the CPG table is empty
cs_cpgtable_must_be_empty    

#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Make sure that the HWC table is empty
cs_hwctable_must_be_empty

#SUT2:  Make sure that the CP table is empty
cs_cptable_must_be_empty

#SUT2:  Make sure that the CPG table is empty
cs_cpgtable_must_be_empty    

#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


##############################
# SUT -> SUT2 synchronisation
##############################


#SUT: Add CP0 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotCP0 -n CPUB -t CP -y 0 -e A -d normal"]} {
    ath_tc_failed "SUT: hwcadd: addition of CP0 board failed"
}


#SUT: Add BC0 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotBC0 -n CPUB -t BC -y 1 -u 0 -d normal"]} {
    ath_tc_failed "SUT: hwcadd: addition of BC0 board failed"
}


#SUT: Add BC1 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotBC1 -n CPUB -t BC -y 1 -u 1 -d normal"]} {
    ath_tc_failed "SUT: hwcadd: addition of BC1 board failed"
}


#SUT: Verify that BC0 and BC1 boards have been added to the HWC table 
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternHWC_all $LastResponse]} {
    ath_tc_failed "SUT: hwcls: Printout does not match the expected one. \n"
}


#SUT: Define a CP group containing BC0 and BC1 boards  
if {[ath_send "cpgdef -cp $ALLBC_Group $GroupName"]} {
    ath_tc_failed "SUT: cpgdef: defining CP group failed"
}


#SUT: Verify that BCG1 group has been added to CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG_all_group $LastResponse]} {
    ath_tc_failed "SUT: cpgls: Printout does not match the expected one. \n"
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect

#Wait 10 seconds 
print "\nWaiting 10 seconds... \n"
after 10000

#SUT2: Verify that BC0 and BC1 boards have been added to the HWC table 
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternHWC_all $LastResponse]} {
    ath_tc_failed "SUT2: hwcls: Printout does not match the expected one. \n"
}


#SUT2: Verify that BCG1 group has been added to CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG_all_group $LastResponse]} {
    ath_tc_failed "SUT2: cpgls: Printout does not match the expected one. \n"
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Remove the CP group containing BC0 and BC1 boards  
if {[ath_send "cpgrm $GroupName"]} {
    ath_tc_failed "SUT: cpgrm: removing CP group failed"
}


#SUT: Verify that BCG1 group has been removed from CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG_all_nogroup $LastResponse]} {
    ath_tc_failed "SUT: cpgls: Printout does not match the expected one. \n"
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Verify that BCG1 group has been removed from CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG_all_nogroup $LastResponse]} {
    ath_tc_failed "SUT2: cpgls: Printout does not match the expected one. \n"
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Remove all added boards
if {[ath_send "hwcrm -m $Mag -s $SlotCP0 -f"]} {
    ath_tc_failed "SUT: hwcrm: removal of SCB-RB board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotBC0 -f"]} {
    ath_tc_failed "SUT: hwcrm: removal of CPU board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotBC1 -f"]} {
    ath_tc_failed "SUT: hwcrm: removal of APU board failed"
}


#SUT: Make sure that the HWC table is empty
cs_hwctable_must_be_empty

#SUT:  Make sure that the CP table is empty
cs_cptable_must_be_empty

#SUT: Make sure that the CPG table is empty
cs_cpgtable_must_be_empty


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect

#Wait 10 seconds 
print "\nWaiting 10 seconds... \n"
after 10000

#SUT2: Make sure that the HWC table is empty
cs_hwctable_must_be_empty

#SUT2:  Make sure that the CP table is empty
cs_cptable_must_be_empty

#SUT2: Make sure that the CPG table is empty
cs_cpgtable_must_be_empty


##############################
# SUT2 -> SUT synchronisation
##############################


#SUT2: Add CP0 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotCP0 -n CPUB -t CP -y 0 -e A -d normal"]} {
    ath_tc_failed "SUT2: hwcadd: addition of CP0 board failed"
}


#SUT2: Add BC0 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotBC0 -n CPUB -t BC -y 1 -u 0 -d normal"]} {
    ath_tc_failed "SUT2: hwcadd: addition of BC0 board failed"
}


#SUT2: Add BC1 board to the HWC table
if {[ath_send "hwcadd -m $Mag -s $SlotBC1 -n CPUB -t BC -y 1 -u 1 -d normal"]} {
    ath_tc_failed "SUT2: hwcadd: addition of BC1 board failed"
}


#SUT2: Verify that BC0 and BC1 boards have been added to the HWC table 
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternHWC_all $LastResponse]} {
    ath_tc_failed "SUT2: hwcls: Printout does not match the expected one. \n"
}


#SUT2: Define a CP group containing BC0 and BC1 boards  
if {[ath_send "cpgdef -cp $ALLBC_Group $GroupName"]} {
    ath_tc_failed "SUT2: cpgdef: defining CP group failed"
}


#SUT2: Verify that BCG1 group has been added to CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG_all_group $LastResponse]} {
    ath_tc_failed "SUT2: cpgls: Printout does not match the expected one. \n"
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Verify that BC0 and BC1 boards have been added to the HWC table 
ath_send "hwcls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternHWC_all $LastResponse]} {
    ath_tc_failed "SUT: hwcls: Printout does not match the expected one. \n"
}


#SUT: Verify that BCG1 group has been added to CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG_all_group $LastResponse]} {
    ath_tc_failed "SUT: cpgls: Printout does not match the expected one. \n"
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Remove the CP group containing BC0 and BC1 boards  
if {[ath_send "cpgrm $GroupName"]} {
    ath_tc_failed "SUT2: cpgrm: removing CP group failed"
}


#SUT2: Verify that BCG1 group has been removed from CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG_all_nogroup $LastResponse]} {
    ath_tc_failed "SUT2: cpgls: Printout does not match the expected one. \n"
}


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Verify that BCG1 group has been removed from CPG table  
ath_send "cpgls"
set LastResponse $ATH::LastResponse
if {[ath_match $PatternCPG_all_nogroup $LastResponse]} {
    ath_tc_failed "SUT: cpgls: Printout does not match the expected one. \n"
}


#Disconnect from SUT and connect to SUT2
print "\n\nDisconnecting from SUT...\n\n"
ath_send "exit"
cs_sut2_connect


#SUT2: Remove all added boards
if {[ath_send "hwcrm -m $Mag -s $SlotCP0 -f"]} {
    ath_tc_failed "SUT2: hwcrm: removal of SCB-RB board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotBC0 -f"]} {
    ath_tc_failed "SUT2: hwcrm: removal of CPU board failed"
}
if {[ath_send "hwcrm -m $Mag -s $SlotBC1 -f"]} {
    ath_tc_failed "SUT2: hwcrm: removal of APU board failed"
}


#SUT2: Make sure that the HWC table is empty
cs_hwctable_must_be_empty

#SUT2:  Make sure that the CP table is empty
cs_cptable_must_be_empty

#SUT2: Make sure that the CPG table is empty
cs_cpgtable_must_be_empty


#SUT2: Check and stop CS service
print "\nChecking and stopping CS service...\n"
cs_check_and_stop_service


#Disconnect from SUT2 and connect to SUT
print "\n\nDisconnecting from SUT2...\n\n"
ath_send "exit"
cs_sut_connect


#SUT: Make sure that the HWC table is empty
cs_hwctable_must_be_empty

#SUT:  Make sure that the CP table is empty
cs_cptable_must_be_empty

#SUT: Make sure that the CPG table is empty
cs_cpgtable_must_be_empty


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

