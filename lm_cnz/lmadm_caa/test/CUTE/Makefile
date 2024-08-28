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
#            2011-11-08  XCSSATA   Created
#                                                            
# **********************************************************************

#CAA_NR = 

CNZ_NAME ?= lm_cnz
CAA_NAME ?= lmadm_caa
CXC_NAME ?= lm_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

BLOCKDIR = $(CAA_PATH)
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test
CUTESRCDIR = $(TESTDIR)/CUTE/src
OBJDIR = $(TESTDIR)/CUTE/obj
SRCOBJDIR = $(CAA_PATH)/obj

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

## # here you can add own libs 
#LIBS = -lACE -lacs_apgcc -lacs_tra -llog4cplus -lacs_dsd -lacs_prc -lacs_jtp -llssrv64 -llsinit64
#-L$(LOTCROOT)/usr/lib64
LDFLAGS   = -L. -lgcov -lACE -lacs_apgcc -lacs_tra -llog4cplus -lacs_dsd -lacs_prc -lacs_jtp -llssrv64 -llsinit64
# here you can add own lib paths
LIBSDIR +=  -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L/vobs/cm4ap/ntacs/lm_cnz/SafeNet_Sentinel/lib64 -L$(LOTCROOT)/usr/lib64 

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA)  -fprofile-arcs -ftest-coverage -DCUTE_DEBUG -DCUTE_FRIEND -DACS_LM_CUTE_TEST -DCUTE_ACE_DEBUG -DFPSTUB='"ITSAAPLEG01-141.137.47.45"'

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR) -I$(TESTDIR)/CUTE/cute -I$(TESTDIR)/CUTE/cute/src -I/home1/sdk -I$(ACS_ROOT)/src/ -I/vobs/cm4ap/ntacs/lm_cnz/lmadm_caa/inc/ -I/vobs/cm4ap/ntacs/lm_cnz/lmadm_caa/src -I/vobs/cm4ap/ntacs/lm_cnz/SafeNet_Sentinel/Include

## here you can add own File paths
VPATH +=$(SRCDIR) $(OBJDIR) $(CUTESRCDIR) 

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES = acs_lm_sentinel.cpp  acs_lm_common.cpp acs_lm_persistent.cpp  acs_lm_tra.cpp  acs_lm_clienthandler.cpp  acs_lm_cprestartmonitor.cpp acs_lm_cmdserver.cpp acs_lm_cmdclient.cpp acs_lm_alarmstore.cpp acs_lm_timer.cpp  \
acs_lm_jtp_conversation_R3.cpp acs_lm_jtp_conversation.cpp  acs_lm_jtp_service_R2.cpp acs_lm_jtp_service_R3.cpp acs_lm_cmd.cpp \
acs_lm_jtp_service.cpp acs_lm_lksender.cpp  acs_lm_scheduler.cpp    \
acs_lm_jtp_conversation_R2.cpp acs_lm_eventhandler.cpp acs_lm_testlkf.cpp  \
acs_lm_electronickey_runtimeowner.cpp acs_lm_runtimeowner.cpp acs_lm_haservice.cpp acs_lm_dsdserver.cpp acs_lm_server.cpp acs_lm_root_objectImplementer.cpp cute_lm_common.cpp cute_lm_persistent.cpp cute_lm_test.cpp 

## Object files
#
OBJS = \
	$(SRCFILES:%.cpp=%.obj)\
	
## Build instructions
#
APNAME = LM_Cute_Test

APLIB = $(TESTDIR)/CUTE/bin/$(APNAME)

LMCUTESERVER_OBJDIR = $(OBJDIR)/acs_lm_sentinel.obj $(OBJDIR)/acs_lm_common.obj $(OBJDIR)/acs_lm_persistent.obj $(OBJDIR)/acs_lm_server.obj $(OBJDIR)/acs_lm_tra.obj $(OBJDIR)/acs_lm_clienthandler.obj $(OBJDIR)/acs_lm_cprestartmonitor.obj $(OBJDIR)/acs_lm_cmdserver.obj $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_alarmstore.obj $(OBJDIR)/acs_lm_timer.obj  $(OBJDIR)/acs_lm_jtp_conversation_R3.obj $(OBJDIR)/acs_lm_jtp_conversation.obj  $(OBJDIR)/acs_lm_jtp_service_R2.obj $(OBJDIR)/acs_lm_jtp_service_R3.obj $(OBJDIR)/acs_lm_cmd.obj $(OBJDIR)/acs_lm_jtp_service.obj $(OBJDIR)/acs_lm_lksender.obj  $(OBJDIR)/acs_lm_scheduler.obj $(OBJDIR)/acs_lm_jtp_conversation_R2.obj $(OBJDIR)/acs_lm_eventhandler.obj $(OBJDIR)/acs_lm_testlkf.obj $(OBJDIR)/acs_lm_electronickey_runtimeowner.obj $(OBJDIR)/acs_lm_haservice.obj $(OBJDIR)/acs_lm_runtimeowner.obj $(OBJDIR)/acs_lm_root_objectImplementer.obj $(OBJDIR)/acs_lm_dsdserver.obj $(OBJDIR)/cute_lm_common.obj $(OBJDIR)/cute_lm_persistent.obj $(OBJDIR)/cute_lm_test.obj

#.PHONY: depend clean
.PHONY: clean
 
all: $(APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(OBJDIR)/*.o*
		$(SILENT)$(RM) $(OBJDIR)/*.gc*

$(APLIB): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating CUTE appl: $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) -o $(APLIB) $(LMCUTESERVER_OBJDIR) $(LIBSDIR) $(LDFLAGS)
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

