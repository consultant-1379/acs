# **********************************************************************
#
# Short description:
# Makefile template for XXX library
# **********************************************************************
#
# Ericsson AB 2012 All rights reserved.
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
#   B        2012-12-06   XLANSRI   Created
# **********************************************************************

CNZ_NAME ?= rtr_cnz
CAA_NAME ?= rtradm_caa
CXC_NAME ?= rtr_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
RTRAPIINCDIR = $(BLOCKDIR)/../rtrapi_caa/inc_int 
RTRLIBSRCDIR = $(BLOCKDIR)/../rtrapi_caa/src
RTRCMDINCDIR = $(BLOCKDIR)/../acacmd_caa/inc
SECINCDIR = $(SEC_SDK_INC)/sec
TESTDIR  = $(BLOCKDIR)/test
NTACS = $(ACS_ROOT)
ACA_ADM_INC = $(ACS_ROOT)/aca_cnz/acaadm_caa/inc
ACA_LIB_INC = $(ACS_ROOT)/aca_cnz/acaapi_caa/inc_int
ACA_LIB_EXTINC = $(ACS_ROOT)/aca_cnz/acaapi_caa/inc_ext

# here you can add more defines
# trace enabled
EXTRA_DEFINES += -DACS_RTR_API_HAS_TRACE
EXTRA_DEFINES += -DACS_RTR_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DACS_RTR_TRACE_HAS_FUNCTION_TRACE
EXTRA_DEFINES += -DACS_RTR_TRACE_USE_PRETTY_FUNCTION
# log enabled
EXTRA_DEFINES += -DACS_RTR_HAS_LOGGING
EXTRA_DEFINES += -DACS_RTR_LOGGING_HAS_SOURCE_INFO
EXTRA_DEFINES += -DACS_RTR_LOGGING_USE_PRETTY_FUNCTION

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA)
CFLAGS += $(EXTRA_DEFINES)
CFLAGS += -ggdb


# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES +=-I$(INCDIR) -I$(RTRCMDINCDIR) -I$(RTRAPIINCDIR) -I$(SECINCDIR) -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC) -I$(BOOST_SDK_INC) -I$(ACA_ADM_INC) -I$(ACA_LIB_INC) -I$(ACA_LIB_EXTINC) 
CINCLUDES += -I$(AP_SDK_INC)

## # here you can add own Include paths and/or other includes 
LDFLAGS += -W1,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -W1,-rpath-link,$(LOTC_SDK_LIB)
LDFLAGS += -W1,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -W1,-rpath-link,$(ACE_SDK_LIB)
LDFLAGS += -W1,-rpath-link,$(COREMW_SDK_LIB)

#LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB) -L$(ACS_ROOT)/rtr_cnz/rtr_cxc/bin/lib_int -L$(AP_SDK_LIB) -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(AP_SDK_LIB) -L$(ACS_ROOT)/rtr_cnz/rtr_cxc/bin/lib_int
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB)
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)


## # here you can add own libs 
LIBS += -lrt -lpthread
#LIBS += -lrt -lpthread -lsec_crypto_api -lACE -lacs_apgcc -lacs_dsd -lacs_aeh -lacs_tra -lacs_csapi -lacs_prc -lboost_serialization -lboost_filesystem -lboost_system -lSaImmOm -lSaImmOi -lSaAmf -lSaClm -llog4cplus -lcrypto

ACS_RTR_LIBS = $(LIBS) $(SEC_CREDU_LIBS) -lacs_rtr -lsec_crypto_api -lACE -lacs_apgcc -lacs_aca -laes_ohi -lssl -lcap -laes_dbo -laes_afp -lacs_dsd -lacs_aeh -lacs_tra -lacs_csapi -lacs_prc -lboost_serialization -lboost_filesystem -lboost_system -lboost_thread -lSaImmOm -lSaImmOi -lSaAmf -lSaClm -llog4cplus -lcrypto -lSaCkpt -lSaLog -lSaNtf -laes_gcc

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) $(SRCAPIDIR) $(OBJDIR) $(RTRLIBSRCDIR)

## Source files


SRCFILES = acs_rtr_filebased_objectImpl.cpp\
acs_rtr_lib.cpp\
acs_rtr_immhandler.cpp\
acs_rtr_global.cpp\
acs_rtr_statistics_impl.cpp\
acs_rtr_cpstatistics_impl.cpp\
acs_rtr_msread.cpp\
acs_rtr_hmacmd5.cpp\
acs_rtr_filenumbermgr.cpp\
acs_rtr_filebuild.cpp\
acs_rtr_filereport.cpp\
acs_rtr_gohreporter.cpp\
acs_rtr_gohtransmitinfo.cpp\
acs_rtr_gohfilereporter.cpp\
acs_rtr_blockreport.cpp\
acs_rtr_blocktocommitstore.cpp\
acs_rtr_gohblockreporter.cpp\
acs_rtr_blockbuild.cpp\
acs_rtr_acachannel.cpp\
acs_rtr_outputqueue.cpp\
acs_rtr_manager.cpp\
acs_rtr_statistics.cpp\
acs_rtr_events.cpp\
acs_rtr_main.cpp\
acs_rtr_service.cpp\
acs_rtr_ha_service.cpp\
acs_rtr_server.cpp\
acs_rtr_blockbased_objectImpl.cpp\
acs_rtr_errorsupportbase.cpp\
acs_rtr_systemconfig.cpp\
acs_rtr_statistics.cpp\
acs_rtr_logger.cpp\
acs_rtr_hashkey_objectImpl.cpp\

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files

OBJFILES =	$(SRCFILES:%.cpp=%.obj) 

## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC

ACS_RTR = acs_rtrd
ACS_RTR_DBG = acs_rtrd_dbg
ACS_RTR_EXE = $(OUTDIR)/$(ACS_RTR)

ACS_RTR_OBJFILES = $(SRCFILES:%.cpp=%.obj) 

DEPS := $(ACS_RTR_OBJFILES:.obj=.d) 

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all

all: $(ACS_RTR_EXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_RTR_EXE) 
		$(SILENT)$(RM) $(ACS_RTR_EXE) 


$(ACS_RTR_EXE): $(ACS_RTR_OBJFILES) $(METRICS)
		$(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(ACS_RTR_EXE)  $(OBJDIR)/*.obj $(LIBSDIR) $(ACS_RTR_LIBS)
		$(call stripp,$(ACS_RTR))
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

	
