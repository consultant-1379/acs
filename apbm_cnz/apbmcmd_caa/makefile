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
CAA_NAME ?= apbmcmd_caa
CXC_NAME ?= apbm_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

MODULE_NAME = APBM_CMD
MODULE_VERSION = 1_0
MODULE_SEARCH = ./$(SRCDIR) ./inc
export DOXYGEN_OUTPUT
export MODULE_NAME
export MODULE_VERSION
export MODULE_SEARCH


CAA_API_DIR = ../apbmapi_caa
CAA_PIPAPI_DIR = ../apbmpipapi_caa
CAA_ADM_DIR = ../apbmadm_caa
export CAA_API_DIR


## Object files
#APBM_HWICONF_OBJS = $(OBJDIR)/hwiconf.obj

HWIPRINT_FILES = ../apbmadm_caa/obj
HWIPRINT_MOREHEADERFILES = ../apbmadm_caa/inc

APBM_HWIPRINT_OBJS = \
	$(OBJDIR)/hwiprint.obj \
	$(HWIPRINT_FILES)/acs_apbm_hwidata.obj \
	$(OBJDIR)/acs_apbm_hwixmlmaker.obj \
	$(OBJDIR)/acs_apbm_hwixstr.obj

APBM_HWMSCBLS_SET_OBJS = \
	$(OBJDIR)/hwmscbls.obj \
	$(OBJDIR)/hwmcommon.obj
	
APBM_HWMXLS_SET_OBJS = \
	$(OBJDIR)/hwmxls.obj \
	$(OBJDIR)/hwmcommon.obj

APBM_DSHELF_SET_OBJS = \
	$(OBJDIR)/dShelfMngrLog.obj \
	
APBM_BIOS_SET_OBJS = \
	$(OBJDIR)/bios_set_cmd.obj \
	$(OBJDIR)/bios_set_handler.obj \
	$(OBJDIR)/bios_set_snmpmanager.obj \
	$(OBJDIR)/bios_set_common.obj\
 	$(OBJDIR)/acs_apbm_cmd_ironsidemanager.obj\

## Dependencies
#APBM_HWICONF_DEPS := $(APBM_HWICONF_OBJS:.obj=.d)

APBM_HWIPRINT_DEPS := $(APBM_HWIPRINT_OBJS:.obj=.d)

APBM_BIOS_SET_DEPS := $(APBM_BIOS_SET_OBJS:.obj=.d)

APBM_HWMSCBLS_SET_DEPS := $(APBM_HWMSCBLS_SET_OBJS:.obj=.d)

APBM_HWMXLS_SET_DEPS := $(APBM_HWMXLS_SET_OBJS:.obj=.d)

APBM_DSHELF_SET_DEPS := $(APBM_DSHELF_SET_OBJS:.obj=.d)


ifneq ($(MAKECMDGOALS),clean)
#include $(APBM_HWICONF_DEPS)
include $(APBM_HWIPRINT_DEPS)
include $(APBM_BIOS_SET_DEPS)
include $(APBM_HWMSCBLS_SET_DEPS)
include $(APBM_HWMXLS_SET_DEPS)
include $(APBM_DSHELF_SET_DEPS)

endif

## Application Command Names
#APBM_HWICONF_APPNAME := hwiconf
APBM_HWIPRINT_APPNAME := hwiprint
APBM_BIOS_SET_APPNAME := bios_set
APBM_HWMSCBLS_APPNAME := hwmscbls
APBM_HWMXLS_APPNAME := hwmxls
APBM_DSHELF_APPNAME := dShelfMngrLog

#APBM_HWICONF_CMDNAME := $(APBM_HWICONF_APPNAME)
APBM_HWIPRINT_CMDNAME := $(APBM_HWIPRINT_APPNAME)
APBM_BIOS_SET_CMDNAME := $(APBM_BIOS_SET_APPNAME)
APBM_HWMSCBLS_CMDNAME := $(APBM_HWMSCBLS_APPNAME)
APBM_HWMXLS_CMDNAME := $(APBM_HWMXLS_APPNAME)
APBM_DSHELF_CMDNAME := $(APBM_DSHELF_APPNAME)

APBM_HWIPRINT_CMDNAME_DBG := $(APBM_HWIPRINT_APPNAME)_dbg
APBM_BIOS_SET_CMDNAME_DBG := $(APBM_BIOS_SET_APPNAME)_dbg
APBM_HWMSCBLS_CMDNAME_DBG := $(APBM_HWMSCBLS_APPNAME)_dbg
APBM_HWMXLS_CMDNAME_DBG := $(APBM_HWMXLS_APPNAME)_dbg
APBM_DSHELF_CMDNAME_DBG := $(APBM_DSHELF_APPNAME)_dbg

#Temporary to include latest CS library ##############################################
#CINCLUDES += -I"$(ACS_ROOT)/cs_cnz/csapi_caa/inc_ext"
#LIBSDIR += -L"$(ACS_ROOT)/cs_cnz/cs_cxc/bin/lib_ext"
######################################################################################


## here you can add own lib paths
LIBSDIR += -L"$(CXCPATH)/bin/lib_ext"
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(3PP_SDK_PATH)/log4cplus/4_0/log4cplus/lib"
LIBSDIR += -L"$(3PP_SDK_PATH)/libXerces/3_1/lib64"
LIBSDIR += -L"$(3PP_SDK_PATH)/libssh2/1_0/lib64"
LIBSDIR += -L"$(ACE_SDK_LIB)"
LIBSDIR += -L"$(LIB_XERCES_SDK_LIB)"
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB) -L$(SEC_SDK_LIB) -L$(BOOST_SDK_LIB)


## here you can add own compiler flags
CFLAGS += -Wno-empty-body

# here you can add own linker flags
LDFLAGS +=

EXTRA_DEFINES += -DBS_DEBUG 
EXTRA_DEFINES += -g -DACS_APBM_HAS_TRACE -DACS_APBM_TRACE_HAS_FUNCTION_TRACE -DACS_APBM_TRACE_USE_PRETTY_FUNCTION -DACS_APBM_TRACE_USE_FILE_INFO

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"inc"
CINCLUDES += -I"$(CAA_API_DIR)/$(INCEXTDIR)"
CINCLUDES += -I"$(CAA_API_DIR)/$(INCINTDIR)"
CINCLUDES += -I"$(CAA_PIPAPI_DIR)/$(INCEXTDIR)"
CINCLUDES += -I"$(CAA_PIPAPI_DIR)/$(INCINTDIR)"
CINCLUDES += -I"$(CAA_ADM_DIR)/inc"
CINCLUDES += -I"$(AP_SDK_INC)" 
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I"$(3PP_SDK_PATH)/libXerces/include"
CINCLUDES += -I"$(HWIPRINT_MOREHEADERFILES)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I$(LIB_XERCES_SDK_INC)

HWIPRINT_APEXE = $(OUTDIR)/$(APBM_HWIPRINT_CMDNAME)
BIOS_SET_APEXE = $(OUTDIR)/$(APBM_BIOS_SET_CMDNAME)
HWMSCBLS_APEXE = $(OUTDIR)/$(APBM_HWMSCBLS_CMDNAME)
HWMXLS_APEXE = $(OUTDIR)/$(APBM_HWMXLS_CMDNAME)
DSHELF_APEXE = $(OUTDIR)/$(APBM_DSHELF_CMDNAME)

# here you can add own libs
#LIBS += -lrt -lpthread -lacs_tra -llog4cplus -lnetsnmp -lacs_csapi -lacs_apgcc -lxerces-c -lxerces-depdom -lacs_dsd -lacs_apbm -lacs_apbmpip -lacs_nclib -lssh2
LIBS += -lrt -lpthread -lacs_tra -llog4cplus -lnetsnmp -lacs_csapi -lacs_apgcc -lxerces-c -lacs_dsd -lacs_apbm -lacs_apbmpip -lacs_nclib -lssh2 -lacs_tra -lacs_aeh -lacs_csapi -lACE  -lboost_serialization -lboost_filesystem  -lboost_system -lacs_prc
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf -lsec_crypto_api -lsec_credu_api

#LIBS += $(ACS_ROOT)/apbm_cxc/bin/lib_ext/libacs_apbm.so.3.1.6

VPATH += $(SRCDIR) $(OUTDIR) $(INCEXTDIR) $(INCINTDIR) inc $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all:	$(OUTDIR)/$(APBM_DSHELF_APPNAME)\
	$(OUTDIR)/$(APBM_BIOS_SET_APPNAME) \
		$(OUTDIR)/$(APBM_HWMSCBLS_APPNAME) \
		$(OUTDIR)/$(APBM_HWIPRINT_APPNAME) \
		$(OUTDIR)/$(APBM_HWMXLS_APPNAME) \
		$(OUTDIR)/$(APBM_DSHELF_APPNAME) 

#		$(OUTDIR)/$(APBM_HWICONF_APPNAME)

$(OUTDIR)/$(APBM_HWIPRINT_APPNAME): $(APBM_HWIPRINT_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(APBM_HWIPRINT_CMDNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(HWIPRINT_APEXE) $(APBM_HWIPRINT_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APBM_HWIPRINT_APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(APBM_BIOS_SET_APPNAME): $(APBM_BIOS_SET_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(ECHO) 'Creating Application: $(APBM_BIOS_SET_CMDNAME)'
	$(CC) $(CFLAGS) -o $(BIOS_SET_APEXE) $(APBM_BIOS_SET_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APBM_BIOS_SET_APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(APBM_HWMSCBLS_APPNAME): $(APBM_HWMSCBLS_SET_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(APBM_HWMSCBLS_CMDNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(HWMSCBLS_APEXE) $(APBM_HWMSCBLS_SET_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APBM_HWMSCBLS_APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(APBM_HWMXLS_APPNAME): $(APBM_HWMXLS_SET_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(APBM_HWMXLS_CMDNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(HWMXLS_APEXE) $(APBM_HWMXLS_SET_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APBM_HWMXLS_APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)
$(OUTDIR)/$(APBM_DSHELF_APPNAME): $(APBM_DSHELF_SET_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(APBM_DSHELF_CMDNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(DSHELF_APEXE) $(APBM_DSHELF_SET_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APBM_DSHELF_APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean

distclean: clean
	$(RM) -r $(OUTDIR)/$(APBM_HWIPRINT_CMDNAME)
	$(RM) -r $(OUTDIR)/$(APBM_BIOS_SET_CMDNAME)
	$(RM) -r $(OUTDIR)/$(APBM_HWMSCBLS_CMDNAME)
	$(RM) -r $(OUTDIR)/$(APBM_HWMXLS_CMDNAME)
	$(RM) -r $(OUTDIR)/$(APBM_DSHELF_CMDNAME)
#	$(RM) -r $(OUTDIR)/$(APBM_HWICONF_CMDNAME)

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
