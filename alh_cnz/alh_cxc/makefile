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
#            2011-10-28   xfabron   Created
#                                                            
# **********************************************************************

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

include $(COMMON_ROOT)/common.mk

CXC_NAME?=ACS_ALHBIN
CXC_NR?=CXC1371472_12
CXC_VER?=R1C
#CXC_OLD_VER?='R1C|R1B|R1A01|R1A02|R1A03|R1A04'
BASE_SW_VER?=3.9.0-R1A
CXC_BLK_NAME ?= acs_alh
CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
export CXCDIR
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH=../alhapi_caa/src ../alhapi_caa/inc_int ../alhapi_caa/inc_ext  ../alhcmd_caa/src ../alhcmd_caa/inc ../alhadm_caa/inc ../alhadm_caa/src
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH


CAA_API_DIR_INT = ../alhapi_caa/inc_int
export CAA_API_DIR_INT

CAA_API_DIR_EXT = ../alhapi_caa/inc_ext
export CAA_API_DIR_EXT

CAA_ADM_DIR = ../alhadm_caa/inc
export CAA_ADM_DIR

CAA_CMD_DIR = ../alhcmd_caa/inc
export CAA_CMD_DIR

CAA_DIRS = \
		../alhadm_caa ../alhcmd_caa #../alhapi_caa		# buil only the API and alh command

.PHONY: all
all: 
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) all) ;\
		if [ $$? != 0 ] ; \
		then $(ECHO) "Error: please see the printout !!!";\
		exit $$?;\
		fi;\
		echo "+++++++++++++++++" ; \
	done

.PHONY: release
release: all rpm_package sdp_package esm_package tar_package ap_sdk documentation metrics

.PHONY: nightbuild
nightbuild: del_update distclean all rpm_package sdp_package esm_package copy_package documentation metrics doc_package

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

.PHONY: documentation
documentation:
	$(call doc_creation)

CPPCKECK_SRCS = ../alhadm_caa/src/*.cpp  ../alhcmd_caa/src/*.cpp
CPPCKECK_INCS = -I../alhadm_caa/inc -I../alhcmd_caa/inc 

.PHONY: cppcheck_m
cppcheck_m:		
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))


CCCC_SRCS = ../alhadm_caa/src/* ../alhcmd_caa/src/*

.PHONY: cccc_m
cccc_m:		
	$(call cccc_doc_creation,$(CCCC_SRCS))


SLOCCOUNT_SRCS = ../alhadm_caa/ ../alhcmd_caa/ 

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
		if [ $$? != 0 ] ; \
		then $(ECHO) "Error: please see the printout !!!";\
		exit $$?;\
		fi;\
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
		if [ $$? != 0 ] ; \
		then $(ECHO) "Error: please see the printout !!!";\
		exit $$?;\
		fi;\
		echo "+++++++++++++++++" ; \
	done
	$(call FX_CLEAN_TMP)

.PHONY: rpm_package
rpm_package:
	$(call rpm_file_creation,alh.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))


.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) 'SDP packaging ...'
	$(call sdp_creation_US4,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
	$(call up_creation,$(CXC_NAME),$(CXC_NR),$(CXC_VER),$(CXC_OLD_VER),$(CXCPATH),$(BASE_SW_VER))

.PHONY: esm_package
esm_package:
	$(call esm_pkg_creation,$(CXCPATH),$(CXC_BLK_NAME),$(CXC_VER),$(BASE_SW_VER),$(CXC_NR),$(CXC_NAME))

.PHONY: upload_package
upload_package:
	$(call upload_package,$(CXC_BLK_NAME),$(CXC_NR),$(CXC_NAME),$(CXC_VER))

.PHONY: copy_package
copy_package:
	$(call copy_file_to_integration,$(RPMDIR)/$(CXC_NAME)-$(CXC_NR)-$(CXC_VER).x86_64.rpm)
	$(call copy_file_to_integration,$(CXC_NAME)-$(CXC_NR)-$(CXC_VER)-documentation.tar)


.PHONY: doc_package
doc_package: documentation metrics
	$(SILENT)$(ECHO) 'DOC and Metrics packaging ...'
	$(call doc_tarfile_creation,$(CXC_NAME)-$(CXC_NR)-$(CXC_VER)-documentation.tar)

.PHONY: tar_package
tar_package:
	$(SILENT)$(ECHO) 'TAR packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) $(MFLAGS) CXCDIR=$(CXCPATH) ap_sdk ) ;\
		if [ $$? != 0 ] ; \
		then $(ECHO) "Error: please see the printout !!!";\
		exit $$?;\
		fi;\
		echo "+++++++++++++++++" ; \
	done

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'
		

