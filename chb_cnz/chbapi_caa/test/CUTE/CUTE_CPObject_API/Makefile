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
CAADIR = chbapi_caa
CURDIR = $(CNZ_PATH)/$(CAADIR)

BLOCKDIR = $(CURDIR)
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test
CUTESRCDIR = $(TESTDIR)/CUTE/CUTE_CPObject_API/src
OBJDIR = $(TESTDIR)/CUTE/CUTE_CPObject_API/obj
ADMSRCDIR = ../../../../../../../cm4ap/ntacs/chb_cnz/chbadm_caa/src

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own lib paths
LIBSDIR  += -L$(AP_SDK_LIB) -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin -L$(LIB_LOGCPLUS_SDK_LIB)

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -fprofile-arcs -ftest-coverage 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR) -I$(ACS_ROOT)/chb_cnz/chbadm_caa/inc -I$(TESTDIR)/CUTE/CUTE_CPObject_API/cute -I$(TESTDIR)/CUTE/CUTE_CPObject_API/src -I/home1/SDK

## # here you can add own libs 
#LIBS += -lrt -lpthread -lACS_TRA  -llog4cplus -lACS_APGCC
LIBS += -lrt -lpthread -lACS_TRA -llog4cplus -lACS_APGCC 

LDFLAGS      =  -lgcov

## here you can add own File paths
VPATH +=$(SRCDIR) $(CUTESRCDIR) $(DSDSRCDIR) $(ADMSRCDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES = Test.cpp acs_chb_cp_object.cpp  \
		acs_chb_error.cpp CUTE_acs_chb_cpobject.cpp
		
		
		

## Object files
#
OBJS = \
	$(SRCFILES:%.cpp=%.obj)
	
## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = CUTE_CPTest

APLIB = $(TESTDIR)/CUTE/CUTE_CPObject_API/bin/$(APNAME)

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

