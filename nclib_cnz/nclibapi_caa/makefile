# **********************************************************************
#
# Short description:
# Makefile (at CAA level) for NCLIB Library
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
#            2012-09-27   xnicmut   Created
# **********************************************************************

CNZ_NAME ?= nclib_cnz
CAA_NAME ?= nclibapi_caa
CXC_NAME ?= nclib_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

DOXYGENFILECONFIG := $(TOOLS_PATH)/linux_conf_file/Doxyfile.config

MODULE_NAME = NCLIB_API
MODULE_VERSION = 1_0
MODULE_SEARCH = ./$(SRCDIR) ./$(INCEXTDIR) ./$(INCINTDIR)
export DOXYGEN_OUTPUT
export MODULE_NAME
export MODULE_VERSION
export MODULE_SEARCH

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make LIB_NAME=ACS_DSD2 LIB_REL_BUILD=1
LIB_NAME ?= acs_nclib
LIB_REL_MAJOR ?= 1
LIB_REL_MINOR ?= 4
LIB_REL_BUILD ?= 0
OUTDIR = $(CXC_PATH)/bin/lib_ext

NO_TRACING ?= 0

## Setting the so library compiler information
LIB_VERSION ?= $(LIB_REL_MAJOR).$(LIB_REL_MINOR).$(LIB_REL_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_REL_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

## Object files
MODULE_OBJS =	\
	$(OBJDIR)/acs_nclib_constants.obj \
	$(OBJDIR)/acs_nclib_errorinfoprovider.obj \
	$(OBJDIR)/acs_nclib_library.obj \
	$(OBJDIR)/acs_nclib_session_impl.obj \
	$(OBJDIR)/acs_nclib_session.obj \
	$(OBJDIR)/acs_nclib_factory.obj \
	$(OBJDIR)/acs_nclib_builder.obj \
	$(OBJDIR)/acs_nclib_edit_config.obj \
	$(OBJDIR)/acs_nclib_filter_impl.obj \
	$(OBJDIR)/acs_nclib_get_config.obj \
	$(OBJDIR)/acs_nclib_get.obj \
	$(OBJDIR)/acs_nclib_hello_impl.obj \
	$(OBJDIR)/acs_nclib_message.obj \
	$(OBJDIR)/acs_nclib_rpc_error_impl.obj \
	$(OBJDIR)/acs_nclib_rpc_impl.obj \
	$(OBJDIR)/acs_nclib_rpc_reply_impl.obj \
	$(OBJDIR)/acs_nclib_close_session.obj \
	$(OBJDIR)/acs_nclib_commit.obj \
	$(OBJDIR)/acs_nclib_lock.obj \
	$(OBJDIR)/acs_nclib_unlock.obj \
	$(OBJDIR)/acs_nclib_action.obj \
	$(OBJDIR)/acs_nclib_tcp_client.obj \
	$(OBJDIR)/acs_nclib_tcp_session_impl.obj \
	$(OBJDIR)/acs_nclib_udp_impl.obj \
	$(OBJDIR)/acs_nclib_udp_reply_impl.obj \
	$(OBJDIR)/acs_nclib_udp_client.obj \
	$(OBJDIR)/acs_nclib_udp_session_impl.obj \

ifeq ($(NO_TRACING),0)
	MODULE_OBJS += $(OBJDIR)/acs_nclib_trace.obj
endif
	
DEPS := $(MODULE_OBJS:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

## here you can add own lib paths
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(LIB_SSH2_SDK_LIB)"
LIBSDIR += -L"$(ACE_SDK_LIB)"
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)
ifeq ($(NO_TRACING),0)
	LIBSDIR += -L"$(LIB_LOGCPLUS_SDK_LIB)"
endif

## here you can add own compiler flags
CFLAGS += -Wno-ignored-qualifiers

# here you can add own linker flags
LDFLAGS = -shared -Wl,-soname=$(LIB_SONAME) -Wl,--export-dynamic -fpic
OUT_FILENAME = $(LIB_FILENAME)

EXTRA_DEFINES += -DACS_NCLIB_SET_SO_KEEPALIVE_ON_CLIENT_SOCKET
EXTRA_DEFINES += -DACS_NCLIB_TRACE_CONTROL_FROM_FILE_DISABLED
EXTRA_DEFINES += -DACS_NCLIB_TRACE_HAS_FUNCTION_TRACE -DACS_NCLIB_TRACE_USE_PRETTY_FUNCTION -DACS_NCLIB_TRACE_USE_SOURCE_INFO
ifeq ($(NO_TRACING),0)
	EXTRA_DEFINES += -DACS_NCLIB_HAS_TRACE
endif

#LDFLAGS += -e __acs_nclib_main
#EXTRA_DEFINES += -DACS_NCLIB_HAS_VERSION_BANNER

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"$(INCEXTDIR)"
CINCLUDES += -I"$(INCINTDIR)"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I"$(LIB_SSH2_SDK_INC)"
CINCLUDES += -I"$(BOOST_SDK_INC)"

# here you can add own libs
LIBS += -lrt -lpthread -lACE -lssh2 -lssl -lz -lcrypto
ifeq ($(NO_TRACING),0)
	LIBS += -lacs_tra -llog4cplus
endif

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

