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
#            2010-10-04   tcschem   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= nsf_cnz
CAA_NAME ?= nsfadm_caa
CXC_NAME ?= nsf_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

CURDIR = $(CAA_PATH)
BLOCKDIR = $(CAA_PATH)
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test
CUTESRCDIR = $(TESTDIR)/CUTE/src
OBJDIR = $(TESTDIR)/CUTE/obj
DSDSRCDIR = $(TESTDIR)/dsd_stub/src

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own lib paths
#LIBSDIR  += -L$(COREMW_SDK_LIB)64 -L/$(TESTDIR)/dsd_stub/bin -L/usr/lib64
#LIBSDIR  =  -L. -L$(AP_SDK_LIB) -L$(ACE_SDK_LIB)  -L$(LOTC_SDK_LIB)  -L/$(TESTDIR)/dsd_stub/bin -L/usr/lib64  -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin -L$(COREMW_SDK_LIB)6464 

LIBSDIR +=   -L$(AP_SDK_LIB)  -L$(LIB_LOGCPLUS_SDK_LIB)

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACS_NSF_CUTE_TEST -fprofile-arcs -ftest-coverage

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
#CINCLUDES += -I$(INCDIR) -I$(ACE_SDK_PATH)/  -I$(TESTDIR)/CUTE/cute -I$(TESTDIR)/CUTE/cute/src -I/home1/SDK -I$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/cc/inc -I$(ACS_ROOT)/tra_cnz/traapi_caa/inc 
CINCLUDES += -I$(INCDIR) -I$(TESTDIR)/CUTE/cute -I$(TESTDIR)/CUTE/cute/src -I/home1/SDK 

## # here you can add own libs 
#LIBS += -lrt -lpthread
LIBS = -lACE -lrt -lpthread  -lacs_apgcc -lgcov -lacs_tra -llog4cplus  -lacs_prc

LDFLAGS      = -L. -L$(LOTCROOT)/usr/lib64 -lgcov

## here you can add own File paths
VPATH +=$(SRCDIR) $(SRCDIR)/Pha_stubs $(CUTESRCDIR) 

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

#SRCFILES = CUTE_NSF_Biostime_Recovery.cpp Test.cpp ACS_NSF_Biostime_Recovery.cpp
SRCFILES = CUTE_ACS_NSF_Server.cpp CUTE_ACS_NSF_Common.cpp acs_nsf_server.cpp acs_nsf_aeh.cpp acs_nsf_common.cpp acs_nsf_command_handler.cpp Test.cpp

## Object files
#
OBJS = \
	$(SRCFILES:%.cpp=%.obj)
	
## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = NSF_Cute_Test

APLIB = $(TESTDIR)/CUTE/bin/$(APNAME)

#.PHONY: depend clean
.PHONY: clean
 
all: $(APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		#$(SILENT)$(RM) $(wildcard $(CUTESRCDIR)/../bin/*) $(wildcard $(OBJDIR)/*)
		$(SILENT)$(RM) $(OBJDIR)/*.o*

$(APLIB): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating CUTE appl: $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) -o $(APLIB) $(LIBSDIR) $(LIBS) $(wildcard $(OBJDIR)/*.obj) $(LDFLAGS)
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

