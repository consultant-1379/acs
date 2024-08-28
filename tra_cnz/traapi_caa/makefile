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
#	     2010-11-08   xgencol   Adjusted
# **********************************************************************

CNZ_NAME ?= tra_cnz
CAA_NAME ?= traapi_caa
CXC_NAME ?= tra_cxc

include $(COMMON_ROOT)/common.mk 

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

#DOXYGENFILECONFIG := $(TOOLS_PATH)/linux_conf_file/Doxyfile.config

#DOCDIR = $(ACS_ROOT)/tra_cnz/tra_cxc/doc
#DOXYGEN_OUTPUT=$(DOCDIR)/docgen
#BLOCK_NAME=TRA_API_CAA
#BLOCK_VERSION=2_0
#BLOCK_SEARCH=./src ./inc
#export DOXYGEN_OUTPUT
#export BLOCK_NAME
#export BLOCK_VERSION
#export BLOCK_SEARCH

LIB_NAME = acs_tra

LIB_RELEASE_MAJOR ?= 3
LIB_RELEASE_MINOR ?= 7
LIB_RELEASE_BUILD ?= 1

OUTDIR = $(CXC_PATH)/bin/lib_ext

LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

# here you can add own lib paths
LIBSDIR += -L"$(CXC_PATH)/bin"

## here you can add own compiler flags
CFLAGS += -O3

#LDFLAGS += -s
LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

#EXTRA_DEFINES ?= -D__TRALIB_DEBUG_ENABLED__
EXTRA_DEFINES ?= 

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes
CINCLUDES += -I"$(INCINTDIR)"
CINCLUDES += -I"$(INCEXTDIR)"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(LIB_LOGCPLUS_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"

# here you can add own libs
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB)
LIBSDIR += -L$(AP_SDK_LIB)

LIBS += -lACE -llog4cplus -lacs_aeh

VPATH += $(SRCDIR) $(OUTDIR) $(LIB_EXT) $(INCDIR) $(OBJDIR) $(LIB_EXT)

TRA_OBJ =	\
	$(OBJDIR)/ACS_TRA_AutoflushThread.obj \
	$(OBJDIR)/ACS_TRA_AutoflushThreadManager.obj \
	$(OBJDIR)/ACS_TRA_common.obj \
	$(OBJDIR)/ACS_TRA_ConfigChangeWatchThread.obj \
	$(OBJDIR)/ACS_TRA_lib.obj \
	$(OBJDIR)/ACS_TRA_Logging.obj \
	$(OBJDIR)/ACS_TRA_trace.obj \
	$(OBJDIR)/ACS_TRA_SigMaskUtil.obj

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(LIB_FILENAME)

$(OUTDIR)/$(LIB_FILENAME): $(TRA_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(LIB_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(CINCLUDES) -o $(OUTDIR)/$(LIB_FILENAME) $(TRA_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LIB_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(LIB_FILENAME)

