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
#            2011-07-27   tcstanu   Created
#            2018-07-03   zbhegna   Added 
#                                                            
# ***********************************************************************
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

include $(COMMON_ROOT)/common.mk

CXC_NAME?=ACS_ACABIN
CXC_NR?=CXC1371516_9
CXC_VER?= R1E
BASE_SW_VER ?= 3.6.0-R1A
CXC_BLK_NAME ?= acs_aca

FX_CLEAN_CONTRIB := find ../ -name \*.contrib* | xargs -I {} rm {}
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

CURDIR = $(shell pwd)
CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
export CXCDIR



DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH=../acaapi_caa/src ../acaapi_caa/inc_ext ../acaapi_caa/inc_int ../acaadm_caa/src ../acaadm_caa/inc \
			../acacmd_caa/src ../acacmd_caa/inc
		
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH


CAA_API_DIR = ../acaapi_caa/inc_int ../acaapi_caa/inc_ext  
export CAA_API_DIR

CAA_DIRS = \
		../acaapi_caa\
		../acaadm_caa\
		../acacmd_caa \
#		../acaapi_caa


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
release: all rpm_package ap_sdk documentation metrics
 

.PHONY: nightbuild
nightbuild: del_update distclean all rpm_package sdp_package esm_package documentation metrics doc_package copy_package 

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

.PHONY: documentation
documentation:
#	$(call doc_creation)
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

CPPCKECK_SRCS = ../acaapi_caa/src/*.cpp ../acaadm_caa/src/*.cpp ../acacmd_caa/src/*.cpp
CPPCKECK_INCS = -I../acaapi_caa/inc_int -I../acaapi_caa/inc_ext -I../acaadm_caa/inc -I../acacmd_caa/inc

.PHONY: cppcheck_m
cppcheck_m:
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))


CCCC_SRCS = ../acaapi_caa/src/* ../acaadm_caa/src/* ../acacmd_caa/src/*

.PHONY: cccc_m
cccc_m:
	$(call cccc_doc_creation,$(CCCC_SRCS))

SLOCCOUNT_SRCS = ../acaapi_caa/ ../acaadm_caa/ ../acacmd_caa/

.PHONY: sloccount_m
sloccount_m:
	$(call sloccount_doc_creation,$(SLOCCOUNT_SRCS))

.PHONY: metrics
metrics: cppcheck_m cccc_m sloccount_m

.PHONY: clean
clean:
	$(call FX_CLEAN_CONTRIB)
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
	$(call rpm_file_creation,aca.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))
#	$(SILENT)$(ECHO) '**********************************'
#	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
#	$(SILENT)$(ECHO) '**********************************'

.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) 'SDP packaging ...'
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
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

.PHONY: copy_package
copy_package:
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'


.PHONY: doc_package
doc_package: documentation metrics
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'



.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'
	
		

