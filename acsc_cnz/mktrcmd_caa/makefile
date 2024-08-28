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
#            2010-11-17   xgencol   Adjusted
# **********************************************************************

CNZ_NAME ?= acsc_cnz
CAA_NAME ?= mktrcmd_caa
CXC_NAME ?= mktr_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

# here you can add own lib paths
LIBSDIR += -L"$(CXC_PATH)/bin"

# libssh2 include files
LIBSDIR += -L$(LIB_SSH2_SDK_LIB)
LIBSDIR += -L"$(AP_SDK_LIB)" -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(SEC_ACS_SDK_LIB)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

# here you can add own Include paths and/or other includes
CINCLUDES += -I"$(INCDIR)"

## # here you can add own Include paths and/or other INCDIRludes
CINCLUDES += -I$(CAA_API_DIR)

# libssh2 include files
CINCLUDES += -I$(LIB_SSH2_SDK_INC)
CINCLUDES += -I"$(AP_SDK_INC)" -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)

# here you can add own libs
LIBS += -lrt -lpthread -lacs_prc -lacs_apgcc -lacs_csapi -lacs_dsd -lcap -lsec_crypto_api -lboost_serialization -lboost_filesystem -lboost_system -lacs_tra -lacs_aeh -lACE
LIBS += $(COREMW_LIBS)
LIBS += $(SEC_LIBS)
LIBS += $(SEC_CREDU_LIBS)

.phony: all clean distclean

## Object files
MODULE_OBJS = 	$(OBJDIR)/APGInfo.obj \
				$(OBJDIR)/Main.obj \
				$(OBJDIR)/Mktr.obj \
				$(OBJDIR)/MktrCmd.obj \
				$(OBJDIR)/MktrCmdCsFiller.obj \
				$(OBJDIR)/MktrCmdFiller.obj \
				$(OBJDIR)/MktrConfig.obj \
				$(OBJDIR)/MktrDate.obj \
				$(OBJDIR)/MktrDateFormat.obj \
				$(OBJDIR)/MktrFile.obj \
				$(OBJDIR)/MktrInputParams.obj \
				$(OBJDIR)/MktrSignalHandler.obj
				
APNAME = mktr

APNAME_DBG = mktr_dbg

APEXE = $(OUTDIR)/$(APNAME)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(APNAME)

$(OUTDIR)/$(APNAME): $(MODULE_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(APEXE) $(MODULE_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(APNAME)

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
