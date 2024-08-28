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

SOFTWARE_TYPE ?= so

CNZ_NAME ?= dsd_cnz
CAA_NAME ?= dsdapi_caa
CXC_NAME ?= dsd_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

MODULE_NAME = DSD_API
MODULE_VERSION = 1_0
MODULE_SEARCH = ./src ./$(INCEXTDIR) ./$(INCINTDIR)
export DOXYGEN_OUTPUT
export MODULE_NAME
export MODULE_VERSION
export MODULE_SEARCH

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make LIB_NAME=ACS_DSD2 LIB_REL_BUILD=1
LIB_NAME ?= acs_dsd
LIB_REL_MAJOR ?= 2
LIB_REL_MINOR ?= 17
LIB_REL_BUILD ?= 1

## Setting the so library compiler information
LIB_VERSION ?= $(LIB_REL_MAJOR).$(LIB_REL_MINOR).$(LIB_REL_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_REL_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

## Object files
MODULE_OBJS =	\
	$(OBJDIR)/ACS_DSD_Acceptor.obj \
	$(OBJDIR)/ACS_DSD_AcceptorProvider.obj \
	$(OBJDIR)/ACS_DSD_AceCallbackLogger.obj \
	$(OBJDIR)/ACS_DSD_Client.obj \
	$(OBJDIR)/ACS_DSD_Communicator.obj \
	$(OBJDIR)/ACS_DSD_ConfigurationHelper.obj \
	$(OBJDIR)/ACS_DSD_Connector.obj \
	$(OBJDIR)/ACS_DSD_ConnectorProvider.obj \
	$(OBJDIR)/ACS_DSD_CPStateNotifier.obj \
	$(OBJDIR)/ACS_DSD_IOStream.obj \
	$(OBJDIR)/ACS_DSD_IOStreamProvider.obj \
	$(OBJDIR)/ACS_DSD_LibGlobalsStartup.obj \
	$(OBJDIR)/ACS_DSD_Logger4cplus.obj \
	$(OBJDIR)/ACS_DSD_LoggerTra.obj \
	$(OBJDIR)/ACS_DSD_MacrosConstants.obj \
	$(OBJDIR)/ACS_DSD_MessageReceiver.obj \
	$(OBJDIR)/ACS_DSD_MessageSender.obj \
	$(OBJDIR)/ACS_DSD_NumberSequencer.obj \
	$(OBJDIR)/ACS_DSD_OCPMessageReceiver.obj \
	$(OBJDIR)/ACS_DSD_OCPMessageSender.obj \
	$(OBJDIR)/ACS_DSD_PrimitiveDataHandler.obj \
	$(OBJDIR)/ACS_DSD_ProtocolHandler.obj \
	$(OBJDIR)/ACS_DSD_Receiver.obj \
	$(OBJDIR)/ACS_DSD_Sender.obj \
	$(OBJDIR)/ACS_DSD_Server.obj \
	$(OBJDIR)/ACS_DSD_Session.obj \
	$(OBJDIR)/ACS_DSD_Trace.obj \
	$(OBJDIR)/ACS_DSD_TraTracer.obj

DEPS := $(MODULE_OBJS:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

## here you can add own lib paths
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

#EXTRA_DEFINES += -DACS_DSD_API_FLAGGED_SEND_DISABLED
EXTRA_DEFINES += -DACS_DSD_ACE_MSG_PEEK_BUG_WORK_AROUND

OUTDIR = $(CXCDIR)/bin/lib_ext

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"$(INCINTDIR)"
CINCLUDES += -I"$(INCEXTDIR)"
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I"$(LIB_LOGCPLUS_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"

# here you can add own libs
LIBS += -lrt -lpthread -lacs_csapi -lacs_prc -lacs_apgcc -lacs_tra -llog4cplus

VPATH += $(SRCDIR) $(OUTDIR) $(LIB_EXT) $(INCINTDIR) $(INCEXTDIR) $(OBJDIR)

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
	$(call stripp,$(LIB_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: prebuild
prebuild:
	$(call ct_co,$(OUTDIR)/$(LIB_FILENAME))
	$(call ct_co,$(OUTDIR)/.debug/$(LIB_FILENAME_DBG))

.PHONY: postbuild
postbuild:
	$(call ct_ci,$(OUTDIR)/$(LIB_FILENAME))
	$(call ct_ci,$(OUTDIR)/.debug/$(LIB_FILENAME_DBG))

.PHONY: undobuild
undobuild:
	$(call ct_unco,$(OUTDIR)/$(LIB_FILENAME))
	$(call ct_unco,$(OUTDIR)/.debug/$(LIB_FILENAME_DBG))

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(OUT_FILENAME)

.PHONY: copy_to_lotc
copy_to_lotc:
	$(SILENT)$(ECHO) 'Copying application $(OUTDIR)/$(OUT_FILENAME) onto the LOTC...'
	$(SILENT) sftp -b copy-to-simlotc.batch $(LOTC_USER) >&/dev/null
	
