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
 
CNZ_PATH = $(ACS_ROOT)/apjtp_cnz
CXCDIR = apjtp_cxc
CAADIR = apjtpapi_caa
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
LIBSDIR  += -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AP_SDK_LIB)

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -fprofile-arcs -ftest-coverage

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES = -I$(ACE_SDK_PATH)  -I$(LOTC_SDK_INC) -I$(LOTC_SDK_INC)/c++/4.3 -I$(LOTC_SDK_INC)/c++/4.3/x86_64-suse-linux -I$(COREMW_SDK_INC) -I$(ACS_ROOT)/apjtp_cnz/apjtpapi_caa/inc  -I$(AP_SDK_INC) -I/home1/SDK -I$(TESTDIR)/CUTE/cute  -I$(TESTDIR)/CUTE/cute/src

## # here you can add own libs 
LIBS += -lrt -lpthread -lacs_tra -llog4cplus -lacs_apgcc -lacs_prc -lacs_dsd -lACS_CSAPI

LDFLAGS      =  -lgcov

## here you can add own File paths
VPATH +=$(SRCDIR) $(CUTESRCDIR) 

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES = Test.cpp CUTE_ACS_JTP_Service_R3A.cpp CUTE_ACS_JTP_Service_R2A.cpp CUTE_ACS_JTP_Job_R2A.cpp \
ACS_JTP_Conversation_R2A.cpp CUTE_ACS_JTP_Job_R3A.cpp CUTE_ACS_JTP_Conversation_R2A.cpp CUTE_ACS_JTP_Conversation_R3A.cpp \
ACS_JTP_Job_R2A.cpp ACS_JTP_Service_R2A.cpp  ACS_PRC_info.cpp jtp.cpp JTP_Service.cpp \
JTP_Session.cpp JTP_Service_R2A.cpp JTP_Session_R2A.cpp\
JTP_Service_R3A.cpp JTP_Session_R3A.cpp ACS_JTP_Conversation_R3A.cpp ACS_JTP_Job_R3A.cpp ACS_JTP_Service_R3A.cpp  

## Object files
#
OBJS = \
	$(SRCFILES:%.cpp=%.obj)
	
## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = Test

APLIB = $(TESTDIR)/CUTE/bin/$(APNAME)

#.PHONY: depend clean

.PHONY: all
all: $(APLIB)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(CUTESRCDIR)/../bin/*) $(wildcard $(OBJDIR)/*)
		#$(SILENT)$(RM) $(OBJDIR)/*.o*

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

