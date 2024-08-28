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
# -----      ----------   --------  --------------------------
#            2010-11-22   teiggam   Created
#                                                            
# **********************************************************************

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common

include $(COMMON_ROOT)/common.mk 

CXC_NAME?=ACS_SSUBIN
CXC_NR?=CXC1371525_15
CXC_VER?=R1C
CXC_OLD_VER?='R1A01|R1A04|R1B01|R1B'
BASE_SW_VER?=4.1.0-R1A
CXC_BLK_NAME ?= acs_ssu

CURDIR = $(shell pwd)
CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
export CXCDIR

FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH= ../ssuadm_caa/src ../ssuadm_caa/inc \
		 ../ssucmd_caa/src ../ssucmd_caa/inc

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

CAA_API_DIR = ../ssuadm_caa/inc
export CAA_API_DIR

CAA_DIRS = \
		../ssuadm_caa
#../ssucmd_caa

.PHONY: all
all: 
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) all) ;\
		if [ $$? != 0 ]; \
		then $(ECHO) "Error: please see the printout!!!";\
		exit $$?;\
		fi;\
		echo "+++++++++++++++++" ; \
		done
.PHONY: release
release: all rpm_package sdp_package esm_package tar_package ap_sdk documentation metrics

.PHONY: nightbuild
nightbuild: del_update distclean all rpm_package sdp_package esm_package  documentation metrics doc_package copy_package 

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

.PHONY: documentation
documentation:
		$(call doc_creation)

#CPPCKECK_SRCS = ../ssuadm_caa/src/*.cpp ../ssucmd_caa/src/*.cpp
CPPCKECK_SRCS = ../ssuadm_caa/src/acs_ssu_monitor.cpp ../ssuadm_caa/src/acs_ssu_svc_loader.cpp ../ssuadm_caa/src/acs_ssu_disk_mon.cpp ../ssuadm_caa/src/acs_ssu_aeh.cpp ../ssuadm_caa/src/acs_ssu_path.cpp\
../ssuadm_caa/src/acs_ssu_alarm_handler.cpp ../ssuadm_caa/src/acs_ssu_perf_mon.cpp ../ssuadm_caa/src/acs_ssu_memory_counter.cpp ../ssuadm_caa/src/acs_ssu_counter.cpp ../ssuadm_caa/src/acs_ssu_file_handle_counter.cpp \
../ssuadm_caa/src/acs_ssu_folder_quota_mon.cpp ../ssuadm_caa/src/acs_ssu_folder_quota.cpp ../ssuadm_caa/src/acs_ssu_execute.cpp  ../ssuadm_caa/src/acs_ssu_service.cpp \
../ssucmd_caa/src/acs_ssu_param.cpp

CPPCKECK_INCS = -I../ssuadm_caa/inc 
# -I../ssucmd_caa/inc
#../ssuadm_caa/src/acs_ssu_command_handler.cpp ../ssucmd_caa/src/acs_ssu_ssucfg.cpp ../ssucmd_caa/src/acs_ssu_ssuls.cpp

.PHONY: cppcheck_m
cppcheck_m:
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))

#CCCC_SRCS = ../ssuadm_caa/src/* ../ssucmd_caa/src/*
CCCC_SRCS = ../ssuadm_caa/src/acs_ssu_monitor.cpp ../ssuadm_caa/src/acs_ssu_svc_loader.cpp ../ssuadm_caa/src/acs_ssu_disk_mon.cpp ../ssuadm_caa/src/acs_ssu_aeh.cpp ../ssuadm_caa/src/acs_ssu_path.cpp\
../ssuadm_caa/src/acs_ssu_alarm_handler.cpp ../ssuadm_caa/src/acs_ssu_perf_mon.cpp ../ssuadm_caa/src/acs_ssu_memory_counter.cpp ../ssuadm_caa/src/acs_ssu_counter.cpp ../ssuadm_caa/src/acs_ssu_file_handle_counter.cpp \
../ssuadm_caa/src/acs_ssu_folder_quota_mon.cpp ../ssuadm_caa/src/acs_ssu_folder_quota.cpp ../ssucmd_caa/src/acs_ssu_execute.cpp ../ssuadm_caa/src/acs_ssu_service.cpp \
../ssuadm_caa/src/acs_ssu_param.cpp

#../ssucmd_caa/src/acs_ssu_ssucfg.cpp ../ssucmd_caa/src/acs_ssu_ssuls.cpp ../ssuadm_caa/src/acs_ssu_command_handler.cpp

.PHONY: cccc_m
cccc_m:
	$(call cccc_doc_creation,$(CCCC_SRCS))

SLOCCOUNT_SRCS = ../ssuadm_caa/ 

#../ssucmd_caa/

.PHONY: sloccount_m
sloccount_m:
	$(call sloccount_doc_creation,$(SLOCCOUNT_SRCS))

.PHONY: metrics
metrics: cppcheck_m cccc_m sloccount_m
		
.PHONY: clean
clean:
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		 echo "cd $$i" ; \
		 cd $$i ; \
		 $(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) clean ) ;\
		 echo "+++++++++++++++++" ; \
	done
	$(call FX_CLEAN_TMP)

.PHONY: distclean
distclean:
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) distclean ) ;\
		echo "+++++++++++++++++" ; \
	done
	$(call FX_CLEAN_TMP)

.PHONY: rpm_package
rpm_package:
	$(call rpm_file_creation,ssu.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))

.PHONY: sdp_package
sdp_package:
	$(call sdp_creation_US3,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
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
#	$(call tar_creation,$(BLOCK_NAME),$(CXC_NR),$(CXC_VER))
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

		
