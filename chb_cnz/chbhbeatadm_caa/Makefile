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
#            2010-12-15   XTANAGG   Created
#                                                            
# **********************************************************************
CNZ_NAME ?= chb_cnz
CAA_NAME ?= chbhbeatadm_caa
CXC_NAME ?= chbhbeat_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
CHBAPIINTINCDIR = $(BLOCKDIR)/../chbapi_caa/inc_int 
CHBAPIEXTINCDIR = $(BLOCKDIR)/../chbapi_caa/inc_ext
CHBADMINCDIR = $(BLOCKDIR)/../chbadm_caa/inc
TESTDIR  = $(BLOCKDIR)/test

# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -DCHBDEBUG 
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES +=-I$(INCDIR) -I$(CHBAPIINTINCDIR)  -I$(CHBAPIEXTINCDIR) -I$(CHBADMINCDIR)

## # here you can add own Include paths and/or other includes 
LDFLAGS      += 

LIBSDIR +=   -L$(ACS_ROOT)/chb_cnz/chbhbeat_cxc/bin/lib_ext \
-L$(ACS_ROOT)/chb_cnz/chbmtzln_cxc/bin/lib_ext \
-L$(LIB_LOGCPLUS_SDK_LIB) -L$(AP_SDK_LIB)

LIBSDIR += 

CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

LIBS += -lrt -lpthread

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

#BOOST
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBS += -lboost_serialization -lboost_filesystem -lboost_system

#SEC SDK
LIBSDIR += -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB)
LIBS += -lsec_crypto_api -lsec_credu_api

ACS_CHB_HEARTBEAT_LIBS = $(LIBS) -llog4cplus -lacs_tra -lacs_apgcc -lacs_dsd -lacs_prc -lacs_csapi -lacs_jtp  -lacs_chb_mtz -lacs_chb_cpobject -lacs_aeh -lutil

ACS_CHB_HEARTBEAT_CHILD_LIBS =  $(LIBS) -lacs_apgcc -llog4cplus -lacs_dsd -lacs_tra -lacs_prc -lacs_csapi -lacs_aeh

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) 

## Source files
ACS_CHB_HEARTBEATSRC = ../chbadm_caa/src/acs_chb_client_handler.cpp ../chbadm_caa/src/acs_chb_aeh.cpp ../chbadm_caa/src/acs_chb_tra.cpp \
../chbadm_caa/src/acs_chb_cp_objectlist.cpp ../chbadm_caa/src/acs_chb_event.cpp ../chbadm_caa/src/acs_chb_eventlist.cpp \
../chbadm_caa/src/acs_chb_heartbeat_comm.cpp acs_chb_hb_service.cpp ../chbadm_caa/src/acs_chb_receive.cpp\
../chbadm_caa/src/acs_chb_request_handler.cpp ../chbadm_caa/src/acs_chb_subscriber.cpp ../chbadm_caa/src/acs_chb_thread_jtp_comm.cpp \
../chbadm_caa/src/acs_chb_threadpool.cpp ../chbadm_caa/src/acs_chb_error.cpp ../chbadm_caa/src/acs_chb_heartbeat_class.cpp  \
../chbadm_caa/src/acs_chb_aadst_handler.cpp ../chbadm_caa/src/acs_chb_aadst_ThreadManager.cpp ../chbadm_caa/src/acs_chb_aadst_common.cpp \
../chbadm_caa/src/acs_chb_aadst_TDstCore.cpp ../chbadm_caa/src/acs_chb_aadst_TDstExtractor.cpp ../chbadm_caa/src/acs_chb_aadst_TDstTypes.cpp \
../chbadm_caa/src/acs_chb_aadst_alarmHandler.cpp ../chbadm_caa/src/acs_chb_aadst_TCastcFacility.cpp

ACS_CHB_HEARTBEAT_CHILDSRC = ../chbadm_caa/src/acs_chb_client_handler.cpp ../chbadm_caa/src/acs_chb_aeh.cpp \
../chbadm_caa/src/acs_chb_tra.cpp ../chbadm_caa/src/acs_chb_cp_objectlist.cpp ../chbadm_caa/src/acs_chb_event.cpp \
../chbadm_caa/src/acs_chb_eventlist.cpp \
../chbadm_caa/src/acs_chb_receive.cpp ../chbadm_caa/src/acs_chb_request_handler.cpp ../chbadm_caa/src/acs_chb_subscriber.cpp \
../chbadm_caa/src/acs_chb_event_manager.cpp ../chbadm_caa/src/acs_chb_error.cpp

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files

ACS_CHB_HEARTBEATOBJ = \
		$(ACS_CHB_HEARTBEATSRC:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

ACS_CHB_HEARTBEAT_CHILDOBJ = \
		$(ACS_CHB_HEARTBEAT_CHILDSRC:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj)
		
## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC

ACS_CHB_HEARTBEAT = acs_chbheartbeatd
ACS_CHB_HEARTBEAT_CHILD = acs_chbheartbeatchildd
ACS_CHB_HEARTBEAT_DBG = acs_chbheartbeatd_dbg
ACS_CHB_HEARTBEAT_CHILD_DBG = acs_chbheartbeatchildd_dbg

ACS_CHB_HEARTBEATEXE = $(OUTDIR)/$(ACS_CHB_HEARTBEAT)
ACS_CHB_HEARTBEAT_CHILDEXE = $(OUTDIR)/$(ACS_CHB_HEARTBEAT_CHILD)
ACS_CHB_HEARTBEATEXE_DBG = $(OUTDIR)/.debug/$(ACS_CHB_HEARTBEAT_DBG)
ACS_CHB_HEARTBEAT_CHILDEXE_DBG = $(OUTDIR)/.debug/$(ACS_CHB_HEARTBEAT_CHILD_DBG)

ACS_CHB_HEARTBEATOBJFILES = $(OBJDIR)/acs_chb_client_handler.obj $(OBJDIR)/acs_chb_aeh.obj $(OBJDIR)/acs_chb_tra.obj \
$(OBJDIR)/acs_chb_cp_objectlist.obj $(OBJDIR)/acs_chb_event.obj $(OBJDIR)/acs_chb_eventlist.obj \
$(OBJDIR)/acs_chb_heartbeat_comm.obj $(OBJDIR)/acs_chb_hb_service.obj $(OBJDIR)/acs_chb_receive.obj\
$(OBJDIR)/acs_chb_request_handler.obj $(OBJDIR)/acs_chb_subscriber.obj $(OBJDIR)/acs_chb_thread_jtp_comm.obj \
$(OBJDIR)/acs_chb_threadpool.obj $(OBJDIR)/acs_chb_error.obj $(OBJDIR)/acs_chb_heartbeat_class.obj \
$(OBJDIR)/acs_chb_aadst_handler.obj $(OBJDIR)/acs_chb_aadst_ThreadManager.obj $(OBJDIR)/acs_chb_aadst_common.obj \
$(OBJDIR)/acs_chb_aadst_TDstCore.obj $(OBJDIR)/acs_chb_aadst_TDstExtractor.obj $(OBJDIR)/acs_chb_aadst_TDstTypes.obj \
$(OBJDIR)/acs_chb_aadst_alarmHandler.obj $(OBJDIR)/acs_chb_aadst_TCastcFacility.obj

ACS_CHB_HEARTBEAT_CHILDOBJFILES =  $(OBJDIR)/acs_chb_client_handler.obj $(OBJDIR)/acs_chb_aeh.obj $(OBJDIR)/acs_chb_tra.obj \
$(OBJDIR)/acs_chb_cp_objectlist.obj $(OBJDIR)/acs_chb_event.obj $(OBJDIR)/acs_chb_eventlist.obj \
$(OBJDIR)/acs_chb_receive.obj $(OBJDIR)/acs_chb_request_handler.obj $(OBJDIR)/acs_chb_subscriber.obj \
$(OBJDIR)/acs_chb_event_manager.obj $(OBJDIR)/acs_chb_error.obj

DEPS := $(ACS_CHB_HEARTBEATOBJFILES:.obj=.d) \
	$(ACS_CHB_HEARTBEAT_CHILDOBJFILES:.obj=.d) 

ifneq ($(MAKECMDGOALS),clean)

-include $(DEPS)
endif

.PHONY: all

all: $(ACS_CHB_HEARTBEATEXE) $(ACS_CHB_HEARTBEAT_CHILDEXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_CHB_HEARTBEATEXE) $(ACS_CHB_HEARTBEAT_CHILDEXE)'
		$(SILENT)$(RM) $(ACS_CHB_HEARTBEATEXE) $(ACS_CHB_HEARTBEAT_CHILDEXE)
		$(SILENT)$(RM) $(ACS_CHB_HEARTBEATEXE_DBG) $(ACS_CHB_HEARTBEAT_CHILDEXE_DBG)

$(ACS_CHB_HEARTBEATEXE): $(ACS_CHB_HEARTBEATOBJ) $(METRICS)
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_CHB_HEARTBEATEXE) $(ACS_CHB_HEARTBEATOBJFILES) $(LIBSDIR) $(ACS_CHB_HEARTBEAT_LIBS)
	    $(call stripp,$(ACS_CHB_HEARTBEAT))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	
$(ACS_CHB_HEARTBEAT_CHILDEXE): $(ACS_CHB_HEARTBEAT_CHILDOBJ) $(METRICS)
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_CHB_HEARTBEAT_CHILDEXE) $(ACS_CHB_HEARTBEAT_CHILDOBJFILES) $(LIBSDIR) $(ACS_CHB_HEARTBEAT_CHILD_LIBS)
	    $(call stripp,$(ACS_CHB_HEARTBEAT_CHILD))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
 

