#!/bin/bash
#
# Author :XANJDIT/ZGXXNAV
#
# [Description: get csm version 
# [Parameters:Input as CXP and R-state
#    $1   - $cxp
#    $2   - $rstate_ver
#
# [Return:
#   csm verstion
#  ex:
#   CXC1371427_7 R1A01  --> 7.0.0-1
#
##

ESM_SCRIPTS_DIR="$3//fcsc_cnz/fcsc_cxc/packages/esm"
. $ESM_SCRIPTS_DIR/commonESMfunctions
csm_vers=$(csm_version $1 $2)
echo $csm_vers
