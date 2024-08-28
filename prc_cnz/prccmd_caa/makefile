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
#            2014-04-23   xsarses   Adjusted
# **********************************************************************

CNZ_NAME ?= prc_cnz
CAA_NAME ?= prccmd_caa
CXC_NAME ?= prc_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

LIBSDIR += -L$(CXC_PATH)/bin/lib_ext

# here you can add own linker flags
#LDFLAGS += -s

# here you can add more defines
EXTRA_DEFINES += -DACS_PRCBIN_REVISION="$(CXC_VER)"
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

## # here you can add own Include paths and/or other INCDIRludes
CINCLUDES += -I"$(CAA_CMD_DIR)"
CINCLUDES += -I"$(CAA_ADM_DIR)"
CINCLUDES += -I"$(CAA_API_DIR_INT)"
CINCLUDES += -I"$(CAA_API_DIR_EXT)"

# Compilation Options
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)

CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

#AP SDK
LIBSDIR += -L$(AP_SDK_LIB)
LIBS += -lacs_prc -lacs_tra -lacs_aeh -lacs_dsd -lacs_csapi -lacs_apgcc

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

.phony: all clean distclean

PRCISPRINT_OBJ = $(OBJDIR)/ispprint.obj\
			$(OBJDIR)/ispprintSignalHandler.obj\
			$(OBJDIR)/getopt.obj

PRCPRCSTATE_OBJ = $(OBJDIR)/acs_prc_state.obj\
				$(CAA_ADM_DIR_OBJ)/ACS_PRC_suInfo.obj

PRCPRCBOOT_OBJ = $(OBJDIR)/acs_prc_reboot.obj

PRCISPRINT_APNAME = ispprint

PRCPRCSTATE_APNAME = prcstate

PRCPRCBOOT_APNAME = prcboot

PRCISPRINT_APNAME_DBG = ispprint_dbg

PRCPRCSTATE_APNAME_DBG = prcstate_dbg

PRCPRCBOOT_APNAME_DBG = prcboot_dbg

PRCISPRINT_APEXE = $(OUTDIR)/$(PRCISPRINT_APNAME)

PRCPRCSTATE_APEXE = $(OUTDIR)/$(PRCPRCSTATE_APNAME)

PRCPRCBOOT_APEXE = $(OUTDIR)/$(PRCPRCBOOT_APNAME)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(PRCISPRINT_APNAME)\
	$(OUTDIR)/$(PRCPRCSTATE_APNAME)\
	$(OUTDIR)/$(PRCPRCBOOT_APNAME)

$(OUTDIR)/$(PRCISPRINT_APNAME): $(PRCISPRINT_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCISPRINT_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(PRCISPRINT_APEXE) $(PRCISPRINT_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(PRCISPRINT_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(PRCPRCSTATE_APNAME): $(PRCPRCSTATE_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCPRCSTATE_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(PRCPRCSTATE_APEXE) $(PRCPRCSTATE_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(PRCPRCSTATE_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
$(OUTDIR)/$(PRCPRCBOOT_APNAME): $(PRCPRCBOOT_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCPRCBOOT_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(PRCPRCBOOT_APEXE) $(PRCPRCBOOT_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(PRCPRCBOOT_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

	
.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(PRCISPRINT_APNAME)
	$(RM) -r $(OUTDIR)/$(PRCPRCSTATE_APNAME)
	$(RM) -r $(OUTDIR)/$(PRCPRCBOOT_APNAME)

