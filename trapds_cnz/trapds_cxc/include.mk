# **********************************************************************
#
# Short description:
# TRAPDS common.mk configuration file
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
#            2010-10-01   xmikhal   Created
#            2010-10-18   xminaon   Updated with Eclipse fix
#            2016-03-30   xsunach   CNI 607 fix
# **********************************************************************

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common

export ACS_ROOT
export COMMON_ROOT

# Handling Eclipse clearcase paths
ifeq ($(ECLIPSE),1)
IODEV_VOB := $(COMMON_ROOT)
VOB_PATH := $(ACS_ROOT)
else
IODEV_VOB = $(COMMON_ROOT)
VOB_PATH = $(ACS_ROOT)
endif
CNZ_NAME = trapds_cnz
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)

CXC_NAME ?= ACS_TRAPDSBIN

CXC_NAME_PATH ?= trapds_cxc
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME_PATH)
CXCDIR = $(CNZ_PATH)/$(CXC_NAME_PATH)
CXC_NR ?= CXC1371470_9
VERSION = 2.2-0
CXC_OLD_VER ?= 'R1A02|R1A03|R1A04|R1A05|R1B'
CXC_VER ?= R1C
BASE_SW_VER ?= 3.6.0-R1A
CXC_BLK_NAME ?= acs_trapds

BLOCK_NAME = ACS_TRAPDSBIN
DOXYGENCONFIGFILE = $(CXCDIR)/doc/cch_doxygen.cfg
include $(COMMON_ROOT)/common.mk
LIBAPI_DIR = $(CNZ_PATH)/trapds_cxc/bin/lib_ext

LIBAPI_EXT_DIR = $(CNZ_PATH)/trapds_cxc/bin/lib_ext
LIBAPI_INT_DIR = $(CNZ_PATH)/trapds_cxc/bin/lib_int

CCCC_FLAGS += --xml_outfile=$(CCCC_OUTPUT)/cccc.xml

FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

# Handling local changes in API during development by adding local cchapi inc
# before IO_Developments vob
CFLAGS += -I../trapdsapi_caa/inc -I $(CNZ_PATH)/trapdsapi_caa/inc

# Handling optimization for release builds
ifneq ($(DEBUG),1)
CFLAGS += -O3
endif

