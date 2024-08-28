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

CNZ_NAME ?= alog_cnz
CAA_NAME ?= alogadm_caa
CXC_NAME ?= alog_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

OUTDIR = $(CXC_PATH)/bin
SECINCDIR = $(SEC_SDK_INC)/sec
# libssh2 include files
LIBSDIR += -L$(CXC_PATH)/bin/LIB_EXT
LIBSDIR += -L$(CXC_PATH)/bin/LIB_INT

# here you can add own linker flags
LDFLAGS += 


## # here you can add own Include paths and/or other INCDIRludes
CINCLUDES += -I"$(CAA_CMD_DIR)"
CINCLUDES += -I"$(CAA_API_DIR_INT)"
CINCLUDES += -I"$(CAA_API_DIR_EXT)"
CINCLUDES += -I"$(INCDIR)"
CINCLUDES += -I"$(CAA_PATH)/../alogadm_caa/inc_ext" 

# Compilation Options
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(COREMW_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(BOOST_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB)


# include files
CINCLUDES += -I"$(AP_SDK_INC)"
CINCLUDES += -I"$(ACE_SDK_INC)"
CINCLUDES += -I"$(COREMW_SDK_INC)"
CINCLUDES += -I"$(3PP_SDK_PATH)"
CINCLUDES += -I$(SEC_SDK_INC)
CINCLUDES += -I$(SEC_CREDU_SDK_INC)
CINCLUDES += -I"$(SECINCDIR)"


# here you can add own libs
#AP SDK
LIBSDIR += -L$(AP_SDK_LIB)
LIBS += -lacs_csapi -lacs_prc -lacs_apgcc -lacs_aeh -lacs_tra -laes_ohi -lacs_dsd  
LIBSDIR += -L$(SEC_CREDU_SDK_LIB) 
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB)
LIBS += -lsec_crypto_api -lsec_credu_api

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE


.phony: all clean distclean

ALOGFIND_OBJ = $(OBJDIR)/alogfind.obj $(OBJDIR)/acs_alog_cmdUtil.obj  $(OBJDIR)/acs_alog_execute.obj $(OBJDIR)/acs_alog_Lister.obj\

ALOGSET_OBJ = $(OBJDIR)/alogset.obj \
               $(OBJDIR)/getoption.obj \
               $(OBJDIR)/acs_alog_cmdUtil.obj \
               $(OBJDIR)/acs_alog_execute.obj \
               
               
ALOGPCHG_OBJ = $(OBJDIR)/alogpchg.obj \
			    $(OBJDIR)/getoption.obj \
                $(OBJDIR)/acs_alog_cmdUtil.obj \
                $(OBJDIR)/acs_alog_execute.obj \
                
ALOGPLS_OBJ = $(OBJDIR)/alogpls.obj \
			    $(OBJDIR)/getoption.obj \
                $(OBJDIR)/acs_alog_cmdUtil.obj \
                $(OBJDIR)/acs_alog_execute.obj \
                $(OBJDIR)/acs_alog_PLFileIterator.obj \
                
ALOGLIST_OBJ = $(OBJDIR)/aloglist.obj \
			    $(OBJDIR)/getoption.obj \
                $(OBJDIR)/acs_alog_cmdUtil.obj \
                $(OBJDIR)/acs_alog_execute.obj \
                $(OBJDIR)/acs_alog_Lister.obj \
                
               
               

ALOGFIND_APNAME = alogfind

ALOGSET_APNAME = alogset

ALOGPCHG_APNAME = alogpchg

ALOGPLS_APNAME = alogpls

ALOGLIST_APNAME = aloglist

ALOGFIND_APNAME_DBG = alogfind_dbg

ALOGSET_APNAME_DBG = alogset_dbg

ALOGPCHG_APNAME_DBG = alogpchg_dbg

ALOGPLS_APNAME_DBG = alogpls_dbg

ALOGLIST_APNAME_DBG = aloglist_dbg

ALOGFIND_APEXE = $(OUTDIR)/$(ALOGFIND_APNAME)

ALOGSET_APEXE = $(OUTDIR)/$(ALOGSET_APNAME)

ALOGPCHG_APEXE = $(OUTDIR)/$(ALOGPCHG_APNAME)

ALOGPLS_APEXE = $(OUTDIR)/$(ALOGPLS_APNAME)

ALOGLIST_APEXE = $(OUTDIR)/$(ALOGLIST_APNAME)

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR) $(CAA_API_DIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(ALOGFIND_APNAME) $(OUTDIR)/$(ALOGSET_APNAME) $(OUTDIR)/$(ALOGPCHG_APNAME) $(OUTDIR)/$(ALOGPLS_APNAME) $(OUTDIR)/$(ALOGLIST_APNAME)

$(OUTDIR)/$(ALOGFIND_APNAME): $(ALOGFIND_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCISPRINT_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(ALOGFIND_APEXE) $(ALOGFIND_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(ALOGFIND_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(OUTDIR)/$(ALOGSET_APNAME): $(ALOGSET_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCISPRINT_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(ALOGSET_APEXE) $(ALOGSET_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(ALOGSET_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
$(OUTDIR)/$(ALOGPCHG_APNAME): $(ALOGPCHG_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCISPRINT_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(ALOGPCHG_APEXE) $(ALOGPCHG_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(ALOGPCHG_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
$(OUTDIR)/$(ALOGPLS_APNAME): $(ALOGPLS_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCISPRINT_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(ALOGPLS_APEXE) $(ALOGPLS_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(ALOGPLS_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
$(OUTDIR)/$(ALOGLIST_APNAME): $(ALOGLIST_OBJ)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(PRCISPRINT_APNAME)'
	$(SILENT)$(CC) $(CFLAGS) -o $(ALOGLIST_APEXE) $(ALOGLIST_OBJ) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(call stripp,$(ALOGLIST_APNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)


.PHONY: clean
clean:
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(ALOGFIND_APNAME)
	$(RM) -r $(OUTDIR)/$(ALOGSET_APNAME)
	$(RM) -r $(OUTDIR)/$(ALOGPCHG_APNAME)
	$(RM) -r $(OUTDIR)/$(ALOGPLS_APNAME)
	$(RM) -r $(OUTDIR)/$(ALOGLIST_APNAME)




