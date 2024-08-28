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
#            2010-07-20   tcstanu   Created
#                                                            
# **********************************************************************

#CAA_NR = 

#include $(COMMON_MK)
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

CNZ_PATH = $(ACS_ROOT)/fcsc_cnz
CXCDIR = fcsc_cxc
CAADIR = fcsccmd_caa
CURDIR = $(CNZ_PATH)/$(CAADIR)

#CC = $(CROSS_COMPILE)/x86_64-dx-g++

BLOCKDIR = $(CURDIR)
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test/
CUTESRCDIR = $(TESTDIR)/src
OBJDIR = $(TESTDIR)/obj
OBJDIRSWUPDATESDIR = $(BLOCKDIR)/obj

OBJDIRSWUPDATES =  fcsc_CompatibiltyChecker.obj fcsc_UpdSdpList.obj fcsc_Common.obj       fcsc_RepositoryMgr.obj fcsc_DownloadMgr.obj fcscPrepare.obj

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own lib paths
LIBSDIR  += -L$(COREMW_SDK_LIB)  -L/usr/lib64 

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -fprofile-arcs -ftest-coverage

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES +=  -I$(INCDIR) -I$(ACE_SDK_PATH)  -I$(TESTDIR)/cute/ -I/home/tcsvijk/cute/  -I$(LIB_CURL_SDK_INC) 

## # here you can add own libs 
#LIBS += -lrt -lpthread -lxml2 -lACE -lcurl 
 
LIBS =  -lxml2 -lgcov -lACE -lcurl 

LDFLAGS      = -L. -L$(LOTCROOT)/usr/lib64 -lgcov -L$(LIB_CURL_SDK_LIB)

## here you can add own File paths
VPATH +=$(SRCDIR) $(CUTESRCDIR) 

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES = Test.cpp test_swupdate.cpp\
 	   fcsc_CompatibiltyChecker.cpp fcsc_UpdSdpList.cpp fcsc_Common.cpp  fcsc_RepositoryMgr.cpp fcsc_DownloadMgr.cpp fcscPrepare.cpp
 

## Object files
#
OBJS = \
	$(SRCFILES:%.cpp=%.obj)
	
## Build instructions
#
APNAME = Test

APLIB = $(TESTDIR)/bin/$(APNAME)

#.PHONY: depend clean
.PHONY: clean
 
all: $(APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(CUTESRCDIR)/../bin/*) $(wildcard $(OBJDIR)/*)
		#$(SILENT)$(RM) $(OBJDIR)/*.o*

$(APLIB): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating CUTE appl: $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) -o $(APLIB) $(LIBSDIR) $(LIBS) $(wildcard $(OBJDIR)/*.obj) $(LDFLAGS)  #$(wildcard $(OBJDIRSWUPDATESDIR)/*.obj) $(LDFLAGS)
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

