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

CNZ_NAME ?= asec_cnz
CAA_NAME ?= asecadm_caa
CXC_NAME ?= asec_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

DOCDIR = $(CXC_PATH)/doc
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=ASEC_ADM_CAA
BLOCK_VERSION=1_0
BLOCK_SEARCH=./src ./inc_int
DEBUGDIR=$(OUTDIR)/.debug

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

SRCDIR = $(CAA_PATH)/src
INCDIR = $(CAA_PATH)/inc_int

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

##Include files
CINCLUDES += -I$(INCDIR) -I$(AP_SDK_INC) -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC) -I$(SEC_CREDU_SDK_INC) -I$(BOOST_SDK_INC) -I$(LIB_LOGCPLUS_SDK_INC) -I$(SEC_SDK_INC)


LIBSDIR += -L$(AP_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(SEC_SDK_LIB)

LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += $(COREMW_LIBS)

# Compilation Options
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(COREMW_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LOTC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)

## Source files
ASEC_SRC = $(SRCDIR)/acs_asec_main.cpp \
	   $(SRCDIR)/acs_asec_adm.cpp \
	   $(SRCDIR)/acs_asec_hamanager.cpp \
	   $(SRCDIR)/acs_asec_global.cpp \
	   $(SRCDIR)/acs_asec_reactorrunner.cpp \
	   $(SRCDIR)/acs_asec_dsdServer.cpp\
	   $(SRCDIR)/acs_asec_RuntimeHandler.cpp \
	   $(SRCDIR)/acs_asec_trace.cpp \
	   $(SRCDIR)/acs_asec_immhandler.cpp \
	   $(SRCDIR)/acs_asec_oi_ipsecConnection.cpp \
	   $(SRCDIR)/acs_asec_ObjectImplementer.cpp \
           $(SRCDIR)/acs_asec_oi_gsnhConnection.cpp 
	

ASEC_OBJ = $(OBJDIR)/acs_asec_main.obj\
	   $(OBJDIR)/acs_asec_adm.obj \
	   $(OBJDIR)/acs_asec_hamanager.obj \
	   $(OBJDIR)/acs_asec_global.obj \
	   $(OBJDIR)/acs_asec_reactorrunner.obj \
	   $(OBJDIR)/acs_asec_dsdServer.obj \
	   $(OBJDIR)/acs_asec_RuntimeHandler.obj\
	   $(OBJDIR)/acs_asec_trace.obj\
	   $(OBJDIR)/acs_asec_immhandler.obj\
	   $(OBJDIR)/acs_asec_oi_ipsecConnection.obj\
	   $(OBJDIR)/acs_asec_ObjectImplementer.obj \
           $(OBJDIR)/acs_asec_oi_gsnhConnection.obj
		
VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

COVERAGE ?= no

# here you can add own compiler flags
ifeq ($(COVERAGE),yes)
CFLAGS += -O0 -fprofile-arcs -ftest-coverage
else
CFLAGS += -O3
endif

# here you can add own lib paths

## # here you can add own lib
LIBS += -lrt -lpthread -lacs_apgcc -lacs_dsd -lacs_csapi -lacs_prc -lacs_tra -lACE -lacs_aeh -lsec_credu_api -lboost_serialization -lboost_filesystem -lboost_system -lboost_thread -llog4cplus
LIBS += -lsec_crypto_api

## Object filebin
OBJ =   $(ASEC_SRC:%.cpp=%.obj) \
	$(ASEC_SRC:%.c=%.obj) \

OUTDIR = $(CXC_PATH)/bin

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) $(OBJDIR)

ASEC_APNAME = acs_asec_csadmd

.PHONY: all CFLAGS += $(GCOV_FLAGS)

all:	$(OUTDIR)/$(ASEC_APNAME)

$(OUTDIR)/$(ASEC_APNAME):$(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(ASEC_APNAME)'
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(ASEC_APNAME) $(ASEC_OBJ) $(LIBSDIR) $(LIBS)
	$(call stripp,$(ASEC_APNAME))
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
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d


