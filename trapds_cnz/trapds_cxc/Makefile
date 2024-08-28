# **********************************************************************
#
# Short description:
# Makefile for TRAPDS
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
#            2014-03-20   eanform   Created
#          
# **********************************************************************

include include.mk

CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common

CURDIR = $(shell pwd)
CXCPATH = $(CURDIR)
CXCDIR = $(CXCPATH)
export CXCDIR
export CXCPATH

FX_CLEAN_CONTRIB := find ../ -name *.contrib* | xargs -I {} rm {}

CAA_DIRS = ../trapdsapi_caa ../trapdsadm_caa
export CAA_DIRS

RPM_SPEC = trapds.spec
SDP_FILE = $(BLOCK_NAME)-$(CXC_NR)-$(CXC_VER).x86_64.sdp

DOCDIR = doc
DOXYGEN_OUTPUT = $(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH = ../trapdsapi_caa/src ../trapdsapi_caa/inc_int ../trapdsapi_caa/inc_ext ../trapdscmd_caa/src ../trapdscmd_caa/inc ../trapdsadm_caa/src ../trapdsadm_caa/inc #../trapdsadm_caa/trapdsprot/src ../trapdsadm_caa/trapdsprot/inc 
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

SLOCCOUNT_SRCS = ../trapdsapi_caa/ ../trapdscmd_caa/ ../trapdsadm_caa/  #../trapdsadm_caa/trapdsprot/
CCCC_SRCS = ../trapdsapi_caa/src/* ../trapdscmd_caa/src/* ../csadm_caa/src/* #../trapdsadm_caa/trapdsprot/src/*
CPPCKECK_SRCS = ../trapdsapi_caa/src/*.cpp ../trapdscmd_caa/src/*.cpp ../trapdsadm_caa/src/*.cpp #../trapdsadm_caa/trapdsprot/src/*.cpp
CPPCKECK_INCS = -I../trapdsapi_caa/inc -I../trapdscmd_caa/inc -I../trapdsadm_caa/inc  #-I../trapdsadm_caa/trapdsprot/inc
	
#Create all CAA folders
.NOTPARALLEL:
.PHONY: all
all:
	@for i in $(CAA_DIRS) ; do \
	$(MAKE) -C $$i $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) all ;\
	if [ $$? != 0 ]; \
	then $(ECHO) "Error: please see the printout !!!"; \
	exit 3; \
	fi; \
	done;

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

#Create all delivery file
.PHONY: release
release: all rpm_package #ap_sdk sdp_package esm_package tar_package documentation metrics

#Create sdp packages for all CAAs
.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'BUILDING SDP FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	$(call sdp_creation_US3,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
	$(call up_creation,$(CXC_NAME),$(CXC_NR),$(CXC_VER),$(CXC_OLD_VER),$(CXCPATH),$(BASE_SW_VER))
#Create rpm packages for all CAAs
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
	$(NEW_LINE)
	@for i in $(CAA_DIRS); do \
	cd $$i ; \
	$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) clean ; \
	done;
	$(call FX_CLEAN_TMP)

#Clean CAA folders binary files
.PHONY: distclean
distclean:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'CLEANING BINARIES FILES FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	$(call FX_CLEAN_CONTRIB)
	$(NEW_LINE)
	@for i in $(CAA_DIRS); do \
	cd $$i ; \
	$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) distclean ; \
	done;
	$(call FX_CLEAN_TMP)
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

