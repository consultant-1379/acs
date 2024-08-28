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
# on a strict need to know$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/cc/makefile basis.
#
# **********************************************************************
#
# Rev        Date         Name      What
# -----      -------      --------  --------------------------
#            2010-06-23   xpaomaz   Created
#            2010-06-29   xantior   Modified   Modified src file and object file
#                                                            
# **********************************************************************

#CAA_NR = CAA109

#include $(COMMON_MK)
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

CNZ_PATH = $(ACS_ROOT)/apgcc_cnz
CXCDIR = apgccapi_cxc
CAADIR = apgccapi_caa
BLOCKNAME = cc
CURDIR = $(CNZ_PATH)/$(CAADIR)/$(BLOCKNAME)

BLOCKDIR = $(CURDIR)
OUTDIR   = $(CURDIR)/../../$(CXCDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test

# here you can add own lib paths
LIBSDIR  += $(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA)
CFLAGS += -fPIC -shared
CFLAGS +=-fprofile-arcs -ftest-coverage 
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR)
#Next LINe To be deleted once COMMON.mk updated
CINCLUDES += -I$(COREMW_SDK_INC)
## # here you can add own libs 
LIBS += -lrt -lpthread 

## here you can add own File paths
#VPATH =	$(BLOCKDIR)/src/ifu $(BLOCKDIR)/src/swu $(OUTDIR)
VPATH +=	$(SRCDIR) $(OUTDIR)

## Source files
#

SRCFILES =	$(SRCDIR)/APGCC_CmdParams_R1.cpp \
	$(SRCDIR)/APGCC_Command.cpp \
	$(SRCDIR)/APGCC_DSD.cpp \
	$(SRCDIR)/APGCC_Directory.cpp \
	$(SRCDIR)/APGCC_Event.cpp \
	$(SRCDIR)/APGCC_FileStream.cpp \
	$(SRCDIR)/APGCC_InitFile.cpp \
	$(SRCDIR)/APGCC_LockFile.cpp \
	$(SRCDIR)/APGCC_PIPE_R1.cpp \
	$(SRCDIR)/APGCC_Parameters.cpp \
	$(SRCDIR)/APGCC_Path.cpp \
	$(SRCDIR)/APGCC_ServiceBase_R1.cpp \
	$(SRCDIR)/APGCC_TempFile.cpp \
	$(SRCDIR)/APGCC_Time_Value.cpp \
	$(SRCDIR)/APGCC_Util.cpp \
	$(SRCDIR)/APGCC_trace.cpp 

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ =	APGCC_CmdParams_R1.obj \
	APGCC_Command.obj \
	APGCC_DSD.obj \
	APGCC_Directory.obj \
	APGCC_Event.obj \
	APGCC_FileStream.obj \
	APGCC_InitFile.obj \
	APGCC_LockFile.obj \
	APGCC_PIPE_R1.obj \
	APGCC_Parameters.obj \
	APGCC_Path.obj \
	APGCC_ServiceBase_R1.obj \
	APGCC_TempFile.obj \
	APGCC_Time_Value.obj \
	APGCC_Util.obj \
	APGCC_trace.obj 

#OBJ = \
#		$(SRCFILES:%.cpp=%.obj) \
#		$(SRCFILES:%.c=%.obj) 
#

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = libACS_CC_cc.so

APLIB = $(OUTDIR)/$(APNAME)

#.PHONY: depend clean
.PHONY: clean

 
all: $(APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file Common Class: $@'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)
		#$(SILENT)$(RM) $(OBJDIR)/*.o*

$(APLIB): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating library Common Class: $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) $(CFLAGS) $(APGINCLUDE) $(CINCLUDES) $(SRCFILES) -o $(APLIB)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

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

