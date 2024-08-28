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

# **********************************************************************

CNZ_NAME ?= bur_cnz
CAA_NAME ?= burcmd_caa
CXC_NAME ?= bur_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

# libssh2 include files
LIBSDIR += -L"$(AP_SDK_LIB)"
LIBSDIR += -L"$(BOOST_SDK_LIB)"
LIBSDIR += -L"$(ACE_SDK_LIB)"
LIBSDIR += -L"$(COREMW_SDK_LIB)"
LIBSDIR += -L"$(SEC_SDK_LIB)"
LIBSDIR += -L"$(LIB_LOGCPLUS_SDK_LIB)"

# here you can add own linker flags
LDFLAGS += -s

## # here you can add own Include paths and/or other INCDIRludes
CINCLUDES += -I"$(INCINTDIR)"
CINCLUDES += -I"$(INCEXTDIR)"
CINCLUDES += -I"$(CAA_CMD_DIR)"
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(BOOST_SDK_INC)"
CINCLUDES += -I"$(BOOST_SDK_INC)"
# here you can add own libs
#LIBS += -lrt -lpthread -lacs_apgcc -lacs_tra -lboost_iostreams -lz -lacs_csapi
LIBS += -lACE -lacs_apgcc -lacs_dsd -lacs_aeh -lacs_tra -lacs_csapi -lacs_prc -lboost_serialization -lboost_filesystem -lboost_system -lSaImmOm -lSaImmOi -lSaAmf -lSaClm -llog4cplus -lsec_crypto_api
# OK LIBS += -lrt -lpthread -lacs_prc -lacs_tra -lacs_apgcc 

## # here you can add own libs

.phony: all clean distclean

BUR_OBJ =  $(OBJDIR)/acs_bur_backup.obj $(OBJDIR)/acs_bur_burbackup.obj $(OBJDIR)/acs_bur_trace.obj $(OBJDIR)/acs_bur_BrmBackupManager.obj $(OBJDIR)/acs_bur_BrmAsyncActionProgress.obj $(OBJDIR)/acs_bur_BrmBackup.obj $(OBJDIR)/acs_bur_util.obj
BUR_OBJ1 =  $(OBJDIR)/acs_bur_restore.obj $(OBJDIR)/acs_bur_burrestore.obj $(OBJDIR)/acs_bur_trace.obj $(OBJDIR)/acs_bur_BrmBackupManager.obj $(OBJDIR)/acs_bur_BrmBackup.obj $(OBJDIR)/acs_bur_util.obj $(OBJDIR)/acs_bur_BrmAsyncActionProgress.obj

BUR_APNAME = burbackup
BUR_APNAME_DBG = burbackup_dbg
BUR_APNAME1 = burrestore
BUR_APNAME1_DBG = burrestore_dbg

BUR_APEXE = $(OUTDIR)/$(BUR_APNAME)
BUR_APEXE1 = $(OUTDIR)/$(BUR_APNAME1)

VPATH += $(SRCDIR) $(OUTDIR) $(INCINTDIR) $(INCEXTDIR) $(OBJDIR) $(CAA_API_DIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(BUR_APNAME) $(OUTDIR)/$(BUR_APNAME1)

$(OUTDIR)/$(BUR_APNAME): $(BUR_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCISPRINT_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(BUR_APEXE) $(BUR_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(BUR_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(BUR_APNAME1): $(BUR_OBJ1)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCISPRINT_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(BUR_APEXE1) $(BUR_OBJ1) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(BUR_APNAME1))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(BUR_APNAME1)
	$(RM) -r $(OUTDIR)/$(BUR_APNAME)



