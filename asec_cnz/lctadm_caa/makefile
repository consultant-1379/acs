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
CAA_NAME ?= lctadm_caa
CXC_NAME ?= lct_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

DOCDIR = $(ACS_ROOT)/doc
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=LCT_ADM_CAA
BLOCK_VERSION=1_0
BLOCK_SEARCH=./src ./inc_int
DEBUGDIR=$(OUTDIR)/.debug

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH
export DEBUGDIR

SRCDIR = $(CAA_PATH)/src
INCDIR = $(CAA_PATH)/inc_int

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

##Include files
CINCLUDES += -I$(INCDIR)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

## Include your libs here
LIBSDIR += -L$(INCDIR)
LIBSDIR += -L$(AP_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB)
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB)

## Source files
HARDENING_SRC =$(SRCDIR)/acs_lct_command_handler_tsusers.cpp \
	  $(SRCDIR)/acs_lct_command_handler_hardening.cpp\
	  $(SRCDIR)/acs_lct_command_handler_securityhandling.cpp\
	  $(SRCDIR)/acs_lct_server.cpp  \
	  $(SRCDIR)/acs_lct_main.cpp \
	  $(SRCDIR)/acs_lct_dsdserver.cpp \
	  $(SRCDIR)/acs_lct_reactorrunner.cpp \
	  $(SRCDIR)/acs_lct_adm.cpp

HARDENING_OBJ = $(OBJDIR)/acs_lct_command_handler_tsusers.obj \
		$(OBJDIR)/acs_lct_command_handler_hardening.obj\
		$(OBJDIR)/acs_lct_command_handler_securityhandling.obj\
		$(OBJDIR)/acs_lct_server.obj\
		$(OBJDIR)/acs_lct_main.obj\
		$(OBJDIR)/acs_lct_dsdserver.obj\
		$(OBJDIR)/acs_lct_reactorrunner.obj\
		$(OBJDIR)/acs_lct_adm.obj

COVERAGE ?= no

# here you can add own compiler flags
ifeq ($(COVERAGE),yes)
CFLAGS += -O0 -fprofile-arcs -g -ftest-coverage
else
CFLAGS += -O3
endif

## # here you can add own libs
LIBS += -lrt -lpthread -lACE -lacs_aeh -lboost_serialization -lboost_filesystem -lboost_system -lsec_credu_api -lsec_crypto_api -lacs_prc -lacs_csapi -lacs_apgcc -lacs_dsd -lacs_tra -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf 

OBJ = $(HARDENING_SRC:%.cpp=%.obj)

OUTDIR = $(CXC_PATH)/bin

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) $(OBJDIR)

HARDENING_APNAME = acs_lct_hardeningd

.PHONY: all CFLAGS += $(GCOV_FLAGS)

$(HARDENING_APNAME): $(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(HARDENING_APNAME)'
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(HARDENING_APNAME) $(OBJDIR)/*.obj $(LIBSDIR) $(LIBS)
	$(call stripp,$(HARDENING_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

all:  $(HARDENING_APNAME)

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


