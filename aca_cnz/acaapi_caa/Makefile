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

CNZ_NAME ?= aca_cnz
CAA_NAME ?= acaapi_caa
CXC_NAME ?= aca_cxc

include $(COMMON_ROOT)/common.mk

VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

CURDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin/lib_ext
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INTINCDIR   = $(CURDIR)/inc_int
EXTINCDIR   = $(CURDIR)/inc_ext
ADMINCDIR = ../acaadm_caa/inc
TESTDIR  = $(CURDIR)/test

ACS_ACA_LIB_NAME = acs_aca

ACS_ACA_LIB_RELEASE_MAJOR ?= 3
ACS_ACA_LIB_RELEASE_MINOR ?= 5
ACS_ACA_LIB_RELEASE_BUILD ?= 2
ACS_ACA_LIB_VERSION ?= $(ACS_ACA_LIB_RELEASE_MAJOR).$(ACS_ACA_LIB_RELEASE_MINOR).$(ACS_ACA_LIB_RELEASE_BUILD)
ACS_ACA_LIB_FILENAME = lib$(ACS_ACA_LIB_NAME).so.$(ACS_ACA_LIB_VERSION)
ACS_ACA_LIB_FILENAME_DBG = lib$(ACS_ACA_LIB_NAME).so.$(ACS_ACA_LIB_VERSION)_dbg
ACS_ACA_LIB_SONAME = lib$(ACS_ACA_LIB_NAME).so.$(ACS_ACA_LIB_RELEASE_MAJOR)
ACS_ACA_LIB_LINKER_NAME = lib$(ACS_ACA_LIB_NAME).so

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)
CINCLUDES += -I$(EXTINCDIR) -I$(INTINCDIR)  -I$(ADMINCDIR)


# here you can add own lib paths
LIBSDIR  += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB)
LIBSDIR  += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(ACS_ROOT)/aca_cnz/aca_cxc/bin/lib_ext 

# here you can add own compiler flags
CFLAGS += -O3 -fPIC -fmessage-length=0

# here you can add more defines
EXTRA_DEFINES += -DACS_ACA_API_HAS_TRACE
EXTRA_DEFINES += -DACS_ACA_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DACS_ACA_TRACE_HAS_FUNCTION_TRACE
EXTRA_DEFINES += -DACS_ACA_TRACE_USE_PRETTY_FUNCTION
EXTRA_CFLAGS += $(EXTRA_DEFINES)

# here you can add own Assembler flags
ASMFLAGS += 

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 
LIBS +=  -lrt -lpthread 

ACS_ACA_LIBS = $(LIBS) -lacs_tra -lacs_apgcc  


ADMSRCDIR = ../acaadm_caa/src
## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(ADMSRCDIR)

## Source files
#

L_FILES += $(ACS_ACAOBJ:%=$(BLOCKDIR)/src/%) 

ACS_ACASRC = \
acs_aca_message_store_access_point.cpp \
acs_aca_message_store_message.cpp \
acs_aca_internal_message_store_access_point.cpp \
acs_aca_ms_msip_protocol.cpp \
acs_aca_ms_commit_file.cpp \
acs_aca_ms_statistics_file.cpp \
acs_aca_msa_msg_store.cpp \
acs_aca_msa_r2.cpp \
acs_aca_message_store_interface.cpp \
acs_aca_ms_msg_store.cpp \
acs_aca_api_tracer.cpp \
acs_aca_configuration_helper.cpp

## Object files
ACS_ACAOBJFILES = $(OBJDIR)/acs_aca_message_store_access_point.obj \
$(OBJDIR)/acs_aca_internal_message_store_access_point.obj \
$(OBJDIR)/acs_aca_message_store_message.obj \
$(OBJDIR)/acs_aca_ms_msip_protocol.obj \
$(OBJDIR)/acs_aca_ms_commit_file.obj \
$(OBJDIR)/acs_aca_ms_statistics_file.obj \
$(OBJDIR)/acs_aca_msa_msg_store.obj \
$(OBJDIR)/acs_aca_msa_r2.obj \
$(OBJDIR)/acs_aca_message_store_interface.obj \
$(OBJDIR)/acs_aca_ms_msg_store.obj \
$(OBJDIR)/acs_aca_api_tracer.obj \
$(OBJDIR)/acs_aca_configuration_helper.obj


## Object files
#
OBJS =  \
        $(SRCFILES:%.cpp=%.obj)

ACS_ACAOBJ = \
	$(ACS_ACASRC:%.cpp=%.obj)

ACS_CHB_CPOBJECTOBJ = \
	$(ACS_CHB_CPOBJECTSRC:%.cpp=%.obj)

DEPS  = $(ACS_ACAOBJ:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif


## Build instructions
#

ACS_ACA_APLIB = $(OUTDIR)/$(ACS_ACA_LIB_FILENAME)
ACS_ACA_APLIB_SO = $(OUTDIR)/$(ACS_ACA_LIB_SONAME)

.PHONY: clean 
.PHONY: distclean 
.PHONY: all
 
all: $(ACS_ACA_APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_ACA_APLIB)'
		$(SILENT)$(RM) $(ACS_ACA_APLIB) 


$(ACS_ACA_APLIB): $(ACS_ACAOBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library ACS ACA: $@'
		$(SILENT)$(RM) $(ACS_ACA_APLIB)
		$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(ACS_ACA_APLIB) $(ACS_ACAOBJFILES) $(LDFLAGS) $(LIBSDIR) $(ACS_ACA_LIBS) 
		$(call stripp,$(ACS_ACA_LIB_FILENAME))
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(call ct_co_ap_sdk_lib_dir)
	$(CP) $(OUTDIR)/$(ACS_ACA_LIB_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_mkelem,$(AP_SDK_LIB)/$(ACS_ACA_LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(ACS_ACA_LIB_FILENAME),$(ACS_ACA_LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(ACS_ACA_LIB_SONAME),$(ACS_ACA_LIB_LINKER_NAME))




.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

