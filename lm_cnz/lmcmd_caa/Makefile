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
# Rev        Date         Name      What
# -----      -------      --------  --------------------------
#            2010-11-24   xnadnar   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= lm_cnz
CAA_NAME ?= lmcmd_caa
CXC_NAME ?= lm_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

CURDIR   = $(CAA_PATH)
BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
LMADMINCDIR= $(CURDIR)/../lmadm_caa/inc
TESTDIR  = $(BLOCKDIR)/test
NTACS = $(ACS_ROOT)

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES+= -I$(INCDIR) -I$(LMADMINCDIR) -I$(ACS_ROOT)/lm_cnz/lmadm_caa/inc  -I$(CURDIR)/../SafeNet_Sentinel/Include
CINCLUDES += -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)

## # here you can add own Include paths and/or other includes
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 

#LIBSDIR +=  -L$(ACS_ROOT)/tra_cnz/tra_cxc/bin/
LIBSDIR += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(CURDIR)/../SafeNet_Sentinel/lib64 -L$(ACE_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(COREMW_SDK_LIB) -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(SEC_ACS_SDK_LIB)

## # here you can add own libs 

LIBS += -ldl -lACE  -lacs_apgcc -lacs_tra -lacs_dsd -lacs_prc -lacs_jtp -llssrv64 -llsinit64 -lsec_crypto_api -lboost_serialization -lboost_filesystem -lboost_system -lacs_aeh -lacs_csapi -llog4cplus -lpthread
LIBS += $(COREMW_LIBS)
LIBS += $(SEC_LIBS)
LIBS += $(SEC_CREDU_LIBS)

## here you can add own File paths
VPATH +=	$(SRCDIR)  $(OBJDIR) $(OUTDIR) $(NTACS)/lm_cnz/lmadm_caa/src/

## Source files
#
SRCFILES = acs_lm_common.cpp acs_lm_common_util.cpp acs_lm_cmd.cpp acs_lm_cmdclient.cpp acs_lm_tra.cpp  lmtestact.cpp lmtestadd.cpp lmtestls.cpp lmtestrm.cpp lmgenlockcode.cpp license_install.cpp show_licenses.cpp lmlkmapls.cpp

		     
IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

## Build instructions
LMTESTACT_APNAME = lmtestact
LMTESTADD_APNAME = lmtestadd
LMTESTRM_APNAME = lmtestrm
LMTESTLS_APNAME = lmtestls
#LMINST_APNAME = lminst
#LMLKLS_APNAME = lmlkls
LMGENLOCKCODE_APNAME = lmgenlockcode
LMLICENSEINSTALL_APNAME = license_install
LMSHOWLICENSES_APNAME = show_licenses
LMLKAPLS_APNAME = lmlkmapls

LMTESTACT_APNAME_DBG = lmtestact_dbg
LMTESTADD_APNAME_DBG = lmtestadd_dbg
LMTESTRM_APNAME_DBG = lmtestrm_dbg
LMTESTLS_APNAME_DBG = lmtestls_dbg
LMGENLOCKCODE_APNAME_DBG = lmgenlockcode_dbg
LMLICENSEINSTALL_APNAME_DBG = license_install_dbg
LMSHOWLICENSES_APNAME_DBG = show_licenses_dbg
LMLKAPLS_APNAME_DBG = lmlkmapls_dbg

LMTESTACT_APEXE = $(OUTDIR)/$(LMTESTACT_APNAME)
LMTESTADD_APEXE = $(OUTDIR)/$(LMTESTADD_APNAME)
LMTESTRM_APEXE = $(OUTDIR)/$(LMTESTRM_APNAME)
LMTESTLS_APEXE = $(OUTDIR)/$(LMTESTLS_APNAME)
#LMINST_APEXE = $(OUTDIR)/$(LMINST_APNAME)
#LMLKLS_APEXE = $(OUTDIR)/$(LMLKLS_APNAME)
LMGENLOCKCODE_APEXE = $(OUTDIR)/$(LMGENLOCKCODE_APNAME)
LMLICENSEINSTALL_APEXE = $(OUTDIR)/$(LMLICENSEINSTALL_APNAME)
LMSHOWLICENSES_APEXE = $(OUTDIR)/$(LMSHOWLICENSES_APNAME)
LMLKAPLS_APEXE = $(OUTDIR)/$(LMLKAPLS_APNAME)

LMTESTACT_OBJDIR = $(OBJDIR)/acs_lm_common.obj   $(OBJDIR)/acs_lm_cmd.obj  $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj  $(OBJDIR)/lmtestact.obj 
LMTESTADD_OBJDIR = $(OBJDIR)/acs_lm_common.obj   $(OBJDIR)/acs_lm_cmd.obj  $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj $(OBJDIR)/lmtestadd.obj
LMTESTRM_OBJDIR = $(OBJDIR)/acs_lm_common.obj   $(OBJDIR)/acs_lm_cmd.obj  $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj $(OBJDIR)/lmtestrm.obj
LMTESTLS_OBJDIR = $(OBJDIR)/acs_lm_common.obj   $(OBJDIR)/acs_lm_cmd.obj  $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj  $(OBJDIR)/lmtestls.obj
#LMINST_OBJDIR = $(OBJDIR)/acs_lm_common.obj   $(OBJDIR)/acs_lm_cmd.obj  $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj  $(OBJDIR)/lminst.obj
#LMLKLS_OBJDIR = $(OBJDIR)/acs_lm_common.obj   $(OBJDIR)/acs_lm_cmd.obj  $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj  $(OBJDIR)/lmlkls.obj
LMGENLOCKCODE_OBJDIR = $(OBJDIR)/acs_lm_common.obj   $(OBJDIR)/acs_lm_cmd.obj  $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj  $(OBJDIR)/lmgenlockcode.obj
LMLICENSEINSTALL_OBJDIR = $(OBJDIR)/acs_lm_common.obj   $(OBJDIR)/acs_lm_cmd.obj  $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj  $(OBJDIR)/license_install.obj
LMSHOWLICENSES_OBJDIR = $(OBJDIR)/acs_lm_common.obj   $(OBJDIR)/acs_lm_common_util.obj $(OBJDIR)/acs_lm_cmd.obj  $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj  $(OBJDIR)/show_licenses.obj
LMLKAPLS_OBJDIR = $(OBJDIR)/acs_lm_common.obj $(OBJDIR)/acs_lm_cmd.obj $(OBJDIR)/acs_lm_cmdclient.obj $(OBJDIR)/acs_lm_tra.obj $(OBJDIR)/lmlkmapls.obj
 
.PHONY: all
all: $(OUTDIR)/$(LMTESTACT_APNAME) $(OUTDIR)/$(LMTESTADD_APNAME)  $(OUTDIR)/$(LMTESTRM_APNAME) $(OUTDIR)/$(LMTESTLS_APNAME) $(OUTDIR)/$(LMINST_APNAME) $(OUTDIR)/$(LMLKLS_APNAME) $(OUTDIR)/$(LMGENLOCKCODE_APNAME) $(OUTDIR)/$(LMLICENSEINSTALL_APNAME) $(OUTDIR)/$(LMSHOWLICENSES_APNAME) $(OUTDIR)/$(LMLKAPLS_APNAME)
		 

	
.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT) rm -f $(wildcard $(OBJDIR)/*)
		

.PHONY: distclean
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(LMTESTACT_APNAME)
		$(RM) $(OUTDIR)/$(LMTESTADD_APNAME)
		$(RM) $(OUTDIR)/$(LMTESTRM_APNAME)
		$(RM) $(OUTDIR)/$(LMTESTLS_APNAME)
#		$(RM) $(OUTDIR)/$(LMINST_APNAME)
#		$(RM) $(OUTDIR)/$(LMLKLS_APNAME)
		$(RM) $(OUTDIR)/$(LMGENLOCKCODE_APNAME)
		$(RM) $(OUTDIR)/$(LMLICENSEINSTALL_APNAME)
		$(RM) $(OUTDIR)/$(LMSHOWLICENSES_APNAME)
		$(RM) $(OUTDIR)/$(LMLKAPLS_APNAME))
		$(RM) $(OUTDIR)/.debug/$(LMTESTACT_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(LMTESTADD_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(LMTESTRM_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(LMTESTLS_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(LMGENLOCKCODE_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(LMLICENSEINSTALL_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(LMSHOWLICENSES_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(LMLKAPLS_APNAME_DBG))

$(LMTESTACT_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(LMTESTACT_APEXE) $(LMTESTACT_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(LMTESTACT_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(LMTESTADD_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(LMTESTADD_APEXE) $(LMTESTADD_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(LMTESTADD_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(LMTESTRM_APEXE):  $(OBJ) $(METRICS) 
	$(NEW_LINE) 
	$(SEPARATOR_STR) 
	$(SILENT)$(ECHO) 'Building target: $@'
	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
	$(CC) -o $(LMTESTRM_APEXE) $(LMTESTRM_OBJDIR) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LMTESTRM_APNAME))
	$(SILENT)$(ECHO) 'Finished building target: $@'
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(LMTESTLS_APEXE): $(OBJ) $(METRICS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building target: $@'
	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
	$(CC) -o $(LMTESTLS_APEXE) $(LMTESTLS_OBJDIR) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LMTESTLS_APNAME))
	$(SILENT)$(ECHO) 'Finished building target: $@'
	$(SEPARATOR_STR)
	$(NEW_LINE)

# $(LMINST_APEXE): $(OBJ) $(METRICS)
#	$(NEW_LINE)
#	$(SEPARATOR_STR)
#	$(SILENT)$(ECHO) 'Building target: $@'
#	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
#	$(CC) -o $(LMINST_APEXE) $(LMINST_OBJDIR) $(LIBSDIR) $(LIBS)
#	$(SILENT)$(ECHO) 'Finished building target: $@'
#	$(SEPARATOR_STR)
#	$(NEW_LINE)
	
# $(LMLKLS_APEXE): $(OBJ) $(METRICS)
#	$(NEW_LINE)
#	$(SEPARATOR_STR)
#	$(SILENT)$(ECHO) 'Building target: $@'
#	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
#	$(CC) -o $(LMLKLS_APEXE) $(LMLKLS_OBJDIR) $(LIBSDIR) $(LIBS)
#	$(SILENT)$(ECHO) 'Finished building target: $@'
#	$(SEPARATOR_STR)
#	$(NEW_LINE)

 $(LMGENLOCKCODE_APEXE): $(OBJ) $(METRICS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building target: $@'
	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
	$(CC) -o $(LMGENLOCKCODE_APEXE) $(LMGENLOCKCODE_OBJDIR) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LMGENLOCKCODE_APNAME))
	$(SILENT)$(ECHO) 'Finished building target: $@'
	$(SEPARATOR_STR)
	$(NEW_LINE)
	
$(LMLICENSEINSTALL_APEXE): $(OBJ) $(METRICS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building target: $@'
	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
	$(CC) -o $(LMLICENSEINSTALL_APEXE) $(LMLICENSEINSTALL_OBJDIR) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LMLICENSEINSTALL_APNAME))
	$(SILENT)$(ECHO) 'Finished building target: $@'
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(LMSHOWLICENSES_APEXE): $(OBJ) $(METRICS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building target: $@'
	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
	$(CC) -o $(LMSHOWLICENSES_APEXE) $(LMSHOWLICENSES_OBJDIR) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LMSHOWLICENSES_APNAME))
	$(SILENT)$(ECHO) 'Finished building target: $@'
	$(SEPARATOR_STR)
	$(NEW_LINE)

$(LMLKAPLS_APEXE): $(OBJ) $(METRICS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building target: $@'
	$(SILENT)$(ECHO) 'Invoking: GCC Linker'
	$(CC) -o $(LMLKAPLS_APEXE) $(LMLKAPLS_OBJDIR) $(LIBSDIR) $(LIBS)
	$(call stripp,$(LMLKAPLS_APNAME))
	$(SILENT)$(ECHO) 'Finished building target: $@'
	$(SEPARATOR_STR)
	$(NEW_LINE)

.PHONY: metrics
metrics : cccc lint

.PHONY: cccc
cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
#		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

				
	
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

