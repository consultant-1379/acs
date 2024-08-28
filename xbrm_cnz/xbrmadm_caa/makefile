# **********************************************************************
#
# Short description:
# Makefile template for XBRM service 
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
NTACS = $(ACS_ROOT)
SOFTWARE_TYPE ?= app                                  
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf
DOCDIR = $(CXC_PATH)/doc
CXCBIN = bin
OUTDIR = $(CXC_PATH)/$(CXCBIN)
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
SECINCDIR = $(SEC_SDK_INC)/sec
BLOCK_NAME=XBRM_ADM_CAA
BLOCK_VERSION=1_1

BLOCK_SEARCH=./src ./inc
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

# Setting the application name
APP_FILENAME = acs_xbrmd
# APP_FILENAME_DBG = acs_xbrmd_dbg




DEPS := $(MODULE_OBJS:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
        -include $(DEPS)
endif

# trace enabled
EXTRA_DEFINES += -DACS_XBRM_API_HAS_TRACE
EXTRA_DEFINES += -DACS_XBRM_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DACS_XBRM_TRACE_HAS_FUNCTION_TRACE
EXTRA_DEFINES += -DACS_XBRM_TRACE_USE_PRETTY_FUNCTION
# log enabled
EXTRA_DEFINES += -DACS_XBRM_HAS_LOGGING
EXTRA_DEFINES += -DACS_XBRM_LOGGING_HAS_SOURCE_INFO
EXTRA_DEFINES += -DACS_XBRM_LOGGING_USE_PRETTY_FUNCTION

## here you can add own compiler flags
CFLAGS += -Wno-ignored-qualifiers -Wno-empty-body



SOURCES += 	ACS_XBRM_SRV.cpp \
			ACS_XBRM_Event.cpp \
			ACS_XBRM_Logger.cpp \
			ACS_XBRM_Tracer.cpp \
			ACS_XBRM_Utilities.cpp \
			ACS_XBRM_Server.cpp \
			ACS_XBRM_HA_AppManager.cpp \
			ACS_XBRM_BrmHandler.cpp \
			ACS_XBRM_ScheduleHandler.cpp \
			ACS_XBRM_PeriodicEventHandler.cpp \
			ACS_XBRM_SystemBackupHandler.cpp \
			ACS_XBRM_ApBackupHandler.cpp \
			ACS_XBRM_MMLEventHandler.cpp \
			ACS_XBRM_MMLMessageHandler.cpp \
			ACS_XBRM_EventScheduler.cpp \
			ACS_XBRM_AlarmHandler.cpp

# ACS_XBRM_BrmHandler.cpp \
# ACS_XBRM_CpBackupGenerator.cpp \		
#ACS_XBRM_SRV.cpp ACS_XBRM_Server.cpp ACS_XBRM_Event.cpp ACS_XBRM_HA_AppManager.cpp 

vpath %.cpp $(SRCDIR)

vpath %.h $(INCDIR)
vpath %.obj $(OBJDIR)

OBJ = $(SOURCES:.cpp=.obj)
OBJFILES = $(addprefix $(OBJDIR)/,$(OBJ))

DEPS := $(SOURCES:.cpp=.d)
ifneq ($(MAKECMDGOALS),clean)
        -include $(DEPS)
endif

ifeq ($(SOFTWARE_TYPE),so)
LDFLAGS += -shared -W1,-soname=$(LIB_SONAME)
OUT_FILENAME = $(LIB_FILENAME)
else
OUT_FILENAME = $(APP_FILENAME)
endif

# Object files
MODULE_OBJS =	\
	$(OBJDIR)/ACS_XBRM_BackupManagerHandler.obj \
	$(OBJDIR)/ACS_XBRM_Event.obj \
	$(OBJDIR)/ACS_XBRM_PeriodicEventHandler.obj \
	$(OBJDIR)/ACS_XBRM_ScheduleHandler.obj \
	$(OBJDIR)/ACS_XBRM_Server.obj \
	$(OBJDIR)/ACS_XBRM_Logger.obj \
	$(OBJDIR)/ACS_XBRM_Tracer.obj \
	$(OBJDIR)/ACS_XBRM_SRV.obj \
	$(OBJDIR)/ACS_XBRM_ApBackupHandler.obj \
	$(OBJDIR)/ACS_XBRM_SystemBackupHandler.obj \
	$(OBJDIR)/ACS_XBRM_MMLEventHandler.obj \
	$(OBJDIR)/ACS_XBRM_MMLMessageHandler.obj \
	$(OBJDIR)/ACS_XBRM_Utilities.obj \
	$(OBJDIR)/ACS_XBRM_EventScheduler.obj \
	$(OBJDIR)/ACS_XBRM_AlarmHandler.obj

# here you can add own Include paths and/or other includes
CINCLUDES += -I"$(INCDIR)"

## # here you can add own Include paths and/or other INCDIR includes
CINCLUDES += -I"$(INCINTDIR)"
CINCLUDES += -I"$(INCEXTDIR)"
# CINCLUDES += -I"/home/zpagsai/git_workspace/backupExport/mml_event/mcs/adh_cnz/adhapi_caa/inc_int"

## here you can add own lib paths
LIBSDIR += -L"$(LIB_EXT)"
LIBSDIR += -L"$(AP_SDK_LIB)"
# LIBSDIR += -L"$(3PP_SDK_PATH)/log4cplus/4_0/log4cplus/lib"
# LIBSDIR += -L"$(3PP_SDK_PATH)/libXerces/3_1/lib64"
# LIBSDIR += -L"$(3PP_SDK_PATH)/libssh2/1_0/lib64"
LIBSDIR += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) #-L$(LIB_XERCES_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) 

# here you can add own linker flags
LDFLAGS +=
# Compilation Options
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
# LDFLAGS += -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB)
# LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)

# here you can add more defines
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"inc"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I"$(BOOST_SDK_INC)"
#CINCLUDES += -I"$(3PP_SDK_PATH)/libXerces/include"
CINCLUDES += -I"$(SEC_SDK_INC)"
CINCLUDES += -I"$(SEC_CREDU_SDK_INC)"
CINCLUDES += -I"$(SECINCDIR)"

# here you can add own libs
#AP SDK
LIBSDIR += -L$(AP_SDK_LIB)
LIBS += -lacs_csapi  -lacs_apgcc -lacs_aeh -lacs_tra -laes_ohi #-lcap -lacs_dsd -lutil-logger  -llde-logger -lacs_prc
LIBS += -lrt -lpthread  -lmcs_mml -laes_gcc -lboost_serialization -lboost_filesystem#-lssh2  -lxerces-c -llog4cplus -lnetsnmp -lboost_serialization

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

#BOOST SDK
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBS += -lboost_filesystem -lboost_system
#SEC API LIBS
LIBS += -lsec_crypto_api -lsec_credu_api

.phony: all clean distclean


XBRM_APNAME = acs_xbrmd  
XBRM_APNAME_DBG = acs_xbrmd_dbg

XBRM_APEXE = $(OUTDIR)/$(XBRM_APNAME)


VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

CFLAGS +=  -std=c++11

.PHONY: all CFLAGS += $(GCOV_FLAGS) 
all: $(OUTDIR)/$(XBRM_APNAME)

$(OUTDIR)/$(XBRM_APNAME): $(OBJFILES)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building application: $(APP_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(XBRM_APEXE) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APP_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(XBRM_APNAME)
	
# .PHONY: copy_to_lotc
# copy_to_lotc:
# 	$(SILENT)$(ECHO) 'Copyng application $(OUTDIR)/$(OUT_FILENAME) onto the lotc...'
# 	$(SILENT) sftp -b copy-to-simlotc.batch $(LOTC_USER) >&/dev/null
.PHONY: documentation
documentation:
	$(SILENT)$(ECHO) 'documentation updating ...'
	$(SILENT)$(ECHO) ' Removing $(DOXYGEN_OUTPUT) directory'
	$(SILENT)$(RM) -r -f $(DOXYGEN_OUTPUT)
	$(SILENT)$(ECHO) ' Creating $(DOXYGEN_OUTPUT) directory'
	$(SILENT)mkdir $(DOXYGEN_OUTPUT)
	$(SILENT)$(DOXYGEN) -g $(DOXYGENFILECONFIG)

.PHONY: metrics
metrics:
	$(SILENT)$(ECHO) 'Metrics updating ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'


