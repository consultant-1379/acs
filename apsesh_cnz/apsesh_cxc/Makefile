# **********************************************************************
#
# Short description:
# Makefile template for APSESH
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
#            2011-04-26   xmaglex   Created
#            2011-08-20   xpiokry	Added SDP creation functionality          
# **********************************************************************
# See the common.mk for common variables and macros

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

include $(COMMON_ROOT)/common.mk


CXC_NAME ?= ACS_APSESHBIN
CXC_NR ?= CXC1371367_9
CXC_VER ?= R1G
CXC_OLD_VER ?= ''
BASE_SW_VER ?=3.6.0-R1A
CXC_BLK_NAME ?= acs_apsesh
RPM_SPEC = apsesh.spec

CXCPATH = $(CURDIR)
CXCDIR = $(CXCPATH)
export CXCDIR
export CXCPATH

DOXYGEN_OUTPUT = $(DOCDIR)/docgen
BLOCK_NAME = $(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH=../apsesh_caa/src ../apsesh_caa/inc
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

CPPCKECK_SRCS = ../apsesh_caa/src/*.cpp 
CPPCKECK_INCS = -I../apsesh_caa/inc 
CCCC_SRCS = ../apsesh_caa/src/* 
SLOCCOUNT_SRCS = ../apsesh_caa/ 

CAA_ADM_DIR=	../apsesh_caa
export CAA_ADM_DIR

SUBSYSTEM_DIRS = $(CAA_ADM_DIR)

TEST_DIR = ../apsesh_caa/test/jtp_initiator

FX_CLEAN_CONTRIB := find ../ -name *.contrib* | xargs -I {} rm {}
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

######################################################################################
# ALL
# Default target to build the binary of this block
######################################################################################	
	
.PHONY: all
all:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'BUILDING CHAIN FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)	
		@for i in $(SUBSYSTEM_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			echo "cd $$i" ; \
			cd $$i ; \
			$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) all) ;\
			if [ $$? != 0 ]; \
			then $(ECHO) "Error: please see the printout!!!";\
			exit $$?; \
			fi;\
			echo "+++++++++++++++++" ; \
		done
	$(NEW_LINE)
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)

######################################################################################
# del_update
# Perform delivery update actions like rename sdp,rpm,esm pacakages
######################################################################################

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

######################################################################################
# 
# Perform delivery update actions like rename sdp,rpm,esm pacakages 
######################################################################################


######################################################################################
# RPM_PACKAGE
# Build a rpm package for delivery. 
######################################################################################	
.PHONY: rpm_package
rpm_package:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'BUILDING RPM FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)	
	$(call rpm_file_creation,$(RPM_SPEC),$(BLOCK_NAME),$(CXC_NR),$(CXC_VER))

######################################################################################
# SDP_PACKAGE
# Build a sdp package for delivery. 
######################################################################################	
.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'BUILDING SDP/UP FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	$(call sdp_creation_US3,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
	$(call up_creation,$(CXC_NAME),$(CXC_NR),$(CXC_VER),$(CXC_OLD_VER),$(CXCPATH),$(BASE_SW_VER))


######################################################################################
# ESM_PACKAGE
# Build a ESM package for delivery. 
######################################################################################	
.PHONY: esm_package
esm_package:
	$(call esm_pkg_creation,$(CXCPATH),$(CXC_BLK_NAME),$(CXC_VER),$(BASE_SW_VER),$(CXC_NR),$(CXC_NAME))

.PHONY: upload_package
upload_package:
	$(call upload_package,$(CXC_BLK_NAME),$(CXC_NR),$(CXC_NAME),$(CXC_VER))

######################################################################################
# TAR_PACKAGE
# Build a tar package
######################################################################################
.PHONY: tar_package
tar_package:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'TAR PACKAGING FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) 'NOT IMPLEMENTED'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	#$(call tar_file_creation,$(BLOCK_NAME)-$(CXC_NR)-$(VERSION).tar)

######################################################################################
# CLEAN
# Delete all files in CAA object directories
######################################################################################
.PHONY: clean
clean:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'CLEANING OBJECT FILES FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	$(call FX_CLEAN_CONTRIB)
	@for i in $(SUBSYSTEM_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			echo "cd $$i" ; \
			cd $$i ; \
			$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) clean ) ;\
			if [ $$? != 0 ]; \
			then $(ECHO) "Error: please see the printout!!!";\
			exit $$?; \
			fi;\
			echo "+++++++++++++++++" ; \
	  done
	$(NEW_LINE)
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	$(call FX_CLEAN_TMP)
######################################################################################
# DISTCLEAN
# Delete binaries related to each CAA
######################################################################################
.PHONY: distclean
distclean:
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'CLEANING BINARIES FOR BLOCK $(BLOCK_NAME)'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	@for i in $(SUBSYSTEM_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			echo "cd $$i" ; \
			cd $$i ; \
			$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) distclean ) ;\
			if [ $$? != 0 ]; \
			then $(ECHO) "Error: please see the printout!!!";\
			exit $$?; \
			fi;\
			echo "+++++++++++++++++" ; \
	    done
	    $(SILENT) $(RM) $(RPMDIR)/$(BLOCK_NAME)-$(CXC_NR)-$(CXC_VER).x86_64.rpm
	$(NEW_LINE)
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)
	$(call FX_CLEAN_TMP)
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
# RELEASE
# Build, package, documents and metrics
######################################################################################
.PHONY: release
release: all rpm_package sdp_package esm_package # tar_package ap_sdk documentation metrics

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
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))

######################################################################################
# CCCC
# - C and C++ Code Counter
######################################################################################

.PHONY: cccc_m
cccc_m:
	check_m:
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
# TEST INITATOR
# Build a test JTP initiator
######################################################################################
.PHONY: testinitiator
testinitiator: 
	$(SILENT)$(ECHO) '===================================================================='
	$(SILENT)$(ECHO) 'BUILDING CHAIN FOR APSESH TEST APP'
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)	
	@for i in $(TEST_DIR) ; do \
	   $(MAKE) -C $$i $(MFLAGS) CXCDIR=$(CXCPATH) all; \
	   if [ $$? != 0 ]; \
	      then $(ECHO) "Error: please see the printout !!!"; \
	      exit 3; \
	      fi; \
	   done;
	$(NEW_LINE)
	$(SILENT)$(ECHO) '===================================================================='
	$(NEW_LINE)


