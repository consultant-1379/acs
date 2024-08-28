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
#            2010-02-19   xnicmut   Created
#            2010-07-30   xnicmut   Adjusted
# **********************************************************************

BUILD_USER_NAME = $(notdir $(HOME))

CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
VOB_PATH = $(ACS_ROOT)
#VOB_PATH = $(abspath ../../../..)

CNZ_NAME = dsd_cnz
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)

include $(COMMON_ROOT)/common.mk 

CAA_NAME = dsdapi_caa
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)

CXC_NAME = dsd_cxc
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

#OUTDIR = $(CXC_PATH)/bin
OUTDIR = ../../obj
OBJDIR = obj
SRCDIR = src
INCDIR = inc
TESTDIR = test

APP_NAME = acceptors-cute-002
APP_FILENAME = $(APP_NAME)

### BEGIN: To be deleted
#CC = g++

#ACE_SDK_LIB = /usr/lib64/ace
#ACE_SDK_INC = /usr/include/ace

#CINCLUDES = -I$(ACE_SDK_INC)
#LIBSDIR  = -L$(ACE_SDK_LIB)
#LIBS = -lACE
### End: To be deleted

#CXCDIR = prc_cxc
#BLOCKNAME = prcapi
#CURDIR = $(CNZ_PATH)/$(BLOCKNAME)_caa
#
#BLOCKDIR = $(CURDIR)

# here you can add own lib paths
LIBSDIR += -L"$(CAA_PATH)/obj"

## Flexelint application
#FL    = $(LINT)     # Global wrap-up mode
#FL1   = $(LINT) -u  # Single unit mode

## here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA)
#CFLAGS += -O3 -pedantic -fPIC -fmessage-length=0 -std=gnu++0x
CFLAGS += -O3 -fPIC -fmessage-length=0

LFLAGS += -s

## here you can add own Assembler flags
#ASMFLAGS += 
#

# here you can add own Include paths and/or other includes
CINCLUDES += -I"$(CAA_PATH)/inc"
CINCLUDES += -I"$(CNZ_PATH)/cute"

# here you can add own libs 
LIBS += -lrt -lpthread -lACS_DSD 

DEFINES += -D"$(BUILD_USER_NAME)"

### here you can add own File paths
##VPATH =	$(BLOCKDIR)/src/ifu $(BLOCKDIR)/src/swu $(OUTDIR)
#VPATH +=	$(SRCDIR) $(OUTDIR)
#

## Source files
SRCFILES =	acceptors-cute.cpp

OBJFILES =	$(OBJDIR)/acceptors-cute.obj

#IFUFILES = \
#
#ASMFILES = \
#
#L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 
#

## Object files
OBJ =	$(SRCFILES:%.cpp=%.obj) \
			$(SRCFILES:%.c=%.obj) 

### Build instructions
##
##APNAME  = BLOCKNAME1API_CXC
#APNAME = libprcapi.so
#
#APLIB = $(OUTDIR)/$(APNAME)
#
#APLIB_LOCAL = $(OBJDIR)/$(APNAME)
#

#.PHONY: depend clean
.PHONY: clean

all: $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating application: $(APP_FILENAME)'
	$(SILENT)$(RM) $(OUTDIR)/$(APP_FILENAME)
	$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) -o $(OUTDIR)/$(APP_FILENAME) $(OBJFILES) $(LFLAGS) $(LIBSDIR) $(LIBS)
	$(SEPARATOR_STR)
	$(NEW_LINE)

#all: $(APLIB_LOCAL)
#
#$(APLIB_LOCAL): $(OBJ) $(METRICS)
#	    $(NEW_LINE)
#	    $(SEPARATOR_STR)
#	    $(SILENT)$(ECHO) 'Creating library: $@'
#		$(SILENT)$(RM) $(APLIB_LOCAL)
#		$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) $(SRCFILES) -o $(APLIB_LOCAL)
#	    $(SEPARATOR_STR)
#	    $(NEW_LINE)
#
#release: $(APLIB)
#
clean:
	$(SILENT)$(ECHO) 'Cleaning file: $@'
	$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

#$(APLIB): $(OBJ) $(METRICS)
#	    $(NEW_LINE)
#	    $(SEPARATOR_STR)
#	    $(SILENT)$(ECHO) 'Creating library: $@'
#		$(SILENT)$(RM) $(APLIB)
#		$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) $(SRCFILES) -o $(APLIB)
#	    $(SEPARATOR_STR)
#	    $(NEW_LINE)
#
#metrics : cccc lint
#
#cccc:
#	    $(NEW_LINE)
#	    $(SEPARATOR_STR)
#	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
##		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
#		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
#	    $(SEPARATOR_STR)
#
#lint:
#	    $(NEW_LINE)
#	    $(SEPARATOR_STR)
#	    $(SILENT)$(ECHO) 'LINT file: $@'
##		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
#		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
#	    $(SEPARATOR_STR)
#
depend:	$(SRCFILES)
	makedepend $(CINCLUDES) $^

## DO NOT DELETE THIS LINE -- make depend needs it

