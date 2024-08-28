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
#            2011-01-03   xmalsha   Created
#                                                            
# *********************************************************************

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

include $(COMMON_ROOT)/common.mk

CXC_NAME?=ACS_LCTBIN
CXC_NR?=CXC1371491_19
#CXC_OLD_VER?='R1C|R1B|R1A12|R1A11|R1A10|R1A09|R1A08|R1A07|R1A06|R1A05|R1A04|R1A03|R1A02|R1A01'
CXC_VER?=R1A02
CXC_BLK_NAME?=acs_lct
CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
BASE_SW_VER?=3.7.0-R1A
export $(CXCDIR)
CAA_CMD_DIR = ../lctcmd_caa/
export CAA_CMD_DIR
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*
SUBSYSTEM_DIRS = \
		../lctcmd_caa\
		../lctadm_caa

.PHONY: all
all:
		@for i in $(SUBSYSTEM_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			echo "cd $$i" ; \
			cd $$i ; \
			$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) all) ;\
			if [ $$? != 0 ] ; \
				then $(ECHO) "Error: please see the printout !!!";\
				exit 3;\
			fi;\
			echo "+++++++++++++++++" ; \
		done

.PHONY: release
release: all rpm_package sdp_package esm_package ap_sdk metrics

.PHONY: nightbuild
nightbuild: del_update  distclean all rpm_package sdp_package esm_package copy_package documentation metrics doc_package

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(CXC_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(CXC_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(CXC_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))


.PHONY: documentation
documentation:
	$(call doc_creation)

CPPCKECK_SRCS = ../lctcmd_caa/src/*.cpp ../lctadm_caa/src/*.cpp
CPPCKECK_INCS = -I../lctcmd_caa/inc -I../lctadm_caa/inc

.PHONY: cppcheck_m
cppcheck_m:
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))

CCCC_SRCS = ../lctcmd_caa/src/* ../lctadm_caa/src/*

.PHONY: cccc_m
cccc_m:
	$(call cccc_doc_creation,$(CCCC_SRCS))

SLOCCOUNT_SRCS = ../lctcmd_caa/ ../lctadm_caa/ 

.PHONY: sloccount_m
sloccount_m:
	$(call sloccount_doc_creation,$(SLOCCOUNT_SRCS))


.PHONY: metrics
metrics: cppcheck_m cccc_m sloccount_m

	
.PHONY: clean
clean:
		@for i in $(SUBSYSTEM_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			 echo "cd $$i" ; \
			 cd $$i ; \
			 $(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) clean ) ;\
			 echo "+++++++++++++++++" ; \
		done
		$(call FX_CLEAN_TMP)

.PHONY: distclean
distclean:
		@for i in $(SUBSYSTEM_DIRS) ; do \
			(echo "+++++++++++++++++" ; \
			 echo "cd $$i" ; \
			 cd $$i ; \
			 $(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) ACS_ROOT=$(ACS_ROOT) distclean ) ;\
			 echo "+++++++++++++++++" ; \
		done
		$(call FX_CLEAN_TMP)

.PHONY: rpm_package
rpm_package:
	$(call rpm_file_creation,lct.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))

.PHONY: sdp_package
sdp_package:
	$(call sdp_creation_US3,$(CXCPATH),$(CXC_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
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


