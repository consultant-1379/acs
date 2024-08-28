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
#            2010-02-19   xpaomaz   Created
#                                                            
# **********************************************************************

#CAA_NR = CAA109

#include $(COMMON_MK)
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

CXCDIR = tra_cxc
BLOCKNAME = traadm
CNZ_PATH = $(ACS_ROOT)/tra_cnz
CURDIR = $(CNZ_PATH)/$(BLOCKNAME)_caa

BLOCKDIR = $(PWD)
OUTDIR   = $(BLOCKDIR)
OBJDIR   = $(BLOCKDIR)
SRCDIR   = $(BLOCKDIR)
INCDIR   = $(BLOCKDIR)
TESTDIR  = $(BLOCKDIR)

# here you can add own lib paths
#LIBSDIR  += -L$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/OMHandling/obj

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA)
#CFLAGS += -L$(CNZ_PATH)/$(CXCDIR)/bin
CFLAGS += -L$(OUTDIR)
CFLAGS += -L$(ACE_SDK_LIB)
CFLAGS += -L$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/OMHandling/obj
CFLAGS += -L$(TSPSAF_SDK_LIB)

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR)
CINCLUDES += -I$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/OMHandling/inc
#CINCLUDES += -I/usr/include
CINCLUDES += -I$(BLOCKDIR)/cute
## # here you can add own libs 
CFLAGS += -lrt -lpthread 
CFLAGS += -lACE -lACS_CC_OmHandler -lSaImmOm -lSaImmOi -lncs_core -limmsv_common -lsaf_common

## here you can add own File paths
#VPATH =	$(BLOCKDIR)/src/ifu $(BLOCKDIR)/src/swu $(OUTDIR)
VPATH +=	$(SRCDIR) $(OUTDIR)

## Source files
#

SRCFILES =	$(SRCDIR)/OmHandlerUT_Main.cpp

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ =		OmHandlerUT_Main.obj

#OBJ = \
#		$(SRCFILES:%.cpp=%.obj) \
#		$(SRCFILES:%.c=%.obj) 

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = OmHandlerUT_Main

APPLICATION = $(OUTDIR)/$(APNAME)

APP_LOCAL = $(OBJDIR)/$(APNAME)

#.PHONY: depend clean
.PHONY: clean

all:$(APP_LOCAL)

$(APP_LOCAL): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating executable: $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) $(CFLAGS) $(LIBSDIR) $(APGINCLUDE) $(CINCLUDES) $(LIBS) $(SRCFILES) -o $(APP_LOCAL)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
 
release: $(APPLICATION)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

metrics : cccc lint

cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
#		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
#		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

				
	

depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

