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
#            2010-28-10   efabron   Created
# **********************************************************************

CNZ_NAME ?= alh_cnz
CAA_NAME ?= alhcmd_caa
CXC_NAME ?= alh_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

## # here you can add own Include paths and/or other INCDIRludes
CINCLUDES += -I"$(CAA_CMD_DIR)"

# Compilation Options
LDFLAGS += -Wl,-rpath-link,$(COREMW_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)


# here you can add own libs
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)


#AP SDK
LIBSDIR += -L$(AP_SDK_LIB)
LIBS += -lpthread -lacs_apgcc -lacs_dsd -lacs_prc -lacs_csapi -lacs_tra -lacs_aeh -lrt

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE


.phony: all clean distclean

ALIST_OBJ = $(OBJDIR)/acs_alh_alist.obj \
			$(OBJDIR)/acs_alh_util.obj \
			$(OBJDIR)/acs_alh_log.obj \
			$(OBJDIR)/acs_alh_signal_handler.obj

ACEASE_OBJ = $(OBJDIR)/acs_alh_acease.obj \
			 $(OBJDIR)/acs_alh_util.obj \
			 $(OBJDIR)/acs_alh_acease_signal_handler.obj

ALIST_APNAME = alist

ACEASE_APNAME = acease

ALIST_APNAME_DBG = alist_dbg

ACEASE_APNAME_DBG = acease_dbg

ALIST_APEXE = $(OUTDIR)/$(ALIST_APNAME)

ACEASE_APEXE = $(OUTDIR)/$(ACEASE_APNAME)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(ALIST_APNAME) \
	 $(OUTDIR)/$(ACEASE_APNAME)

$(OUTDIR)/$(ALIST_APNAME): $(ALIST_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(ALIST_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(ALIST_APEXE) $(ALIST_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(ALIST_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(ACEASE_APNAME): $(ACEASE_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(ACEASE_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(ACEASE_APEXE) $(ACEASE_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(ACEASE_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(ALIST_APNAME)
	$(RM) -r $(OUTDIR)/$(ACEASE_APNAME)
