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
#            2010-12-17   XMALSHA   Created
#
#***********************************************************************

CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
CNZ_NAME ?= apgcc_cnz
CAA_NAME ?= apgccapi_caa
CXC_NAME ?= apgccapi_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(TOOLS_PATH)/linux_conf_file/Doxyfile.config

DOCDIR = $(ACS_ROOT)/asec_cnz/lct_cxc/doc
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=APGCCAPI_CAA
BLOCK_VERSION=1_0
BLOCK_SEARCH=./src ./inc

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

SRCDIR = $(CAA_PATH)/acs_security/test/src
INCDIR = $(CAA_PATH)/acs_security/inc/
CXCDIR = $(CAA_PATH)/acs_security/test/

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

##Include files
CINCLUDES += -I$(INCDIR)

## Include your libs here
LIBSDIR += -L$(ACS_ROOT)/apgcc_cnz/apgcc_cxc/bin/
LIBSDIR += -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin/
LIBSDIR += -L$(AP_SDK_LIB)

## Source files
COMMONLIB_TEST_SRC =$(SRCDIR)/ACS_Security_CommonAPIDemo.cpp 

COMMONLIB_TEST_OBJ =$(OBJDIR)/ACS_Security_CommonAPIDemo.obj
		
VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

COVERAGE ?= no

# here you can add own compiler flags
ifeq ($(COVERAGE),yes)
CFLAGS += -O0 -fprofile-arcs -ftest-coverage
else
CFLAGS += -O3
endif

# here you can add own lib paths

## # here you can add own libs
LIBS += -lrt -lpthread -lACS_APGCC -lacs_tra

## Object filebin
OBJ =   $(COMMONLIB_TEST_SRC:%.cpp=%.obj) 
	
## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) $(OBJDIR)

COMMONLIB_TEST_APNAME = ACS_Security_CommonAPIDemo

.PHONY: all CFLAGS += $(GCOV_FLAGS)

all:	$(OUTDIR)/$(COMMONLIB_TEST_APNAME) 
	 

$(OUTDIR)/$(COMMONLIB_TEST_APNAME):$(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(COMMONLIB_TEST_APNAME)'
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(COMMONLIB_TEST_APNAME) $(COMMONLIB_TEST_OBJ) $(LIBSDIR) $(LIBS)
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: cccc
cccc:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*)
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*)
	$(SEPARATOR_STR)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: documentation
documentation:
	$(SILENT)$(ECHO) 'documentation updating ...'
	$(SILENT)$(ECHO) ' Removing $(DOXYGEN_OUTPUT) directory'
	$(SILENT)$(RM) -r -f $(DOXYGEN_OUTPUT)
	$(SILENT)$(ECHO) ' Creating $(DOXYGEN_OUTPUT) directory'
	$(SILENT)mkdir $(DOXYGEN_OUTPUT)
	$(SILENT)$(DOXYGEN) -g $(DOXYGENFILECONFIG)

.PHONY: metrics
metrics:
	$(SILENT)$(ECHO) 'Metrics updating ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

