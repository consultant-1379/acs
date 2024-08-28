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
CAA_NAME ?= alhadm_caa
CXC_NAME ?= alh_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

## Object files
ALHD_OBJ =  $(OBJDIR)/acs_alh_eventmgr.obj \
			$(OBJDIR)/acs_alh_ahtrecord.obj \
			$(OBJDIR)/acs_alh_util.obj \
			$(OBJDIR)/acs_alh_error.obj \
			$(OBJDIR)/acs_alh_ahtmgr.obj \
			$(OBJDIR)/acs_alh_log.obj \
			$(OBJDIR)/acs_alh_imm_runtime_owner.obj \
			$(OBJDIR)/acs_alh_imm_data_handler.obj \
			$(OBJDIR)/acs_alh_alarm_manager.obj \
			$(OBJDIR)/acs_alh_cpnotifier.obj \
			$(OBJDIR)/acs_alh_jtpmgr.obj \
			$(OBJDIR)/acs_alh_main.obj \
			$(OBJDIR)/acs_alh_ha_appmanager.obj \
			$(OBJDIR)/acs_alh_manager.obj \
			$(OBJDIR)/acs_alh_signal_handler.obj
			
## here you can add own compiler flags
CFLAGS += -O3


# here you can add own linker flags
#LDFLAGS += -s


# here you can add own Include paths and/or other includes

## # here you can add own Include paths and/or other INCDIRludes
CINCLUDES += -I"$(CAA_ADM_DIR)"

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
LIBS += -lpthread -lacs_apgcc -lacs_dsd -lacs_prc -lacs_csapi -lacs_tra -lacs_aeh -lacs_jtp

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

.phony: all clean distclean

ALHD_APNAME = acs_alhd

ALHD_APNAME_DBG = acs_alhd_dbg

ALHD_APEXE = $(OUTDIR)/$(ALHD_APNAME)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(ALHD_APNAME)

$(OUTDIR)/$(ALHD_APNAME): $(ALHD_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Linking $@...'
	$(SILENT)$(CC) $(CFLAGS) -o $(ALHD_APEXE) $(ALHD_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(ALHD_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)


.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(ALHD_APNAME)



