# **********************************************************************
#
# Short description:
# Makefile template for ALOG
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
#            2010-11-17   xgencol   Adjusted
# **********************************************************************
CNZ_NAME ?= alog_cnz
CAA_NAME ?= alogadm_caa
CXC_NAME ?= alog_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

SOFTWARE_TYPE ?= app
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf
DOCDIR = $(CXC_PATH)/doc
OUTDIR = $(CXC_PATH)/bin
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
SECINCDIR = $(SEC_SDK_INC)/sec
BLOCK_NAME=ALOG_ADM_CAA
BLOCK_VERSION=1_1
BLOCK_SEARCH=./src ./inc
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

# Setting the application name
APP_FILENAME = acs_alogmaind

## Object files
ALOG_OBJ = $(OBJDIR)/acs_alog_main.obj\
		$(OBJDIR)/acs_alog_parser.obj\
		$(OBJDIR)/acs_alog_nodestate.obj\
		$(OBJDIR)/acs_alog_errorHandler.obj\
		$(OBJDIR)/acs_alog_ha_AppManager.obj\
		$(OBJDIR)/acs_alog_activeWorks.obj\
		$(OBJDIR)/acs_alog_Imm_Implementer.obj\
		$(OBJDIR)/acs_alog_cmdHandler.obj\
		$(OBJDIR)/acs_alog_utility.obj\
		$(OBJDIR)/acs_alog_command.obj\
		$(OBJDIR)/acs_alog_brfc_interface.obj\
		
		

DEPS := $(MODULE_OBJS:.obj=.d)

ifneq ($MAKECMDGOALS),clean)
# include $(DEPS)
endif

# here you can add own lib paths
LIBSDIR += -L"$(LIB_EXT)"
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB) \
-L$(SEC_ACS_SDK_LIB)

## here you can add own compiler flags
CFLAGS += 


# here you can add own linker flags
LDFLAGS += 

ifeq ($(SOFTWARE_TYPE),so)
LDFLAGS += -shared -W1,-soname=$(LIB_SONAME)
OUT_FILENAME = $(LIB_FILENAME)
else
OUT_FILENAME = $(APP_FILENAME)
endif

# here you can add own Include paths and/or other includes
CINCLUDES += -I"$(INCDIR)"

## # here you can add own Include paths and/or other INCDIR includes
CINCLUDES += -I"$(INCINTDIR)"
CINCLUDES += -I"$(INCEXTDIR)"

# Compilation Options
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)


# include files
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I"$(BOOST_SDK_INC)"
CINCLUDES += -I"$(SEC_SDK_INC)"
CINCLUDES += -I"$(SEC_CREDU_SDK_INC)"
CINCLUDES += -I"$(SECINCDIR)"

# here you can add own libs
#AP SDK
LIBSDIR += -L$(AP_SDK_LIB)
LIBS += -lacs_csapi -lacs_prc -lacs_apgcc -lacs_aeh -lacs_tra -laes_ohi -lcap -lacs_dsd -lutil-logger  -llde-logger

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

ALOG_APNAME = acs_alogmaind

ALOG_APNAME_DBG = acs_alogmaind_dbg

ALOG_APEXE = $(OUTDIR)/$(ALOG_APNAME)


VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(ALOG_APNAME)

$(OUTDIR)/$(ALOG_APNAME): $(ALOG_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Linking $@...'
	$(SILENT)$(CC) $(CFLAGS) -o $(ALOG_APEXE) $(ALOG_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APP_FILENAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(ALOG_APNAME)

.PHONY: copy_to_lotc
copy_to_lotc:
	$(SILENT)$(ECHO) 'Copyng application $(OUTDIR)/$(OUT_FILENAME) onto the lotc...'
	$(SILENT) sftp -b copy-to-simlotc.batch $(LOTC_USER) >&/dev/null
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

