# **********************************************************************
#
# Short description:
# Makefile template for DSD Daemon
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

SOFTWARE_TYPE ?= app

CNZ_NAME ?= dsd_cnz
CAA_NAME ?= dsdcmd_caa
CXC_NAME ?= dsd_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

MODULE_NAME = DSD_DAEMON
MODULE_VERSION = 1_0
MODULE_SEARCH = ./src ./inc
export DOXYGEN_OUTPUT
export MODULE_NAME
export MODULE_VERSION
export MODULE_SEARCH

# Setting the application name
APP_FILENAME = dsdls

APP_FILENAME_DBG = dsdls_dbg

## Object files
MODULE_OBJS =	\
	$(OBJDIR)/acs_dsd_command.obj \
	$(OBJDIR)/acs_dsd_dsdls.obj
	
DEPS := $(MODULE_OBJS:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

## here you can add own lib paths
LIBSDIR += -L"$(LIB_EXT)"
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(LIB_LOGCPLUS_SDK_LIB)"

## here you can add own compiler flags
CFLAGS +=

# here you can add own linker flags
LDFLAGS +=

ifeq ($(SOFTWARE_TYPE),so)
LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)
OUT_FILENAME = $(LIB_FILENAME)
else
OUT_FILENAME = $(APP_FILENAME)
endif

EXTRA_DEFINES ?=

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# Compilation Options
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"inc"
CINCLUDES += -I"$(CAA_API_DIR)/$(INCEXTDIR)"
CINCLUDES += -I"$(CAA_API_DIR)/$(INCINTDIR)"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(LIB_LOGCPLUS_SDK_INC)"

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

# here you can add own libs
LIBS += -lrt -lpthread -lacs_dsd -lacs_csapi -lacs_prc -lacs_apgcc -lacs_tra -llog4cplus

VPATH += $(SRCDIR) $(OUTDIR) inc $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(OUT_FILENAME)

$(OUTDIR)/$(OUT_FILENAME): $(MODULE_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
ifeq ($(SOFTWARE_TYPE),so)
		$(SILENT)$(ECHO) 'Creating library: $(OUT_FILENAME)'
else
		$(SILENT)$(ECHO) 'Creating Application: $(OUT_FILENAME)'
endif
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(OUTDIR)/$(OUT_FILENAME) $(MODULE_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APP_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: prebuild
prebuild:
	$(call ct_co,$(OUTDIR)/$(APP_FILENAME))
	$(call ct_co,$(OUTDIR)/.debug/$(APP_FILENAME_DBG))

.PHONY: postbuild
postbuild:
	$(call ct_ci,$(OUTDIR)/$(APP_FILENAME))
	$(call ct_ci,$(OUTDIR)/.debug/$(APP_FILENAME_DBG))

.PHONY: undobuild
undobuild:
	$(call ct_unco,$(OUTDIR)/$(APP_FILENAME))
	$(call ct_unco,$(OUTDIR)/.debug/$(APP_FILENAME_DBG))

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(OUT_FILENAME)

.PHONY: documentation
documentation:
	$(SILENT)$(ECHO) 'documentation updating ...'
	$(SILENT)$(ECHO) ' Removing $(DOXYGEN_OUTPUT) directory'
	$(SILENT)$(RM) -r -f $(DOXYGEN_OUTPUT)
	$(SILENT)$(ECHO) ' Creating $(DOXYGEN_OUTPUT) directory'
	$(SILENT)mkdir $(DOXYGEN_OUTPUT)
	$(SILENT)$(DOXYGEN) -g $(DOXYGENFILECONFIG)


.PHONY: metrics
metrics:
	$(SILENT)$(ECHO) 'Metrics updating ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'
