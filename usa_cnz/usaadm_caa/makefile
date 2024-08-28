# **********************************************************************
#
# Short description:
# Makefile template for XXX library
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
#            2010-12-17   XMALSHA   Created
#
#***********************************************************************

CNZ_NAME ?= usa_cnz
CAA_NAME ?= usaadm_caa
CXC_NAME ?= usa_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

DOCDIR = $(CXC_PATH)/doc
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=ACS_USA
BLOCK_VERSION=1_0
BLOCK_SEARCH=./src ./inc

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

USA_SRCDIR = $(CAA_PATH)/src
USA_INCDIR = $(CAA_PATH)/inc
CXCDIR	= $(CXC_PATH)

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

##Include files
CINCLUDES += -I$(USA_INCDIR)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(BOOST_SDK_INC)

## Source files
USA_SRC =	$(USA_SRCDIR)/acs_usa_main.cpp \
		$(USA_SRCDIR)/acs_usa_aprebootmonitor.cpp\
		$(USA_SRCDIR)/acs_usa_autoCeaseManager.cpp\
		$(USA_SRCDIR)/acs_usa_initialiser.cpp\
		$(USA_SRCDIR)/acs_usa_time.cpp\
		$(USA_SRCDIR)/acs_usa_analyser.cpp\
		$(USA_SRCDIR)/acs_usa_control.cpp\
		$(USA_SRCDIR)/acs_usa_tokenizer.cpp\
		$(USA_SRCDIR)/acs_usa_config.cpp\
		$(USA_SRCDIR)/acs_usa_analysisObject.cpp\
		$(USA_SRCDIR)/acs_usa_criterion.cpp \
		$(USA_SRCDIR)/acs_usa_adm.cpp \
		$(USA_SRCDIR)/acs_usa_hamanager.cpp \
		$(USA_SRCDIR)/acs_usa_global.cpp \
		$(USA_SRCDIR)/acs_usa_tratrace.cpp \
		$(USA_SRCDIR)/acs_usa_reactorrunner.cpp \
		$(USA_SRCDIR)/acs_usa_zombiemonitor.cpp \
		$(USA_SRCDIR)/acs_usa_cpuldavg.cpp \
		$(USA_SRCDIR)/acs_usa_sysloganalyser.cpp \
		$(USA_SRCDIR)/acs_usa_ntpSrvM.cpp \
		$(USA_SRCDIR)/acs_usa_regexp.cpp \
		$(USA_SRCDIR)/acs_usa_atlRegexp.cpp \
		$(USA_SRCDIR)/acs_usa_atlRegexpImpl.cpp \
		$(USA_SRCDIR)/acs_usa_error.cpp \
		$(USA_SRCDIR)/acs_usa_errorimpl.cpp \
		$(USA_SRCDIR)/acs_usa_file.cpp \
		$(USA_SRCDIR)/acs_usa_event.cpp \
		$(USA_SRCDIR)/acs_usa_fileVersioning.cpp \
		$(USA_SRCDIR)/acs_usa_eventManager.cpp \
		$(USA_SRCDIR)/acs_usa_common.cpp \
		$(USA_SRCDIR)/acs_usa_eventInfo.cpp \
		$(USA_SRCDIR)/acs_usa_logtrace.cpp  \
		$(USA_SRCDIR)/acs_usa_ntfNotificationThread.cpp \
		$(USA_SRCDIR)/acs_usa_adhIntUserObserver.cpp

USA_OBJ =	$(OBJDIR)/acs_usa_main.obj \
		$(OBJDIR)/acs_usa_aprebootmonitor.obj\
		$(OBJDIR)/acs_usa_autoCeaseManager.obj\
		$(OBJDIR)/acs_usa_initialiser.obj\
		$(OBJDIR)/acs_usa_time.obj\
		$(OBJDIR)/acs_usa_analyser.obj\
		$(OBJDIR)/acs_usa_control.obj\
		$(OBJDIR)/acs_usa_tokenizer.obj\
		$(OBJDIR)/acs_usa_config.obj\
		$(OBJDIR)/acs_usa_analysisObject.obj\
		$(OBJDIR)/acs_usa_criterion.obj \
		$(OBJDIR)/acs_usa_adm.obj \
		$(OBJDIR)/acs_usa_hamanager.obj\
		$(OBJDIR)/acs_usa_global.obj \
		$(OBJDIR)/acs_usa_tratrace.obj \
		$(OBJDIR)/acs_usa_reactorrunner.obj \
		$(OBJDIR)/acs_usa_zombiemonitor.obj \
		$(OBJDIR)/acs_usa_cpuldavg.obj \
		$(OBJDIR)/acs_usa_sysloganalyser.obj \
		$(OBJDIR)/acs_usa_ntpSrvM.obj \
		$(OBJDIR)/acs_usa_regexp.obj \
		$(OBJDIR)/acs_usa_atlRegexp.obj \
		$(OBJDIR)/acs_usa_atlRegexpImpl.obj \
		$(OBJDIR)/acs_usa_error.obj \
		$(OBJDIR)/acs_usa_errorimpl.obj \
		$(OBJDIR)/acs_usa_file.obj \
		$(OBJDIR)/acs_usa_event.obj \
		$(OBJDIR)/acs_usa_fileVersioning.obj \
		$(OBJDIR)/acs_usa_eventManager.obj\
		$(OBJDIR)/acs_usa_common.obj \
		$(OBJDIR)/acs_usa_eventInfo.obj \
		$(OBJDIR)/acs_usa_logtrace.obj	\
		$(OBJDIR)/acs_usa_ntfNotificationThread.obj \
		$(OBJDIR)/acs_usa_adhIntUserObserver.obj

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

COVERAGE ?= no

# here you can add own compiler flags
ifeq ($(COVERAGE),yes)
CFLAGS += -O0 -fprofile-arcs -ftest-coverage
else
CFLAGS += -O3
endif

# here you can add own lib paths
LIBSDIR += -L$(AP_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB)

## # here you can add own libs
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf -lsec_crypto_api -lsec_credu_api -lrt -lpthread -lACE -lacs_tra -lacs_apgcc -lacs_aeh -lacs_dsd -lboost_filesystem -lboost_serialization -lboost_system -lacs_csapi -lacs_prc -llog4cplus
## Object filebin
OBJ =   $(USA_SRC:%.cpp=%.obj) \

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) $(OBJDIR)

USA_APNAME = acs_usad
USA_EXE = $(OUTDIR)/$(USA_APNAME)

.PHONY: all CFLAGS += $(GCOV_FLAGS)

all:	$(OUTDIR)/$(USA_APNAME)

$(USA_EXE):$(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(USA_APNAME)'
	$(CC) $(CFLAGS) -o $(USA_EXE) $(USA_OBJ) $(LIBSDIR) $(LIBS)
	$(call stripp,$(USA_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: cccc
cccc:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*)
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*)
	$(SEPARATOR_STR)

.PHONY: clean
clean:
	$(RM) $(OBJDIR)/*.obj
	$(RM) $(OBJDIR)/*.d
	$(RM) $(OUTDIR)/$(USA_APNAME)
	$(RM) $(DEBUGDIR)/$(USA_APNAME)_dbg

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


