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
#   A        2011-07-27   XTANAGG   Created
#   B        2012-10-09   XLANSRI   Updated source files                                              
# **********************************************************************

CNZ_NAME ?= aca_cnz
CAA_NAME ?= acaadm_caa
CXC_NAME ?= aca_cxc

include $(COMMON_ROOT)/common.mk

VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
SRCCMDDIR = $(BLOCKDIR)/../acacmd_caa/src
INCDIR   = $(BLOCKDIR)/inc
ACAAPIINTINCDIR = $(BLOCKDIR)/../acaapi_caa/inc_int 
ACAAPIEXTINCDIR = $(BLOCKDIR)/../acaapi_caa/inc_ext
ACACMDINTINCDIR = $(BLOCKDIR)/../acacmd_caa/inc
TESTDIR  = $(BLOCKDIR)/test

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  
CFLAGS += -DACS_ACA_HAS_LOGGING -DACS_ACA_HAS_DUMPING
CFLAGS += -DACS_ACA_LOGGING_HAS_SOURCE_INFO -DACS_ACA_LOGGING_USE_PRETTY_FUNCTION
CFLAGS += -DACS_ACA_HAS_HARD_LIMIT
CFLAGS += -DACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES +=-I$(ACE_SDK_INC) -I$(COREMW_SDK_INC) -I$(BOOST_SDK_INC) 
CINCLUDES +=-I$(INCDIR) -I$(ACAAPIINTINCDIR) -I$(ACAAPIEXTINCDIR) -I$(ACACMDINTINCDIR)

## # here you can add own Include paths and/or other includes 
LDFLAGS      += 

LIBSDIR += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB)  -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(OUTDIR)/lib_ext -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AP_SDK_LIB) 
LIBSDIR += -L$(BOOST_SDK_LIB)

## # here you can add own libs
LIBS += -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf
LIBS += -lboost_serialization -lboost_filesystem  -lboost_system  
LIBS += -lrt -lpthread -lcap -llog4cplus

ACS_MSD_LIBS = $(LIBS) -lacs_tra -lacs_apgcc -lacs_dsd -lacs_csapi -lacs_aca -lsec_crypto_api -lacs_aeh -lacs_prc

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) $(SRCCMDDIR)

## Source files
SRCFILES = \
	acs_aca_common.cpp \
	acs_aca_cptable_observer.cpp \
	acs_aca_logger.cpp \
	acs_aca_msd_haservice.cpp \
	acs_aca_msd_ms_manager.cpp \
	acs_aca_msd_msg_collection.cpp \
	acs_aca_msd_msg_store.cpp \
	acs_aca_msd_msip_channel_mgr.cpp \
	acs_aca_msd_mtap_channel_mgr.cpp \
	acs_aca_msd_mtap_message.cpp \
	acs_aca_msd_mtap_protocol.cpp \
	acs_aca_msd_service.cpp \
	acs_aca_msdls_server.cpp \
	acs_aca_msdls_worker_thread.cpp \
	acs_aca_msg_builder.cpp \
	acs_aca_msg_director.cpp \
	acs_aca_msgfile.cpp \
	acs_aca_root_objectImpl.cpp \
	acs_aca_service_main.cpp \
	acs_aca_structured_response_builder.cpp \
	acs_aca_structured_response.cpp \

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
OBJFILES =	$(SRCFILES:%.cpp=%.obj) 

## Build instructions
#APNAME  = BLOCKNAME1API_CXC

ACS_ACA = acs_acad
ACS_ACA_DBG = acs_acad_dbg
ACS_ACA_EXE = $(OUTDIR)/$(ACS_ACA)

ACS_ACA_OBJFILES = $(SRCFILES:%.cpp=%.obj) 

DEPS := $(ACS_ACA_OBJFILES:.obj=.d) 

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all

all: $(ACS_ACA_EXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_ACA_EXE) 
		$(SILENT)$(RM) $(ACS_ACA_EXE) 


$(ACS_ACA_EXE): $(ACS_ACA_OBJFILES) $(METRICS)
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_ACA_EXE) $(wildcard $(OBJDIR)/*.obj) $(LIBSDIR) $(ACS_MSD_LIBS)
		$(call stripp,$(ACS_ACA))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	
metrics : cccc lint

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
#               $(LINT) $(wildcard $(BLOCKDIR)/inc/*)
		$(LINT) $(wildcard $(BLOCKDIR)/src/*)
		$(SEPARATOR_STR)


.PHONY: depend
depend: $(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
