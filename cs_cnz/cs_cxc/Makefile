# **********************************************************************
#
# Short description:
# Makefile for CS
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
#            2010-09-23   xmikhal   Created
#            2010-10-05   xmikhal   Updated with target for building
#                                   service and commands only
#            2010-11-10   xpiokry   Updated to follow DR.
#            2011-01-12   xmaglex	Added targets: prebuild, postbuild, metrics and documentation
#            2011-06-14   xpiokry	Added target: ap_sdk - not implemented
#            2011-08-20   xpiokry	Added SDP creation functionality
#            2012-11-26	  estevol	Added block UP creation functionality
#            2023-04-27   zsgrain   Added support for TIPC networks in vBSC VMware
#            2024-05-14   xcsrpad   Support for flexible naming feature
# **********************************************************************

include ./include.mk

CXC_NAME = ACS_CSBIN
CXC_NR ?= CXC1371495_20
CXC_VER ?= R1A01
#CXC_OLD_VER = R1A02
BASE_SW_VER ?= 4.0.0-R1A
CXC_BLK_NAME?=acs_cs
CURDIR = $(shell pwd)
CXCPATH = $(CURDIR)
CXCDIR = $(CXCPATH)
export CXCDIR
export CXCPATH
CAA_DIRS = ../csadm_caa/src/imm_mapper ../csadm_caa/csprot ../csapi_caa ../cscmd_caa ../csadm_caa
export CAA_DIRS

LIBAPI_EXT_DIR = $(CXCPATH)/bin/lib_ext
LIBAPI_INT_DIR = $(CXCPATH)/bin/lib_int
export LIBAPI_EXT_DIR
export LIBAPI_INT_DIR

RPM_SPEC = cs.spec
#SDP_FILE = $(BLOCK_NAME)-$(CXC_NR)-$(CXC_VER).x86_64.sdp

DOCDIR = doc
DOXYGEN_OUTPUT = $(DOCDIR)/docgen
BLOCK_NAME = $(CXC_NAME)
BLOCK_VERSION = $(CXC_VER)
BLOCK_SEARCH = ../csadm_caa/csprot/src ../csadm_caa/csprot/inc ../csapi_caa/src ../csapi_caa/inc_int ../csapi_caa/inc_ext ../cscmd_caa/src ../cscmd_caa/inc ../csadm_caa/src ../csadm_caa/inc
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

SLOCCOUNT_SRCS = ../csadm_caa/csprot/ ../csapi_caa/ ../cscmd_caa/ ../csadm_caa/
CCCC_SRCS = ../cscmd_caa/src/imm_mapper ../csadm_caa/csprot/src/* ../csapi_caa/src/* ../cscmd_caa/src/* ../csadm_caa/src/*
CPPCKECK_SRCS = ../cscmd_caa/src/imm_mapper/*.cpp ../csadm_caa/csprot/src/*.cpp ../csapi_caa/src/*.cpp ../cscmd_caa/src/*.cpp ../csadm_caa/src/*.cpp
CPPCKECK_INCS = ../cscmd_caa/inc/imm_mapper -I../csadm_caa/csprot/inc -I../csapi_caa/inc -I../cscmd_caa/inc -I../csadm_caa/inc

FX_CLEAN_CONTRIB := find ../ -name *.contrib* | xargs -I {} rm {}
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*
	
#Create all CAA folders
.NOTPARALLEL:
.PHONY: all
all:
	@for i in $(CAA_DIRS) ; do \
	$(MAKE) -C $$i COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) all ;\
	if [ $$? != 0 ]; \
	then $(ECHO) "Error: please see the printout !!! $$?"; \
	exit 3; \
	fi; \
	done;


.PHONY: release
release: all rpm_package sdp_package esm_package #ap_sdk  tar_package documentation metrics

#Create sdp packages
.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'BUILDING SDP FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	$(call sdp_creation_US3,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
	$(call up_creation,$(CXC_NAME),$(CXC_NR),$(CXC_VER),$(CXC_OLD_VER),$(CXCPATH),$(BASE_SW_VER))
	
#Create rpm package
# NOTE: Renaming packages for now since buildrpm doesn't name them with revision
.PHONY: rpm_package
rpm_package:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'BUILDING RPM FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)	
	$(call rpm_file_creation,$(RPM_SPEC),$(BLOCK_NAME),$(CXC_NR),$(CXC_VER))

.PHONY: esm_package
esm_package:
	$(call esm_pkg_creation,$(CXCPATH),$(CXC_BLK_NAME),$(CXC_VER),$(BASE_SW_VER),$(CXC_NR),$(CXC_NAME))

.PHONY: upload_package
upload_package:
	$(call upload_package,$(CXC_BLK_NAME),$(CXC_NR),$(CXC_NAME),$(CXC_VER))

#Create TAR package
.PHONY: tar_package
tar_package:
	$(SILENT)$(ECHO) '-CALL TO TAR_PACKAGE COMMENTED. UNCOMMENT WHEN AVAILABLE IN IO_DEV COMMON.MK RELEASE'
#$(call tar_file_creation,$(BLOCK_NAME)-$(CXC_NR)-$(VERSION).x86_64.tar)

#Clean CAA folders build files
.PHONY: clean
clean:		
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'CLEANING OBJECT FILES FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	$(call FX_CLEAN_CONTRIB)
	$(call FX_CLEAN_TMP)	
	@for i in $(CAA_DIRS); do \
	cd $$i ; \
	$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) clean ;\
	cd $(CXCPATH); \
	done;

#Clean CAA folders binary files
.PHONY: distclean
distclean:
	@for i in $(CAA_DIRS); do \
	cd $$i ; \
	$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) distclean ;\
	cd $(CXCPATH); \
	done;
	$(SILENT) $(RM) $(RPMDIR)/$(BLOCK_NAME)-$(CXC_NR)-$(CXC_VER).x86_64.rpm
#	$(SILENT) $(RM) $(SDPDIR)/rpms/$(BLOCK_NAME)-$(CXC_NR)-$(CXC_VER).x86_64.rpm
#	$(SILENT) $(RM) $(SDPDIR)/$(SDP_FILE)

# Perform chain of code analysis tools
######################################################################################
# METRICS
# Perform chain of code analysis tools
######################################################################################
.PHONY: metrics
metrics: cppcheck_m cccc_m sloccount_m	

######################################################################################
# CPPCHECK
# - analysis tool for C/C++ code
######################################################################################

.PHONY: cppcheck_m
cppcheck_m:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'CPPCHECK FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(SEPARATOR_STR)
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))

######################################################################################
# CCCC
# - C and C++ Code Counter
######################################################################################

.PHONY: cccc_m
cccc_m:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'CCCC FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='	
	$(call cccc_doc_creation,$(CCCC_SRCS))
   
######################################################################################
# SLOC
# - tools for counting physical Source Lines of Code
######################################################################################

.PHONY: sloccount_m
sloccount_m:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'SLOC FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(call sloccount_doc_creation,$(SLOCCOUNT_SRCS))

######################################################################################
# DOCUMENTATION
# Generate HTML documentation with Doxygen for each CAA
######################################################################################	
.PHONY: documentation
documentation:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'DOCUMENTATION CHAIN FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	$(call doc_creation)
	$(NEW_LINE)
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)

######################################################################################
# AP_SDK synchronization
######################################################################################
.PHONY: ap_sdk
ap_sdk:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Synchronizing AP_SDK...'
	$(SILENT)$(ECHO) 'NOT IMPLEMENTED'
	$(NEW_LINE)

# Depends
.PHONY: depend
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^
	
# DO NOT DELETE THIS LINE -- make depend needs it
