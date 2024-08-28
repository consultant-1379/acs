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
#            2010-12-02   tcstanu   Created
#                                                            
# **********************************************************************

#CAA_NR = 

#include $(COMMON_MK)
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

CXCDIR = chb_cxc
CAADIR = chbapi_caa/chbmtz_caa
CURDIR = $(CNZ_PATH)/$(CAADIR)

BLOCKDIR = $(CURDIR)
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test
CUTESRCDIR = $(TESTDIR)/CUTE/src
OBJDIR = $(TESTDIR)/CUTE/obj

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own lib paths
LIBSDIR  += -L$(AP_SDK_LIB) -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -fprofile-arcs -ftest-coverage -DACS_MTZ_CUTE_TEST

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR) -I$(TESTDIR)/CUTE/cute -I$(TESTDIR)/CUTE/cute/src -I/home1/SDK

## # here you can add own libs 
#LIBS += -lrt -lpthread -lACS_TRA  -llog4cplus -lACS_APGCC
LIBS += -lrt -lpthread -lACS_TRA -llog4cplus -lACS_APGCC 

LDFLAGS      =  -lgcov

## here you can add own File paths
VPATH +=$(SRCDIR) $(CUTESRCDIR) $(DSDSRCDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES = Test.cpp ACS_CHB_filehandler.cpp  \
		ACS_CHB_mtzconv.cpp ACS_CHB_NtZone.cpp ACS_CHB_Threadhandler.cpp \
		CUTE_ACS_CHB_mtzconv.cpp CUTE_ACS_CHB_filehandler.cpp CUTE_ACS_CHB_Threadhandler.cpp \
		
		

## Object files
#
OBJS = \
	$(SRCFILES:%.cpp=%.obj)
	
## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = Test

APLIB = $(TESTDIR)/CUTE/bin/$(APNAME)

.PHONY: all
all: $(APLIB)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(CUTESRCDIR)/../bin/*) $(wildcard $(OBJDIR)/*)
		

$(APLIB): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating CUTE appl: $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) -o $(APLIB) $(LIBSDIR) $(LIBS) $(wildcard $(OBJDIR)/*.obj) $(LDFLAGS)
		$(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: metrics
metrics : cccc lint

cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
#		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

.PHONY: lint
lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
#		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

