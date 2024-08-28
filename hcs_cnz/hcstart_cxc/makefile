# **********************************************************************
#
# Short description:
# Makefile template for XXX library
# **********************************************************************
#
# Ericsson AB 2010 All rights reserved.
# The information in this document is the property of Ericsson.
# Except as specifically authorized in writing by Ericsson, the receiver of this
# document shall keep the information contained herein confidential and shall protect
# the same in whole or in part from disclosure and dissemination to third parties.
# Disclosure and disseminations to the receivers employees shall only be made
# on a strict need to know basis.
#
# **********************************************************************
#
# Rev        Date         Name      What
# -----      -------      --------  --------------------------
#            2012-06-06   xsatdee   Created
#  R1F	     2013-03-27	  xgrecha   Changed Revision,Package for GIW	          
#  R1G 	     2013-04-16	  xgrecha   Changed revision,Package for OSU and added new requirement
#  R1H	     2013-04-21   xgrecha   Changed acs_hcs_reqid file for extra "-"	
#  R1J	     2013-05-02   xgrecha   Changed ME_ID length check to validate le 7        
#  R1K 	     2013-05-07   xgrecha   Changed for Result message logic,ME_ID log message, sensors 41 and 42 removal 
#  R1L	     2013-05-15   xgrecha   Changed for improvement
#  R1M	     2013-05-28   xfurull   Changed to validate -l option and add IO_HCRS_32
#  R1N       2013-06-08   xgrecha   Changed to verify baseline create
#  R1S	     2013-07-08   xgrecha   Changed for cosmetic changes in conf file and adding new checks for raidmgr
#  R1T	     2013-07-23   xgrecha   Changed for timeout increse IO_HCRS_28
#  R1U	     2013-09-12   xgrecha   Changed to spilt IOHCRS37 into IO_HCRS_37 and IO_HCRS_38 and modify result in non-verbose mode		
# ***********************************************************************

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common

include $(COMMON_ROOT)/common.mk 

CXC_NAME?=ACS_HCSBIN
CXC_NR?=CXC1371513_16
CXC_OLD_VER?=''
CXC_VER?= R1D

CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
export $(CXCDIR)

BASE_SW_VER ?= 4.0.0-R1E
CXC_BLK_NAME ?= acs_hcs
CAA_CMD_DIR = ../hcstartcmd_caa/
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*
export CAA_CMD_DIR

SUBSYSTEM_DIRS = \
		../hcstartcmd_caa\
		../hcstartadm_caa
.PHONY: all
all:
		$(SILENT)$(ECHO) 'Implementing make all ...'

.PHONY: release
#release: all rpm_package sdp_package ap_sdk metrics
release: all rpm_package  

.PHONY: nightbuild
nightbuild:   distclean all rpm_package sdp_package esm_package copy_package  documentation metrics doc_package


.PHONY: documentation
documentation:
	$(call doc_creation)

#CPPCKECK_SRCS = ../hcscmd_caa/src/*.cpp ../hcsadm_caa/src/*.cpp
#CPPCKECK_INCS = -I../hcscmd_caa/inc -I../hcsadm_caa/inc

.PHONY: cppcheck_m
cppcheck_m:
	$(SILENT)$(ECHO) 'Implementing cppcheck ...'
#	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))

#CCCC_SRCS = ../hcscmd_caa/src/* ../hcsadm_caa/src/*

.PHONY: cccc_m
cccc_m:
	$(SILENT)$(ECHO) 'Implementing cccc_m ...'
#	$(call cccc_doc_creation,$(CCCC_SRCS))

#SLOCCOUNT_SRCS = ../hcscmd_caa/ ../hcsadm_caa/ 

.PHONY: sloccount_m
sloccount_m:
	$(SILENT)$(ECHO) 'Implementing sloc count ...'
#	$(call sloccount_doc_creation,$(SLOCCOUNT_SRCS))

.PHONY: metrics
metrics: cppcheck_m cccc_m sloccount_m

	
.PHONY: clean
clean:
		$(SILENT)$(ECHO) 'make clean ...'
		$(call FX_CLEAN_TMP)

.PHONY: distclean
distclean:
	        $(SILENT)$(ECHO) 'Implementing distclean ...'
		$(call FX_CLEAN_TMP)

.PHONY: rpm_package
rpm_package:
	$(call rpm_file_creation,hcs.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))

.PHONY: sdp_package
sdp_package:
	$(call sdp_creation_US1,$(CXCPATH),$(CXC_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
	$(call up_creation,$(CXC_NAME),$(CXC_NR),$(CXC_VER),$(CXC_OLD_VER),$(CXCPATH),$(BASE_SW_VER))
.PHONY: esm_package
esm_package:
	$(call esm_pkg_creation,$(CXCPATH),$(CXC_BLK_NAME),$(CXC_VER),$(BASE_SW_VER),$(CXC_NR),$(CXC_NAME))
                      
.PHONY: upload_package 
upload_package: 
	$(call upload_package,$(CXC_BLK_NAME),$(CXC_NR),$(CXC_NAME),$(CXC_VER))

.PHONY: tar_package
tar_package:
	$(SILENT)$(ECHO) 'TAR packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

.PHONY: copy_package
copy_package:
	$(call copy_file_to_integration,$(RPMDIR)/$(CXC_NAME)-$(CXC_NR)-$(CXC_VER).x86_64.rpm)
	$(call copy_file_to_integration,$(CXC_NAME)-$(CXC_NR)-$(CXC_VER)-documentation.tar)

.PHONY: doc_package
doc_package: documentation metrics
	$(SILENT)$(ECHO) 'DOC and Metrics packaging ...'
	$(call doc_tarfile_creation,$(CXC_NAME)-$(CXC_NR)-$(CXC_VER)-documentation.tar)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

