# **********************************************************************
#
# Short description:
# CS common.mk configuration file
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
# **********************************************************************
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
# END

# Handling Eclipse clearcase paths
ifeq ($(ECLIPSE),1)
IODEV_VOB := /cc/IO_Developments
VOB_PATH := /cc/cm4ap/$(VOB_NAME)
else
IODEV_VOB = /vobs/IO_Developments
VOB_PATH = $(ACS_ROOT)
endif
CNZ_NAME = cs_cnz
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)

#CXC_NAME = cs_cxc
#CURDIR = $(shell pwd)
#CXCPATH=$(CURDIR)
#CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
#CXCDIR = $(CNZ_PATH)/$(CXC_NAME)
#CXC_NAME = ACS_CS
#CXC_NR ?= CXC1371330
#VERSION = 2.2-0
#CXC_VER ?= R1A18
#CXC_OLD_VER = R1A17

#BLOCK_NAME = ACS_CS
#DOXYGENCONFIGFILE = $(CXCPATH)/doc/cs_doxygen.cfg
include $(COMMON_ROOT)/common.mk
#LIBAPI_DIR = $(CNZ_PATH)/cs_cxc/bin/lib_ext

#LIBAPI_EXT_DIR = $(CNZ_PATH)/cs_cxc/bin/lib_ext
#LIBAPI_INT_DIR = $(CNZ_PATH)/cs_cxc/bin/lib_int

CCCC_FLAGS += --xml_outfile=$(CCCC_OUTPUT)/cccc.xml

# Handling local changes in API during development by adding local csapi inc
# before IO_Developments vob
CFLAGS += -I../csapi_caa/inc -I $(CNZ_PATH)/csapi_caa/inc

# Handling optimization for release builds
#ifneq ($(DEBUG),1)
#CFLAGS += -O3
#endif

