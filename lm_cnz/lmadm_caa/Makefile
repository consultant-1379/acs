#***********************************************************************
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
#            2011-05-05   xnadnar   Created
#                                                            
# **********************************************************************

#CXC_NR = CXC120333

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

CURDIR   = $(CAA_PATH)
BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
INCCMDDIR= $(BLOCKDIR)/../lmcmd_caa/inc
TESTDIR  = $(BLOCKDIR)/test
NTACS = $(ACS_ROOT)

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
 
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE 
# -DFPSTUB='"ITSAAPLEG01-141.137.47.45"'
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -g  
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES += -I$(INCDIR) -I$(INCCMDDIR) -I$(CURDIR)/../SafeNet_Sentinel/Include
CINCLUDES += -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)

## # here you can add own Include paths and/or other includes
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTC_SDK_LIB)

LIBSDIR +=  -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)  -L$(CURDIR)/../SafeNet_Sentinel/lib64 -L$(ACE_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(LIB_XERCES_SDK_LIB) -L$(COREMW_SDK_LIB) -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(SEC_ACS_SDK_LIB)

## # here you can add own libs 

LIBS += -ldl -lACE -lacs_apgcc -lacs_tra -llog4cplus -lacs_dsd -lacs_prc -lacs_jtp -llssrv64 -llsinit64 -lpthread -lacs_csapi -lacs_aeh -lsec_crypto_api -lboost_serialization -lboost_filesystem -lboost_system 
LIBS += $(COREMW_LIBS)
LIBS += $(SEC_LIBS)
LIBS += $(SEC_CREDU_LIBS)

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OBJDIR) $(OUTDIR) $(CURDIR)/../lmcmd_caa/src

## Source files
#
SRCFILES = acs_lm_senddata.cpp acs_lm_cpdata.cpp acs_lm_sentinel.cpp  acs_lm_common.cpp acs_lm_common_util.cpp acs_lm_persistent.cpp acs_lm_server.cpp acs_lm_tra.cpp  acs_lm_clienthandler.cpp  acs_lm_cprestartmonitor.cpp acs_lm_cmdserver.cpp acs_lm_cmdclient.cpp acs_lm_alarmstore.cpp acs_lm_timer.cpp acs_lm_haservice.cpp \
acs_lm_jtp_conversation_R3.cpp acs_lm_jtp_conversation.cpp  acs_lm_jtp_service_R2.cpp acs_lm_jtp_service_R3.cpp acs_lm_cmd.cpp \
acs_lm_jtp_service.cpp acs_lm_lksender.cpp  acs_lm_scheduler.cpp    \
acs_lm_jtp_conversation_R2.cpp acs_lm_eventhandler.cpp acs_lm_testlkf.cpp  \
acs_lm_electronickey_runtimeowner.cpp acs_lm_runtimeowner.cpp acs_lm_main.cpp acs_lm_root_objectImplementer.cpp acs_lm_managed_objectImplementer.cpp acs_lm_northbound_objectImplementer.cpp  acs_lm_dsdserver.cpp \
acs_lm_brfc_interface.cpp 

 

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

## Build instructions
#
APNAME = acs_lmserverd
APNAME_DBG = acs_lmserverd_dbg

APEXE = $(OUTDIR)/$(APNAME)

LMSERVER_OBJDIR = $(OBJDIR)/acs_lm_sentinel.obj $(OBJDIR)/acs_lm_common.obj $(OBJDIR)/acs_lm_common_util.obj $(OBJDIR)/acs_lm_persistent.obj $(OBJDIR)/acs_lm_server.obj $(OBJDIR)/acs_lm_tra.obj $(OBJDIR)/acs_lm_clienthandler.obj $(OBJDIR)/acs_lm_cprestartmonitor.obj $(OBJDIR)/acs_lm_cmdserver.obj $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_alarmstore.obj $(OBJDIR)/acs_lm_timer.obj  $(OBJDIR)/acs_lm_jtp_conversation_R3.obj $(OBJDIR)/acs_lm_jtp_conversation.obj  $(OBJDIR)/acs_lm_jtp_service_R2.obj $(OBJDIR)/acs_lm_jtp_service_R3.obj $(OBJDIR)/acs_lm_cmd.obj $(OBJDIR)/acs_lm_jtp_service.obj $(OBJDIR)/acs_lm_lksender.obj  $(OBJDIR)/acs_lm_scheduler.obj $(OBJDIR)/acs_lm_jtp_conversation_R2.obj $(OBJDIR)/acs_lm_eventhandler.obj $(OBJDIR)/acs_lm_testlkf.obj $(OBJDIR)/acs_lm_electronickey_runtimeowner.obj $(OBJDIR)/acs_lm_haservice.obj $(OBJDIR)/acs_lm_runtimeowner.obj $(OBJDIR)/acs_lm_main.obj $(OBJDIR)/acs_lm_root_objectImplementer.obj $(OBJDIR)/acs_lm_managed_objectImplementer.obj $(OBJDIR)/acs_lm_northbound_objectImplementer.obj $(OBJDIR)/acs_lm_dsdserver.obj $(OBJDIR)/acs_lm_brfc_interface.obj $(OBJDIR)/acs_lm_cpdata.obj $(OBJDIR)/acs_lm_senddata.obj

.PHONY: all
all: $(APEXE)

.PHONY: clean
clean:	
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT) rm -f  $(wildcard $(OBJDIR)/*)

.PHONY: distclean
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(APNAME)
		$(RM) $(OUTDIR)/.debug/$(APNAME_DBG)

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(LMSERVER_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
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

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

				
	
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

