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
CAA_NAME ?= apbmadm_caa
CXC_NAME ?= apbm_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

MODULE_NAME = APBM_ADM
MODULE_VERSION = 1_0
MODULE_SEARCH = ./$(SRCDIR) ./$(SRCDIR)/ironside ./inc ./inc/ironside
export DOXYGEN_OUTPUT
export MODULE_NAME
export MODULE_VERSION
export MODULE_SEARCH

# Setting application name
APP_FILENAME = acs_apbmd

APP_FILENAME_DBG = acs_apbmd_dbg

CAA_API_DIR = ../apbmapi_caa
CAA_PIPAPI_DIR = ../apbmpipapi_caa
CAA_ADM_DIR = ../apbmadm_caa

## Object files
MODULE_OBJS =	\
	$(OBJDIR)/acs_apbm_action_timer.obj \
	$(OBJDIR)/acs_apbm_alarmeventhandler.obj \
	$(OBJDIR)/acs_apbm_boardserviceacceptor.obj \
	$(OBJDIR)/acs_apbm_boardsessionhandler.obj \
	$(OBJDIR)/acs_apbm_cmdoptionparser.obj \
	$(OBJDIR)/acs_apbm_cshwctablechangeobserver.obj \
	$(OBJDIR)/acs_apbm_csreader.obj \
	$(OBJDIR)/acs_apbm_haappmanager.obj \
	$(OBJDIR)/acs_apbm_hwidata.obj \
	$(OBJDIR)/acs_apbm_hwihandler.obj \
	$(OBJDIR)/acs_apbm_hwiimmconfobjhandler.obj \
	$(OBJDIR)/acs_apbm_hwiimmruntimeobjhandler.obj \
	$(OBJDIR)/acs_apbm_immrepositoryhandler.obj \
	$(OBJDIR)/acs_apbm_ipmiupgradehandler.obj \
	$(OBJDIR)/acs_apbm_logger.obj \
	$(OBJDIR)/acs_apbm_main.obj \
	$(OBJDIR)/acs_apbm_monitoringservicehandler.obj \
	$(OBJDIR)/acs_apbm_operationpipeeventhandler.obj \
	$(OBJDIR)/acs_apbm_operationpipescheduler.obj \
	$(OBJDIR)/acs_apbm_procsignalseventhandler.obj \
	$(OBJDIR)/acs_apbm_programconfiguration.obj \
	$(OBJDIR)/acs_apbm_sanotifiereventhandler.obj \
	$(OBJDIR)/acs_apbm_sanotifiermanager.obj \
	$(OBJDIR)/acs_apbm_serverwork.obj \
	$(OBJDIR)/acs_apbm_serverworkingset.obj \
	$(OBJDIR)/acs_apbm_shelvesdatamanager.obj \
	$(OBJDIR)/acs_apbm_snmpmanager.obj \
	$(OBJDIR)/acs_apbm_snmpmanagercallbacks.obj \
	$(OBJDIR)/acs_apbm_snmpsessioncallback.obj \
	$(OBJDIR)/acs_apbm_snmpsessionhandler.obj \
	$(OBJDIR)/acs_apbm_snmpsessioninfo.obj \
	$(OBJDIR)/acs_apbm_snmptrapdsobserver.obj \
	$(OBJDIR)/acs_apbm_snmptrapmessagehandler.obj \
	$(OBJDIR)/acs_apbm_trapsubscriptionmanager.obj \
	$(OBJDIR)/acs_apbm_arpping_dmx.obj \
	$(OBJDIR)/acs_apbm_scxlagimmhandler.obj \
	$(OBJDIR)/acs_apbm_ironsidemanager.obj \
	$(OBJDIR)/ironside/acs_apbm_commandbuilder.obj \
	$(OBJDIR)/ironside/acs_apbm_remotesender.obj \
	$(OBJDIR)/ironside/acs_apbm_ironsidecommand.obj \
	../apbmapi_caa/$(OBJDIR)/acs_apbm_api_imp.obj \
        ../apbmapi_caa/$(OBJDIR)/acs_apbm_api.obj  

DEPS := $(MODULE_OBJS:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

#Temporary to include latest CS library ##############################################
CINCLUDES += -I"$(ACS_ROOT)/cs_cnz/csapi_caa/inc_ext"
LIBSDIR += -L"$(ACS_ROOT)/cs_cnz/cs_cxc/bin/lib_ext"
LIBSDIR += -L"$(ACS_ROOT)/apbm_cnz/apbm_cxc/bin/lib_ext"
CINCLUDES += -I"$(ACS_ROOT)/apbm_cnz/apbmapi_caa/inc_ext"
######################################################################################

#Temporary to include latest NETCONF library ##############################################
#CINCLUDES += -I"$(ACS_ROOT)/nclib_cnz/nclibapi_caa/inc_ext"
#LIBSDIR += -L"$(ACS_ROOT)/nclib_cnz/nclib_cxc/bin/lib_ext"
#CINCLUDES += -I"$(ACS_ROOT)/apbm_cnz/apbmapi_caa/inc_ext"
#CINCLUDES += -I"$(ACS_ROOT)/apbm_cnz/apbmapi_caa/inc_int"
#CINCLUDES += -I"$(ACS_ROOT)/apbm_cnz/apbmadm_caa/inc"

######################################################################################

#Temporary to include latest APGCC library ##############################################
######################################################################################

## here you can add own lib paths
# Temporary for NCLIB
#LIBSDIR += -L"$(ACS_ROOT)/nclib_cnz/nclib_cxc/bin/lib_ext"
LIBSDIR += -L"$(CXC_PATH)/bin/lib_ext"
LIBSDIR += -L"$(LIB_EXT)"
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(3PP_SDK_PATH)/log4cplus/4_0/log4cplus/lib"
LIBSDIR += -L"$(3PP_SDK_PATH)/libXerces/3_1/lib64"
LIBSDIR += -L"$(3PP_SDK_PATH)/libssh2/1_0/lib64"
LIBSDIR += -L$(COREMW_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(ACE_SDK_LIB)  #-L$(LIB_XERCES_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) 

## here you can add own compiler flags
CFLAGS += -Wno-ignored-qualifiers -Wno-empty-body

# here you can add own linker flags
LDFLAGS +=

EXTRA_DEFINES += -DACS_APBM_LOGGING_ENABLED
EXTRA_DEFINES += -DACS_APBM_LOGGING_HAS_SOURCE_INFO -DACS_APBM_LOGGING_USE_PRETTY_FUNCTION
EXTRA_DEFINES += -DACS_APBM_USE_IMM_CONFIGURATION_OBJECTS
EXTRA_DEFINES += -DACS_ACA_HAS_DISK_REINTEGRATION_WORKAROUND
#EXTRA_DEFINES += -DACS_APBM_USE_IMM_CONFIGURATION_PRODUCT_OBJECTS
#EXTRA_DEFINES += -DACS_APBM_USE_IMM_CONFIGURATION_LED_OBJECTS
#EXTRA_DEFINES += -DACS_APBM_USE_IMM_CONFIGURATION_NIC_OBJECTS

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
#CINCLUDES += -I"$(ACS_ROOT)/nclib_cnz/nclibapi_caa/inc_ext"
CINCLUDES += -I"inc"
CINCLUDES += -I"inc/ironside"
CINCLUDES += -I"$(CAA_API_DIR)/$(INCEXTDIR)"
CINCLUDES += -I"$(CAA_API_DIR)/$(INCINTDIR)"
CINCLUDES += -I"$(CAA_PIPAPI_DIR)/$(INCEXTDIR)"
CINCLUDES += -I"$(CAA_PIPAPI_DIR)/$(INCINTDIR)"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(BOOST_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I"$(3PP_SDK_PATH)/libXerces/include"




# here you can add own libs
LIBS += -lrt -lpthread -lnetsnmp
LIBS += -lacs_tra -llog4cplus
LIBS += -lacs_apbmpip -lacs_apbm 
LIBS += -lacs_dsd -lacs_csapi -lacs_apgcc -lacs_trapdsapi -lacs_nclib -lssh2 -lxerces-c -lACE -lacs_aeh -lacs_prc \
-lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf \
-lboost_serialization -lboost_filesystem  -lboost_system -lsec_crypto_api -lsec_credu_api

VPATH += $(SRCDIR) $(SRCDIR)/ironside $(OUTDIR) $(LIB_EXT) $(LIB_INT) $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(APP_FILENAME)

$(OUTDIR)/$(APP_FILENAME): $(MODULE_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building application: $(APP_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(OUTDIR)/$(APP_FILENAME) $(MODULE_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APP_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/ironside/*.obj
	$(RM) -r $(OBJDIR)/*.d
	$(RM) -r $(OBJDIR)/ironside/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(APP_FILENAME)
	
