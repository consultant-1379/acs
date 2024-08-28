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
#            2010-11-09   xgencol   Adjusted
# **********************************************************************

CNZ_NAME ?= tra_cnz
CAA_NAME ?= traadm_caa
CXC_NAME ?= tra_cxc

include $(COMMON_ROOT)/common.mk 

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

#DOXYGENFILECONFIG := $(TOOLS_PATH)/linux_conf_file/Doxyfile.config
#
#DOCDIR = $(ACS_ROOT)/tra_cnz/tra_cxc/doc
#DOXYGEN_OUTPUT=$(DOCDIR)/docgen
#BLOCK_NAME=TRA_ADM_CAA
#BLOCK_VERSION=1_0
#BLOCK_SEARCH=./src ./inc
#export DOXYGEN_OUTPUT
#export BLOCK_NAME
#export BLOCK_VERSION
#export BLOCK_SEARCH

## Object files
TRAUTIL_OBJ = $(OBJDIR)/ACS_TRA_util.obj

# here you can add own lib paths
LIBSDIR += -L"$(CXC_PATH)/bin/lib_ext"

LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(LIB_LOGCPLUS_SDK_LIB)"

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

# here you can add own Include paths and/or other includes

## # here you can add own Include paths and/or other INCDIRludes
CINCLUDES += -I$(CAA_API_DIR_INT)
CINCLUDES += -I$(CAA_API_DIR_EXT)
CINCLUDES += -I$(CAA_ADM_DIR)

# Compilation Options
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)

CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

# here you can add own libs
LIBS += -lrt -lpthread -lreadline -llog4cplus -lacs_tra -lacs_aeh

.phony: all clean distclean

## here you can add own compiler flags
FLAGS += -O3

# here you can add own linker flags
#LDFLAGS += -s

TRAUTIL_APNAME = trautil

TRAUTIL_APNAME_DBG = trautil_dbg

TRAUTIL_APEXE = $(OUTDIR)/$(TRAUTIL_APNAME)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(TRAUTIL_APNAME)

$(OUTDIR)/$(TRAUTIL_APNAME): $(TRAUTIL_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Linking $@...'
	$(SILENT)$(CC) $(CFLAGS) $(CINCLUDES) -o $(TRAUTIL_APEXE) $(TRAUTIL_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(TRAUTIL_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(TRAUTIL_APNAME)

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

