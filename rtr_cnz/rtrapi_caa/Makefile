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
CNZ_PATH = $(ACS_ROOT)/rtr_cnz
CNZ_NAME ?= rtr_cnz
CAA_NAME ?= rtrapi_caa
CXC_NAME ?= rtr_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

CAADIR = rtrapi_caa
CURDIR = $(CNZ_PATH)/$(CAADIR)
OUTDIR   = $(CXC_PATH)/bin/lib_int
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INTINCDIR   = $(CURDIR)/inc_int
EXTINCDIR   = $(CURDIR)/inc_ext
ADMINCDIR = $(ACS_ROOT)/rtr_cnz/rtradm_caa/inc
TESTDIR  = $(CURDIR)/test
SECINCDIR = $(SEC_SDK_INC)/sec

ACS_RTR_LIB_NAME = acs_rtr

ACS_RTR_LIB_RELEASE_MAJOR ?= 2
ACS_RTR_LIB_RELEASE_MINOR ?= 1
ACS_RTR_LIB_RELEASE_BUILD ?= 0
ACS_RTR_LIB_VERSION ?= $(ACS_RTR_LIB_RELEASE_MAJOR).$(ACS_RTR_LIB_RELEASE_MINOR).$(ACS_RTR_LIB_RELEASE_BUILD)
ACS_RTR_LIB_FILENAME = lib$(ACS_RTR_LIB_NAME).so.$(ACS_RTR_LIB_VERSION)
ACS_RTR_LIB_FILENAME_DBG = lib$(ACS_RTR_LIB_NAME).so.$(ACS_RTR_LIB_VERSION)_dbg
ACS_RTR_LIB_SONAME = lib$(ACS_RTR_LIB_NAME).so.$(ACS_RTR_LIB_RELEASE_MAJOR)
ACS_RTR_LIB_LINKER_NAME = lib$(ACS_RTR_LIB_NAME).so

## # here you can add own Include paths and/or other includes
#modified by me
CINCLUDES += -I$(EXTINCDIR) -I$(INTINCDIR)  -I$(ADMINCDIR) -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC) -I$(BOOST_SDK_INC) -I$(SEC_SDK_INC) -I$(SECINCDIR)

# here you can add own lib paths
LIBSDIR  += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(CNZ_PATH)/rtr_cxc/bin/lib_int -L$(SEC_SDK_LIB) -L$(BOOST_SDK_LIB) 

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
CFLAGS += -fPIC
CFLAGS += -fmessage-length=0

# here you can add own Assembler flags
ASMFLAGS += 

LDFLAGS += -s -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 
LIBS +=  -lrt -lpthread -lboost_filesystem

ACS_RTR_LIBS = $(LIBS) -lacs_tra -lacs_apgcc -lacs_aeh -lsec_crypto_api 

ADMSRCDIR = $(ACS_ROOT)/rtr_cnz/rtradm_caa/src
## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(ADMSRCDIR)

## Source files
#

L_FILES += $(ACS_RTROBJ:%=$(BLOCKDIR)/src/%) 

ACS_RTRSRC = acs_rtr_defaultvalues.cpp\
acs_rtr_lib.cpp\
acs_rtr_lib_events.cpp\
acs_rtr_tracer.cpp
 
## Object files
ACS_RTROBJFILES = $(OBJDIR)/acs_rtr_defaultvalues.obj \
$(OBJDIR)/acs_rtr_lib.obj \
$(OBJDIR)/acs_rtr_lib_events.obj \
$(OBJDIR)/acs_rtr_tracer.obj \

## Object files
#
OBJS =  \
        $(SRCFILES:%.cpp=%.obj)

ACS_RTROBJ = \
	$(ACS_RTRSRC:%.cpp=%.obj)

DEPS  = $(ACS_RTROBJ:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

## Build instructions
#

ACS_RTR_APLIB = $(OUTDIR)/$(ACS_RTR_LIB_FILENAME)
ACS_RTR_APLIB_SO = $(OUTDIR)/$(ACS_RTR_LIB_SONAME)

.PHONY: clean 
.PHONY: distclean 
.PHONY: all
 
all: $(ACS_RTR_APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(ACS_RTR_APLIB)'
		$(SILENT)$(RM) $(ACS_RTR_APLIB) 

$(ACS_RTR_APLIB): $(ACS_RTROBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library ACS RTR: $@'
		$(SILENT)$(RM) $(ACS_RTR_APLIB)
		$(CC) -o $(ACS_RTR_APLIB) $(ACS_RTROBJFILES) $(LDFLAGS) $(LIBSDIR) $(ACS_RTR_LIBS) 
		$(call stripp,$(ACS_RTR_LIB_FILENAME))
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(call ct_co_ap_sdk_lib_dir)
	$(CP) $(OUTDIR)/$(ACS_RTR_LIB_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_mkelem,$(AP_SDK_LIB)/$(ACS_RTR_LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(ACS_RTR_LIB_FILENAME),$(ACS_RTR_LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(ACS_RTR_LIB_SONAME),$(ACS_RTR_LIB_LINKER_NAME))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

