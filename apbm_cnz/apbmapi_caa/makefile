# **********************************************************************
#
# Short description:
# Makefile template for DSD Private Implementation Library
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
#            2010-02-19   teiggam   Created
#            2010-07-30   xnicmut   Adjusted
#            2010-11-19   xgencol   Adjusted
# **********************************************************************

CNZ_NAME ?= apbm_cnz
CAA_NAME ?= apbmapi_caa
CXC_NAME ?= apbm_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

MODULE_NAME = APBM_API
MODULE_VERSION = 1_0
MODULE_SEARCH = ./$(SRCDIR) ./$(INCEXTDIR) ./$(INCINTDIR)
export DOXYGEN_OUTPUT
export MODULE_NAME
export MODULE_VERSION
export MODULE_SEARCH

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make LIB_NAME=ACS_DSD2 LIB_REL_BUILD=1
LIB_NAME ?= acs_apbm
LIB_REL_MAJOR ?= 3
LIB_REL_MINOR ?= 3
LIB_REL_BUILD ?= 1
OUTDIR = $(CXCDIR)/bin/lib_ext
## Setting the so library compiler information
LIB_VERSION ?= $(LIB_REL_MAJOR).$(LIB_REL_MINOR).$(LIB_REL_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_REL_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

## Object files
MODULE_OBJS =	\
	$(OBJDIR)/acs_apbm_api_imp.obj \
	$(OBJDIR)/acs_apbm_api.obj \
	$(OBJDIR)/acs_apbm_configurationhelper.obj \
	$(OBJDIR)/acs_apbm_primitivedatahandler.obj \
	$(OBJDIR)/acs_apbm_trace.obj \
	$(OBJDIR)/acs_apbm_trapmessage_imp.obj \
	$(OBJDIR)/acs_apbm_trapmessage.obj
	
DEPS := $(MODULE_OBJS:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

## here you can add own lib paths
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(3PP_SDK_PATH)/log4cplus/4_0/log4cplus/lib"
LIBSDIR += -L$(COREMW_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(ACE_SDK_LIB)

## here you can add own compiler flags
CFLAGS +=

# here you can add own linker flags
LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)
OUT_FILENAME = $(LIB_FILENAME)

EXTRA_DEFINES += -DACS_APBM_HAS_TRACE -DACS_APBM_TRACE_HAS_FUNCTION_TRACE -DACS_APBM_TRACE_USE_PRETTY_FUNCTION -DACS_APBM_TRACE_USE_FILE_INFO

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"$(INCEXTDIR)"
CINCLUDES += -I"$(INCINTDIR)"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I"$(CAA_ADM_DIR)/inc"
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(LIB_XERCES_SDK_INC)
CINCLUDES += -I$(LOTC_SDK_INC)

# here you can add own libs
LIBS += -lrt -lpthread -lacs_tra -llog4cplus -lacs_dsd -lacs_apgcc

VPATH += $(SRCDIR) $(OUTDIR) $(INCEXTDIR) $(INCINTDIR) $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(OUT_FILENAME)

$(OUTDIR)/$(OUT_FILENAME): $(MODULE_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building library: $(OUT_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(OUTDIR)/$(OUT_FILENAME) $(MODULE_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LIB_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(OUT_FILENAME)
	
