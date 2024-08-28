# **********************************************************************
#
# Short description:
# Makefile template for 
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
#            2010-12-07   tcskapr   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= emf_cnz
CAA_NAME ?= emfadm_caa
CXC_NAME ?= emf_cxc

# Basic paths
VOB_PATH = $(ACS_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXCDIR)/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INCDIR   = $(CURDIR)/inc
TESTDIR  = $(CURDIR)/test

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE 
# -DEMFONSIMAP -DEMFDEBUG -g
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I"$(ACS_ROOT)/apgcc_cnz/apgccapi_caa/acs_security/inc_ext" -I$(INCDIR) -I$(BLOCKDIR)/../emfcmd_caa/inc

# here you can add own libs
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

#AP SDK
LIBSDIR += -L$(AP_SDK_LIB)
LIBS += -lpthread -lacs_apgcc -lacs_dsd -lacs_csapi -lacs_prc -lacs_tra -lacs_aeh -lm

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE

#OpenSaf
LIBSDIR += -L$(COREMW_SDK_LIB)
LIBS += -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf

#BOOST
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBS += -lboost_serialization -lboost_filesystem -lboost_system

#SEC SDK
LIBSDIR += -L$(SEC_SDK_LIB) -L$(SEC_CREDU_SDK_LIB)
LIBS += -lsec_crypto_api -lsec_credu_api

#LOGCPLUS SDK
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB)
LIBS += -llog4cplus


## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) $(BLOCKDIR)/../emfcmd_caa/src/

## Source files

SRCFILES =  acs_emf_tra.cpp acs_emf_root_ObjectImpl.cpp acs_emf_server.cpp acs_emf_haservice.cpp acs_emf_dvdhandler.cpp acs_emf_param.cpp acs_emf_execute.cpp acs_emf_commandhandler.cpp acs_emf_common.cpp acs_emf_execute.cpp acs_emf_aeh.cpp acs_emf_cmd.cpp acs_emf_cmdclient.cpp acs_emf_cmdserver.cpp  acs_emf_dsdserver.cpp acs_emf_mediahandler.cpp acs_emf_nanousbhandler.cpp acs_emf_TScsiDev.cpp
#acs_emf_clienthandler.cpp acs_emfget_commandhandler.cpp acs_emfinfo_commandhandler.cpp 
L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%)

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

## Build instructions
#
APNAME =  acs_emfserverd
APNAME_DBG =  acs_emfserverd_dbg
APEXE = $(OUTDIR)/$(APNAME)

.PHONY: all 
all: $(APEXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files: '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(APNAME)
		$(RM) $(OUTDIR)/.debug/$(APNAME_DBG)

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*.obj) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

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
