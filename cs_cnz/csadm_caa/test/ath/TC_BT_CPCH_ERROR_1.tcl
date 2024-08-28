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
# :Title        Command cpch error codes
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
# Test cpch error codes
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
                        
#Start CS service
print "\nStarting CS service...\n"
cs_start_service

# Verify that all needed commands are available
cs_command_must_exist "cpch"

###########################################################################
# Testcase start
###########################################################################
 
AutoFail OFF

# Test error codes

#Set the system to Single-CP
ath_send "immcfg -a isMultipleCPSystem=0 CSId=1"

# Illegal command in this system configuration
# Exit code: 115
cs_send_cmd_exp_errcode "cpch" 115

#Set the system to Multi-CP
ath_send "immcfg -a isMultipleCPSystem=1 CSId=1"

# Incorrect usage
# Exit code: 2
cs_send_cmd_exp_errcode "cpch -q" 2

# CP is not defined
# Exit code: 118
cs_send_cmd_exp_errcode "cpch -a TSC-CP1 1001" 118

# Incorrect alias
# Exit code: 32
cs_send_cmd_exp_errcode "cpch -a TSC-CP1ALIAS 1001" 32

# Incorrect APZ system
# Exit code: 33
cs_send_cmd_exp_errcode "cpch -s APZ11111 1001" 33

# Incorrect CP type
# Exit code: 43
cs_send_cmd_exp_errcode "cpch -t 11111 1001" 43

# Reserved name or occupied alias
# Exit code: 20
cs_send_cmd_exp_errcode "cpch -a CP111 1001" 20

# Unable to connect to server
# Exit code: 117
# if {[ath_send "hwcadd -m 2.4.0.5 -s 10 -n CPUB -t CP -y 1 -e A -d normal"]} {
#    ath_tc_failed "hwcadd: addition of board failed"
# }
cs_check_and_stop_service
# cs_send_cmd_exp_errcode "cpch -a TSC-CP1 1001" 115


print "\n\nACTION: Disconnect the terminal session.\n\n"
ath_send "exit"

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
                     
            #Set the system to Single-CP
            ath_send "immcfg -a isMultipleCPSystem=0 CSId=1"

            #Stop CS service
			print "\nStopping CS service...\n"
			cs_stop_service
			
            ath_send "exit"
        }
    }
    print "\n\nClean up done.\n"
}

#**************************************************************************
# Report Test Case Result
#**************************************************************************

# If we've reached this far we can safely assume that the test case has
# passed, assuming we'd have exited already if it would not have passed.

print "\n\nACTION: Report test case result \"PASSED\".\n\n"
ath_tc_passed


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

