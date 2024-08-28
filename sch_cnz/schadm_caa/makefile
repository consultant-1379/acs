# **********************************************************************
#
# Short description:
# Makefile template for SCH service 
# **********************************************************************
#
# Ericsson AB 2017 All rights reserved.
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
#            2017-03-05   xsabjha   Created
# **********************************************************************

CNZ_NAME ?= sch_cnz
CAA_NAME ?= schadm_caa
CXC_NAME ?= sch_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk
                                  
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

NTACS = $(ACS_ROOT)
CXCDIR= $(NTACS)/sch_cnz/sch_cxc
CXCBIN= bin
OUTDIR= $(CXCDIR)/$(CXCBIN)
MODULE_NAME = SCH_ADM
MODULE_VERSION = 1_0
MODULE_SEARCH = ./$(SRCDIR) ./inc 
export DOXYGEN_OUTPUT
export MODULE_NAME
export MODULE_VERSION
export MODULE_SEARCH

# Setting application name
APP_FILENAME = acs_schd

APP_FILENAME_DBG = acs_schd_dbg

CAA_ADM_DIR = ../schadm_caa

CINCLUDES += -I inc -I inc/imm_mapper
SOURCES +=ACS_SCH_SRV.cpp ACS_SCH_HA_AppManager.cpp ACS_SCH_Server.cpp ACS_SCH_Event.cpp ACS_SCH_Logger.cpp ACS_SCH_CrmObserver.cpp ACS_SCH_IMMCallbackThread.cpp ACS_SCH_ComputeResourceRole.cpp ACS_SCH_Trace.cpp ACS_SCH_Registry.cpp ACS_SCH_Role.cpp ACS_SCH_CrmHandler.cpp ACS_SCH_ScalingCommandInterface.cpp ACS_SCH_ScalingCommandInvoker.cpp ACS_SCH_ScalingCommands.cpp ACS_SCH_ScalingCommandReceiver.cpp ACS_SCH_Util.cpp ACS_SCH_ClusterCPObserver.cpp

vpath %.cpp $(SRCDIR)

vpath %.h $(INCDIR)
vpath %.obj $(OBJDIR)

OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

DEPS := $(SOURCES:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)
        -include $(DEPS)
endif
## Object files
#MODULE_OBJS =	\
	$(OBJDIR)/ACS_SCH_SRV.obj \
	$(OBJDIR)/ACS_SCH_HA_AppManager.obj \
	$(OBJDIR)/ACS_SCH_Server.obj \
	$(OBJDIR)/ACS_SCH_Event.obj \
	$(OBJDIR)/ACS_SCH_Logger.obj \
  $(OBJDIR)/ACS_SCH_ModelHandler.obj \
  #$(OBJDIR)/ACS_SCH_ImUtil.obj \

#DEPS := $(MODULE_OBJS:.obj=.d)

#ifneq ($(MAKECMDGOALS),clean)
#include $(DEPS)
#endif

#Temporary to include latest CS library ##############################################
#CINCLUDES += -I"$(ACS_ROOT)/cs_cnz/csapi_caa/inc_ext"
#LIBSDIR += -L"$(ACS_ROOT)/cs_cnz/cs_cxc/bin/lib_ext"
#LIBSDIR += -L"$(ACS_ROOT)/apbm_cnz/apbm_cxc/bin/lib_ext"
#CINCLUDES += -I"$(ACS_ROOT)/apbm_cnz/apbmapi_caa/inc_ext"
######################################################################################

######################################################################################

#Temporary to include latest APGCC library ##############################################
######################################################################################

## here you can add own lib paths
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

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"inc"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(BOOST_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I"$(3PP_SDK_PATH)/libXerces/include"

# here you can add own libs
LIBS += -lrt -lpthread -lnetsnmp -lmcs_mml
LIBS += -lacs_tra -llog4cplus
LIBS += -lacs_apgcc -lssh2 -lxerces-c -lACE -lacs_aeh -lacs_prc \
-lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf \
-lboost_serialization -lboost_filesystem  -lboost_system -lsec_crypto_api -lsec_credu_api -lacs_csapi

VPATH += $(SRCDIR) $(OUTDIR) $(LIB_EXT) $(LIB_INT) $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(APP_FILENAME)

$(OUTDIR)/$(APP_FILENAME): $(OBJFILES)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building application: $(APP_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(OUTDIR)/$(APP_FILENAME) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APP_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(APP_FILENAME)
	

