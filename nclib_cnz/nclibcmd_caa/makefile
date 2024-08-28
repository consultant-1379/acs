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
#            2014-01-28   estevol   Created
# **********************************************************************

CNZ_NAME ?= nclib_cnz
CAA_NAME ?= nclibcmd_caa
CXC_NAME ?= nclib_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

MODULE_NAME = NCLIB_CMD
MODULE_VERSION = 1_0
MODULE_SEARCH = ./$(SRCDIR) ./inc
export DOXYGEN_OUTPUT
export MODULE_NAME
export MODULE_VERSION
export MODULE_SEARCH

CAA_API_DIR = ../nclibapi_caa
export CAA_API_DIR

## Object files
#NCGET_FILES = $(ACS_ROOT)/apbm_cnz/apbmadm_caa/obj
#NCGET_MOREHEADERFILES = $(ACS_ROOT)/apbm_cnz/apbmadm_caa/inc

NCLIB_IRONSIDE_OBJS = \
        $(OBJDIR)/acs_nclib_ironsidecmd.obj \

NCLIB_NCGET_OBJS = \
	$(OBJDIR)/ncget.obj \
	$(OBJDIR)/acs_nclib_cmd.obj \

NCLIB_NCEDITCFG_OBJS = \
	$(OBJDIR)/nceditcfg.obj \
	$(OBJDIR)/acs_nclib_cmd.obj \
	
NCLIB_NCACTION_OBJS = \
	$(OBJDIR)/ncaction.obj \
	$(OBJDIR)/acs_nclib_cmd.obj \

## Dependencies

NCLIB_NCGET_DEPS := $(NCLIB_NCGET_OBJS:.obj=.d)

NCLIB_NCEDITCFG_DEPS := $(NCLIB_NCEDITCFG_OBJS:.obj=.d)

NCLIB_IRONSIDE_DEPS := $(NCLIB_IRONSIDE_OBJS:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
#include $(APBM_HWICONF_DEPS)
include $(NCLIB_NCGET_DEPS)
include $(NCLIB_NCEDITCFG_DEPS)
include $(NCLIB_IRONSIDE_DEPS)
include $(NCLIB_NCACTION_DEPS)
endif

## Application Command Names
NCLIB_NCGET_APPNAME := ncget
NCLIB_NCEDITCFG_APPNAME := nceditconfig
NCLIB_NCACTION_APPNAME := ncaction
NCLIB_IRONSIDE_APPNAME := ironsidecmd

#APBM_HWICONF_CMDNAME := $(APBM_HWICONF_APPNAME)
NCLIB_NCGET_CMDNAME := $(NCLIB_NCGET_APPNAME)
NCLIB_NCEDITCFG_CMDNAME := $(NCLIB_NCEDITCFG_APPNAME)
NCLIB_NCACTION_CMDNAME := $(NCLIB_NCACTION_APPNAME)
NCLIB_IRONSIDE_CMDNAME := $(NCLIB_IRONSIDE_APPNAME)

NCLIB_NCGET_CMDNAME_DBG := $(NCLIB_NCGET_APPNAME)_dbg
NCLIB_NCEDITCFG_CMDNAME_DBG := $(NCLIB_NCEDITCFG_APPNAME)_dbg
NCLIB_NCACTION_CMDNAME_DBG := $(NCLIB_NCACTION_APPNAME)_dbg
NCLIB_IRONSIDE_CMDNAME_DBG := $(NCLIB_IRONSIDE_APPNAME)_dbg

## here you can add own lib pathsEDITCFG
LIBSDIR += -L"$(CXCPATH)/bin/lib_ext"
LIBSDIR += -L"$(AP_SDK_LIB)"
#LIBSDIR += -L"$(LIB_LOGCPLUS_SDK_LIB)"
#LIBSDIR += -L"$(LIB_XERCES_SDK_LIB)"
#LIBSDIR += -L"$(LIB_SSH2_SDK_LIB)"
LIBSDIR += -L"$(ACE_SDK_LIB)"
LIBSDIR += -L"$(COREMW_SDK_LIB)"
#LIBSDIR += -L"$(SEC_SDK_LIB)"
LIBSDIR += -L"$(SEC_CREDU_SDK_LIB)"

## here you can add own compiler flags
CFLAGS += -Wno-empty-body -Wno-unused-parameter -Wno-unused-local-typedefs

# here you can add own linker flags
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB) -Wl,-rpath-link,$(ACE_SDK_LIB) -Wl,-rpath-link,$(COREMW_SDK_LIB) -Wl,-rpath-link,$(SEC_SDK_LIB) 
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB) -Wl,-rpath-link,$(BOOST_SDK_LIB) -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB) -Wl,-rpath-link,$(LOTC_SDK_LIB)

EXTRA_DEFINES += -DBS_DEBUG 
EXTRA_DEFINES += -g -DACS_APBM_HAS_TRACE -DACS_APBM_TRACE_HAS_FUNCTION_TRACE -DACS_APBM_TRACE_USE_PRETTY_FUNCTION -DACS_APBM_TRACE_USE_FILE_INFO

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"inc"
CINCLUDES += -I"$(ACS_ROOT)/nclib_cnz/nclibapi_caa/inc_int"
CINCLUDES += -I"$(CAA_API_DIR)/$(INCEXTDIR)"
CINCLUDES += -I"$(CAA_API_DIR)/$(INCINTDIR)"
CINCLUDES += -I"$(CAA_PIPAPI_DIR)/$(INCEXTDIR)"
CINCLUDES += -I"$(CAA_PIPAPI_DIR)/$(INCINTDIR)"
CINCLUDES += -I"$(CAA_ADM_DIR)/inc"
CINCLUDES += -I"$(AP_SDK_INC)" 
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I"$(LIB_XERCES_SDK_INC)"

NCGET_APEXE = $(OUTDIR)/$(NCLIB_NCGET_CMDNAME)
NCEDITCFG_APEXE = $(OUTDIR)/$(NCLIB_NCEDITCFG_CMDNAME)
NCACTION_APEXE = $(OUTDIR)/$(NCLIB_NCACTION_CMDNAME)
IRONSIDE_APEXE = $(OUTDIR)/$(NCLIB_IRONSIDE_CMDNAME)

# here you can add own libs
LIBS += -lrt -lpthread -lacs_nclib

VPATH += $(SRCDIR) $(OUTDIR) $(INCEXTDIR) $(INCINTDIR) inc $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all:	$(OUTDIR)/$(NCLIB_IRONSIDE_APPNAME) \
$(OUTDIR)/$(NCLIB_NCEDITCFG_APPNAME) \
		$(OUTDIR)/$(NCLIB_NCGET_APPNAME) \
		$(OUTDIR)/$(NCLIB_NCACTION_APPNAME) \
		

$(OUTDIR)/$(NCLIB_NCGET_APPNAME): $(NCLIB_NCGET_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(NCLIB_NCGET_CMDNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(NCGET_APEXE) $(NCLIB_NCGET_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(NCLIB_NCGET_APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(NCLIB_NCEDITCFG_APPNAME): $(NCLIB_NCEDITCFG_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(NCLIB_NCEDITCFG_CMDNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(NCEDITCFG_APEXE) $(NCLIB_NCEDITCFG_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(NCLIB_NCEDITCFG_APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(NCLIB_NCACTION_APPNAME): $(NCLIB_NCACTION_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(NCLIB_NCACTION_CMDNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(NCACTION_APEXE) $(NCLIB_NCACTION_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(NCLIB_NCACTION_APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(NCLIB_IRONSIDE_APPNAME): $(NCLIB_IRONSIDE_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(NCLIB_IRONSIDE_CMDNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(IRONSIDE_APEXE) $(NCLIB_IRONSIDE_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(NCLIB_IRONSIDE_APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

#$(OUTDIR)/$(APBM_HWICONF_APPNAME): APP_NAME := $(APBM_HWICONF_CMDNAME)
#$(OUTDIR)/$(APBM_HWICONF_APPNAME): OUTPUT_FILE := $(OUTDIR)/$(APBM_HWICONF_CMDNAME)
#$(OUTDIR)/$(APBM_HWICONF_APPNAME): OBJ_FILES := $(APBM_HWICONF_OBJS)
#$(OUTDIR)/$(APBM_HWICONF_APPNAME): $(APBM_HWICONF_OBJS)
#$(call build_application,$(APP_NAME),$(OUTPUT_FILE),$(OBJ_FILES))

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean

distclean: clean
	$(RM) -r $(OUTDIR)/$(NCLIB_NCGET_CMDNAME)
	$(RM) -r $(OUTDIR)/$(NCLIB_NCEDITCFG_CMDNAME)
	$(RM) -r $(OUTDIR)/$(NCLIB_NCACTION_CMDNAME)
	$(RM) -r $(OUTDIR)/$(NCLIB_IRONSIDE_CMDNAME)

####################
# $(call build_application,AppName,OutpuFile,ObjectFiles)
define build_application
$(NEW_LINE)
$(SEPARATOR_STR)
$(SILENT)$(ECHO) 'Building Application: $1'
$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $2 $3 $(LDFLAGS) $(LIBSDIR) $(LIBS)
$(SEPARATOR_STR)
$(NEW_LINE)
endef

