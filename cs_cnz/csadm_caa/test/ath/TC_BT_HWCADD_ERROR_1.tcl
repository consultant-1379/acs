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
# :Title        Command hwcadd error codes
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
# Test hwcadd error codes
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

set target_modified 1;  # Inform the CleanUp procedure that it has to clean up
                        # the modified exchange data.
                        
# Make sure that the HWC table is empty
cs_hwctable_must_be_empty

###########################################################################
# Testcase start
###########################################################################

AutoFail OFF


# Test error codes

# Incorrect usage
# Exit code: 2
cs_send_cmd_exp_errcode "hwcadd -m" 2

# Illegal option
# Exit code: 5
cs_send_cmd_exp_errcode "hwcadd -q" 5

# Incorrect DHCP request method 
# Exit code: 29
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 5 -n CPUB -t CP -y 1 -e A -d nodhcp" 29

# Incorrect functional board name
# Exit code: 23
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 5 -n CPBOARD -t CP -y 1 -e A -d normal" 23

# Incorrect IP alias
# Exit code: 27
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 5 -n CPUB -t CP -y 1 -e A -a 1000.11.12.13,10.11.13.13 -b 255.255.255.0,255.255.3.0 -d normal" 27

# Incorrect magazine address
# Exit code: 21
cs_send_cmd_exp_errcode "hwcadd -m 2.4.5.5 -s 5 -n CPUB -t CP -y 1 -e A -d normal" 21

# Incorrect netmask
# Exit code: 28
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 5 -n CPUB -t CP -y 1 -e A -a 10.11.12.13,10.11.13.13 -b 255.255.255.0,255.255.3.0 -d normal" 28

# Incorrect sequence number
# Exit code: 25
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 5 -n CPUB -t BC -u A -y 1 -e A -d normal" 25

# Incorrect side
# Exit code: 26
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 5 -n CPUB -t CP -y 1 -e 1 -d normal" 26

# Incorrect slot position
# Exit code: 22
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 26 -n CPUB -t CP -y 1 -e A -d normal" 22

# Incorrect system number
# Exit code: 24
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 5 -n CPUB -t CP -y A -e A -d normal" 24

# Incorrect system type
# Exit code: 30
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 5 -n CPUB -t CPTYPE -y 1 -e A -d normal" 30

# Slot already occupied 
# Exit code: 15
if {[ath_send "hwcadd -m 2.4.0.5 -s 5 -n CPUB -t CP -y 1 -e A -d normal"]} {
    ath_tc_failed "hwcadd: adding board failed"
}
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 5 -n CPUB -t CP -y 1 -e A -d normal" 15

# Unable to connect to server
# Exit code: 117
cs_check_and_stop_service
cs_send_cmd_exp_errcode "hwcadd -m 2.4.0.5 -s 10 -n CPUB -t CP -y 1 -e A -d normal" 117

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

