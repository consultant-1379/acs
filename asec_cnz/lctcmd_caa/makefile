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
# Rev	Date	 Name      What
# -----      -------      --------  --------------------------
#	    2010-12-17   XMALSHA   Created
#
#***********************************************************************

CNZ_NAME ?= asec_cnz
CAA_NAME ?= lctcmd_caa
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
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB)

## Source files
LCT_SRC =$(SRCDIR)/acs_lct_integrityap.cpp
SFGEN_SRC =$(SRCDIR)/acs_lct_sfgen.cpp $(SRCDIR)/../../lctadm_caa/src/acs_lct_common.cpp

LCT_OBJ =$(OBJDIR)/acs_lct_integrityap.obj

COVERAGE ?= no

# here you can add own compiler flags
ifeq ($(COVERAGE),yes)
CFLAGS += -O0 -fprofile-arcs -ftest-coverage
else
CFLAGS += -O3
endif

# here you can add own lib paths

## # here you can add own libs
	LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf -lrt -lpthread -lacs_apgcc -lacs_prc -lacs_tra -lacs_aeh -lsec_crypto_api -lsec_credu_api -lboost_system -lboost_serialization -lboost_filesystem -lacs_dsd -lACE -lacs_csapi


## Object filebin
OBJ =   $(LCT_SRC:%.cpp=%.obj) \
	$(LCT_SRC:%.c=%.obj) 

OUTDIR = $(CXC_PATH)/bin

## here you can add own File paths
VPATH += $(SRCDIR) $(OUTDIR) $(OBJDIR)

LCT_APNAME = integrityap

.PHONY: all CFLAGS += $(GCOV_FLAGS)

all:	$(OUTDIR)/$(LCT_APNAME) 


$(OUTDIR)/$(LCT_APNAME):$(OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(LCT_APNAME)'
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(LCT_APNAME) $(LCT_OBJ) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LCT_APNAME))
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




