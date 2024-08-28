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
#            2010-11-17   xgencol   Adjusted
# **********************************************************************
CNZ_NAME ?= prc_cnz
CAA_NAME ?= prcapi_evt_caa
CXC_NAME ?= prc_cxc

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = acs
ACS_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(ACS_ROOT)/common

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

# If the product is a shared object library, here you can specify the name and the version numbers of the library
# You can also specify these variable as arguments of make command.
# For example: make LIB_NAME=ACS_DSD2 LIB_RELEASE_BUILD=1
LIB_NAME ?= acs_prc_evt
LIB_RELEASE_MAJOR ?= 1
LIB_RELEASE_MINOR ?= 0
LIB_RELEASE_BUILD ?= 2

OUTDIR = $(CXC_PATH)/bin/lib_int

## Object files
PRC_OBJ = $(OBJDIR)/acs_prc_evt.obj

## Setting the so library compiler information
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

# here you can add more defines
EXTRA_DEFINES += -DACS_PRCBIN_REVISION="$(CXC_VER)"

EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

## here you can add own compiler flags
CFLAGS += -O3

# here you can add own linker flags
#LDFLAGS += -s

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

# here you can add own lib paths
LIBSDIR += -L"$(CXC_PATH)/bin/lib_ext"
LIBSDIR += -L"$(CXC_PATH)/bin/lib_int"
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L$(COREMW_SDK_LIB)

# here you can add own Include paths and/or other includes
CINCLUDES += -I"$(CAA_API_EVT_DIR)"
CINCLUDES += -I"$(AP_SDK_INC)"

CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(COM_SDK_INC)

LIBS += $(COREMW_LIBS)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(LIB_FILENAME)
#all: $(LIB_EXT)/$(LIB_FILENAME)

#$(LIB_EXT)/$(LIB_FILENAME): $(PRC_OBJ)
$(OUTDIR)/$(LIB_FILENAME): $(PRC_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating library: $(LIB_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(OUTDIR)/$(LIB_FILENAME) $(PRC_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
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

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
#	$(call copy_header_file_in_ap_sdk_inc,acs_prc_types.h,inc/acs_prc_types.h);
	$(call ct_co_ap_sdk_lib_dir)
	$(CP) $(OUTDIR)/$(LIB_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_mkelem,$(AP_SDK_LIB)/$(LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_FILENAME),$(LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_SONAME),$(LIB_LINKER_NAME))
#	$(call ct_ci,$(AP_SDK_LIB))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

