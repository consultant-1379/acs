# **********************************************************************
#
# Short description:
# Makefile template for XBRM Private Implementation Library
# **********************************************************************
#
# Ericsson AB 2023 All rights reserved.
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
#            
#
# **********************************************************************
CNZ_NAME ?= xbrm_cnz
CAA_NAME ?= xbrmadm_caa
CXC_NAME ?= xbrm_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make LIB_NAME=ACS_DSD2 LIB_RELEASE_BUILD=1
LIB_NAME ?= acs_xbrm
LIB_RELEASE_MAJOR ?= 1
LIB_RELEASE_MINOR ?= 8
LIB_RELEASE_BUILD ?= 4

XBRM_INC = $(CAA_PATH)/inc

## Object files
XBRM_OBJ = 	$(OBJDIR)/ACS_XBRM_BackupManagerHandler.obj \
			$(OBJDIR)/ACS_XBRM_Event.obj \
			$(OBJDIR)/ACS_XBRM_PeriodicEventHandler.obj \
			$(OBJDIR)/ACS_XBRM_ScheduleHandler.obj \
			$(OBJDIR)/ACS_XBRM_BrmHandler.obj \
			$(OBJDIR)/ACS_XBRM_HA_AppManager.obj \
			$(OBJDIR)/ACS_XBRM_SRV.obj \
			$(OBJDIR)/ACS_XBRM_Server.obj \


## Setting the so library compiler information
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

OUTDIR = $(CXC_PATH)/bin/lib_ext

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

## here you can add own compiler flags
CFLAGS += -O3

# here you can add own linker flags
LDFLAGS +=

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

# here you can add own lib paths
LIBSDIR += -L"$(CXC_PATH)/bin"
LIBSDIR += -L"$(AP_SDK_LIB)"

# here you can add own Include paths and/or other includes
CINCLUDES += -I"$(INCDIR)"
CINCLUDES += -I"$(XBRM_INC)"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I$(ACE_SDK_INC)

# here you can add own libs


.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(LIB_FILENAME)

$(OUTDIR)/$(LIB_FILENAME): $(XBRM_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(LIB_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(OUTDIR)/$(LIB_FILENAME) $(XBRM_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
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


