#!/bin/csh
#\
exec ath $0

# The line(s) above enable this script to be executed from a file manager
# on Unix. They must end with LF (not by CRLF).

###########################################################################
# This is a Tcl/Expect test case script

###########################################################################
# Copyright 2010 Ericsson AB
# All rights reserved.

###########################################################################
# :Title        Command fddef error codes
# -------------------------------------------------------------------------
# :TestAct      FT        # One of FT, ST, BT, IT,....
# :SubSys       FMS       # Mandatory for FT, optional (leave empty) if ST
# :TestObj      CPF       # Block name if FT. Rob, Char, Stab or Maint if ST
# :Prepared     EAB/FTE/RT Name of script author
# :Approved     EAB/FTE/RT (My Boss)
# :Date         yyyy-mm-dd
# :Req          <comma-separated list of requirements tested>    # Optional
# :Ref          <Optional reference to a TC in an external TS or TI>
# :TCLTCS
# :TCLTCI
# :RegTest      Yes
# :Scope        Design Based
# :OS           Linux
# :APZ          No
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
# Test fddef error codes.
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
						
# Run preparation script
cs_preparation

# Start daemon
cs_start_service

# Verify that all needed commands are available
cs_command_must_exist "fddef"                    
cs_command_must_exist "hwcadd" 


# Make sure that the HWC table is empty
cs_hwctable_must_be_empty
# Make sure that the FD table is empty
cs_fdtable_must_be_empty


###########################################################################
# Testcase start
###########################################################################
 
AutoFail OFF


#Add AP board to the HWC table
if {[ath_send "hwcadd -m 2.4.0.5 -s 9 -n APUB -t AP -y 1"]} {
    ath_tc_failed "hwcadd: addition of CPU board failed"
}


#Add a function distribution for AP1
if {[ath_send "fddef -d MTAP -s CHS AP1"]} {
    ath_tc_failed "fddef: addition of FD failed"
}


# Test error codes

#AP is not defined
#Exit code: 113
cs_send_cmd_exp_errcode "fddef -d MTAP -s CHS AP2" 113

#Duplicated function
#Exit code: 39
cs_send_cmd_exp_errcode "fddef -d MTAP -s CHS AP1" 39

# Incorrect usage
# Exit code: 2
cs_send_cmd_exp_errcode "fddef -x" 2

# Illegal domain name
# Exit code: 42
cs_send_cmd_exp_errcode "fddef -d MTAPMTAP -s CHS AP1" 42

# Illegal service name
# Exit code: 38
cs_send_cmd_exp_errcode "fddef -d MTAP -s 1CHS AP1" 38

# Unable to connect to server
# Exit code: 117
cs_check_and_stop_service
# cs_send_cmd_exp_errcode "fddef -d MTAP -s CHT AP1" 117

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

proc CleanUp args {
    if {[info exists target_modified]} { # To avoid runtime error if the variable
                                         # wasn't declared before this procedure
                                         # was called.
                                                         
        print "Cleaning up...\n"
                                 
        if {$target_modified} {
            print "\n\nACTION: Connect a Telnet or SSH terminal session to the SUT\n\n"
            cs_sut_connect

            # Clean-up:
            cs_cleanup
            ath_send "exit"
        }
    }
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

