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
# PA1		 25/11/2010		XTANAGG	Initial Release                                                       
# **********************************************************************

CNZ_NAME ?= aca_cnz
CAA_NAME ?= acacmd_caa
CXC_NAME ?= aca_cxc

include $(COMMON_ROOT)/common.mk

VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INCDIR   = $(CURDIR)/inc
ACAAPISRCDIR = $(CURDIR)/../acaapi_caa/src
ACAAPIINTINCDIR = $(CURDIR)/../acaapi_caa/inc_int
ACAAPIEXTINCDIR = $(CURDIR)/../acaapi_caa/inc_ext
ACAADMINCDIR = $(CURDIR)/../acaadm_caa/inc
ACAADMSRCDIR = $(CURDIR)/../acaadm_caa/src

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode


# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) 

# here you can add more defines
EXTRA_DEFINES += -DACS_ACA_API_HAS_TRACE
EXTRA_DEFINES += -DACS_ACA_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DACS_ACA_TRACE_HAS_FUNCTION_TRACE
EXTRA_DEFINES += -DACS_ACA_TRACE_USE_PRETTY_FUNCTION
EXTRA_CFLAGS += $(EXTRA_DEFINES)

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)  
CINCLUDES += -I$(INCDIR) -I$(ACAAPIINTINCDIR) -I$(ACAAPIEXTINCDIR) -I$(ACAADMINCDIR)

## # here you can add own libs
LIBS +=  -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf 
LIBS +=  -lrt -lpthread -lacs_apgcc -lacs_tra -llog4cplus -lacs_prc 
LIBS +=  -lacs_dsd -lacs_csapi -lacs_aeh -lboost_serialization -lboost_filesystem  -lboost_system -lsec_crypto_api

LIBSDIR +=  -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(SEC_SDK_LIB) 
LIBSDIR +=  -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) $(ACAAPISRCDIR) $(ACAADMSRCDIR)

## Source files
#
SRCFILES =  \
acs_aca_api_tracer.cpp \
acs_aca_command.cpp \
acs_aca_command_invoker.cpp \
acs_aca_msd_cmd_msdls.cpp \
acs_aca_structured_request_builder.cpp \
acs_aca_msg_builder.cpp \
acs_aca_msg_director.cpp  \
acs_aca_structured_request.cpp \
acs_aca_msd_cmd_stub.cpp \
acs_aca_mcli.cpp\
acs_aca_ms_statistics_file.cpp \
acs_aca_message_number.cpp \
acs_aca_ms_commit_file.cpp \
acs_aca_structured_response.cpp \
acs_aca_structured_response_builder.cpp \


IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(CURDIR)/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) \



## Build instructions
#
#APNAME  = BLOCKNAME1API_CXC
APNAME = msdls
APNAME_DBG = msdls_dbg
APEXE = $(OUTDIR)/$(APNAME)

.PHONY: all
all: $(APEXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(APEXE)'
		$(SILENT)$(RM) $(APEXE)
		$(RM) $(OUTDIR)/.debug/$(APNAME_DBG)


		
$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(APEXE) $(wildcard $(OBJDIR)/*.obj) $(LIBSDIR) $(LIBS)
		$(call stripp,$(APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    

.PHONY: metrics
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
		$(LINT) $(wildcard $(BLOCKDIR)/src/*)
		$(SEPARATOR_STR)

	
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^


.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '******     NOT NEEDED      *******'
	$(SILENT)$(ECHO) '**********************************'

#DO NOT DELETE THIS LINE -- make depend needs it
