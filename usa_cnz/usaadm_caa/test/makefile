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
 
CNZ_NAME ?= usa_cnz
CAA_NAME ?= usaadm_caa
CXC_NAME ?= usa_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

DOXYGENFILECONFIG := $(TOOLS_PATH)/linux_conf_file/Doxyfile.config

DOCDIR = $(ACS_ROOT)/usa_cnz/usa_cxc/doc
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=APOS_HA
BLOCK_VERSION=1_0
BLOCK_SEARCH=./src ./inc

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

EVT_GEN_SRCDIR = $(CAA_PATH)/test
TOOL_INCDIR = $(CAA_PATH)/test
OBJDIR = $(CAA_PATH)/test
CXCDIR	= $(CXC_PATH)

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

##Include files
CINCLUDES += -I$(TOOL_INCDIR)

## Source files
EVT_GEN_SRC =	$(EVT_GEN_SRCDIR)/acs_usa_eventGen.cpp
EVT_GEN_OBJ =	$(OBJDIR)/acs_usa_eventGen.obj

EVT_CRT_SRC =	$(EVT_GEN_SRCDIR)/acs_usa_eventCreate.cpp
EVT_CRT_OBJ =	$(OBJDIR)/acs_usa_eventCreate.obj
		
VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

COVERAGE ?= no

# here you can add own compiler flags
ifeq ($(COVERAGE),yes)
CFLAGS += -O0 -fprofile-arcs -ftest-coverage
else
CFLAGS += -O3
endif

# here you can add own lib paths
LIBSDIR += -L$(CAA_PATH)/lib64
LIBSDIR += -L$(AP_SDK_LIB)

## # here you can add own libs
LIBS += -lrt -lpthread -lACE 

## Object filebin
OBJ =   $(EVT_GEN_SRC:%.cpp=%.obj) \
	$(EVT_GEN_SRC:%.c=%.obj) \
	$(EVT_CRT_SRC:%.cpp=%.obj) \
	$(EVT_CRT_SRC:%.c=%.obj)

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR)

APNAME_EVTGEN = eventGen
APNAME_EVTCRT = eventCreate

.PHONY: all CFLAGS += $(GCOV_FLAGS)

all:	$(OUTDIR)/$(APNAME_EVTGEN) $(APNAME_EVTCRT)

$(OUTDIR)/$(APNAME_EVTGEN):$(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(APNAME_EVTGEN)'
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(APNAME_EVTGEN) $(EVT_GEN_OBJ) $(LIBSDIR) $(LIBS)
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(APNAME_EVTCRT):$(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(APNAME_EVTCRT)'
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(APNAME_EVTCRT) $(EVT_CRT_OBJ) $(LIBSDIR) $(LIBS)
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

