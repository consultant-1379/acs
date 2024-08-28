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
#            2011-05-03   xfabron   Adjusted
# **********************************************************************

CNZ_NAME ?= aeh_cnz
CAA_NAME ?= aehapi_caa
CXC_NAME ?= aeh_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make LIB_NAME=ACS_DSD2 LIB_RELEASE_BUILD=1
LIB_NAME ?= acs_aeh
LIB_RELEASE_MAJOR ?= 2
LIB_RELEASE_MINOR ?= 7
LIB_RELEASE_BUILD ?= 1

OUTDIR = $(CXC_PATH)/bin/lib_ext

## Object files
AEH_OBJ = $(OBJDIR)/acs_aeh_error.obj\
          $(OBJDIR)/acs_aeh_errorimpl.obj\
          $(OBJDIR)/acs_aeh_evreport.obj\
          $(OBJDIR)/acs_aeh_evreportimpl.obj\
          $(OBJDIR)/acs_aeh_eventmsg.obj\
          $(OBJDIR)/acs_aeh_exception.obj\
          $(OBJDIR)/acs_aeh_communicationhandler.obj \
		  $(OBJDIR)/acs_aeh_signalhandler.obj             
          


#APDSDK_PRC_API_HEADER_FILES = acs_prc_types.h acs_prc_api.h ACS_PRC_ispapi.h ACS_PRC_Process.h


## Setting the so library compiler information
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so


VPATH += $(SRCDIR) $(OUTDIR) $(LIB_EXT) $(INCINTDIR) $(INCEXTDIR) $(OBJDIR)

## here you can add own compiler flags
CFLAGS += -O3

# here you can add own linker flags
#LDFLAGS += -s

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

# here you can add own lib paths
LIBSDIR += -L"$(CXCDIR)/bin"
LIBSDIR += -L"$(AP_SDK_LIB)"

# here you can add own Include paths and/or other includes
#CINCLUDES += -I"$(INCDIR)"
CINCLUDES += -I"$(INCINTDIR)"
CINCLUDES += -I"$(INCEXTDIR)"

CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

LIBS += -lrt -lpthread -lacs_prc -lacs_tra -lacs_apgcc

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(LIB_FILENAME)

$(OUTDIR)/$(LIB_FILENAME): $(AEH_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(LIB_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(OUTDIR)/$(LIB_FILENAME) $(AEH_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
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


