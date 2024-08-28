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
CAA_NAME ?= prcadm_caa
CXC_NAME ?= prc_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

## Object files
PRCISP_OBJ = $(OBJDIR)/ACS_PRC_SignalHandler.obj\
		$(OBJDIR)/ACS_PRC_IspPrintService.obj\
		$(OBJDIR)/ACS_PRC_ispprintBackupThread.obj\
		$(OBJDIR)/ACS_PRC_IspLogEvent.obj\
		$(OBJDIR)/ACS_PRC_IspAdminOperationImplementer.obj\
		$(OBJDIR)/ACS_PRC_IspAdminOperationImplementerThread.obj\
		$(OBJDIR)/ACS_PRC_IspNotificationThread.obj\
		$(OBJDIR)/ACS_PRC_prcispThread.obj

PRCMAN_OBJ = $(OBJDIR)/ACS_PRC_Prcman.obj\
		$(OBJDIR)/ACS_PRC_SignalHandler.obj\
		$(OBJDIR)/ACS_PRC_prcmanThread.obj\
		$(OBJDIR)/ACS_PRC_suInfo.obj\
		$(OBJDIR)/ACS_PRC_RunTimeOwner.obj\
		$(OBJDIR)/ACS_PRC_RunTimeOwnerThread.obj\
		$(OBJDIR)/ACS_PRC_RunTimeOwnerInitializationThread.obj\
		$(OBJDIR)/ACS_PRC_NotificationThread.obj\
		$(OBJDIR)/acs_prc_runlevel.obj\
		$(OBJDIR)/acs_prc_runlevel_thread.obj\
		$(OBJDIR)/acs_prc_eventmatrix_implementer.obj\
		$(OBJDIR)/acs_prc_eventmatrix_implementerThread.obj\
		$(OBJDIR)/acs_prc_eventmatrix_pipe_handler.obj\
		$(OBJDIR)/acs_prc_brfImplementer.obj\
		$(OBJDIR)/acs_prc_brfImplementerThread.obj\
		$(OBJDIR)/acs_prc_observerImplementer.obj

LIBSDIR += -L$(CXC_PATH)/bin/lib_ext

## here you can add own compiler flags
CFLAGS += -O3

# here you can add own linker flags
#LDFLAGS += -s

# here you can add more defines
EXTRA_DEFINES += -DACS_PRCBIN_REVISION="$(CXC_VER)"
EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES)

# here you can add own Include paths and/or other includes

## # here you can add own Include paths and/or other INCDIRludes
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
CINCLUDES += -I$(BOOST_SDK_INC)

#AP SDK
LIBSDIR += -L$(AP_SDK_LIB)
LIBS += -lrt -lpthread -lacs_prc -lacs_apgcc -lacs_tra -lacs_aeh -lacs_dsd -lacs_csapi
LIBS += -lboost_filesystem

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

.phony: all clean distclean

PRCISP_APNAME = acs_prcispd

PRCMMAN_APNAME = acs_prcmand

PRCISP_APNAME_DBG = acs_prcispd_dbg

PRCMMAN_APNAME_DBG = acs_prcmand_dbg

PRCISP_APEXE = $(OUTDIR)/$(PRCISP_APNAME)

PRCMAN_APEXE = $(OUTDIR)/$(PRCMMAN_APNAME)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(PRCISP_APNAME)\
	$(OUTDIR)/$(PRCMMAN_APNAME)

$(OUTDIR)/$(PRCISP_APNAME): $(PRCISP_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Linking $@...'
	$(CC) $(CFLAGS) -o $(PRCISP_APEXE) $(PRCISP_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(PRCISP_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(PRCMMAN_APNAME): $(PRCMAN_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Linking $@...'
	$(SILENT)$(CC) $(CFLAGS) -o $(PRCMAN_APEXE) $(PRCMAN_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(PRCMMAN_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(PRCISP_APNAME)

