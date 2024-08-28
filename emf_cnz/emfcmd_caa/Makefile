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
#            2010-12-02   tcskrpr   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= emf_cnz
CAA_NAME ?= emfcmd_caa
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
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR) -I$(BLOCKDIR)/../emfadm_caa/inc 

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE 
# -DEMFONSIMAP -DEMFDEBUG
#CFLAGS += -O3 -fPIC -g -fmessage-length=0

# here you can add own Assembler flags
ASMFLAGS += 

#LDFLAGS += -s -shared -Wl,-soname=$(LIB_SONAME)

# here you can add own libs
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

#AP SDK
LIBSDIR += -L$(AP_SDK_LIB)
LIBS += -lacs_apgcc -lacs_dsd -lacs_csapi -lacs_prc -lacs_tra -lacs_aeh

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
VPATH += $(SRCDIR) $(OUTDIR) $(BLOCKDIR)/../emfadm_caa/src/

## Source files
#

SRCFILES = acs_emf_aeh.cpp acs_emf_emfcopy.cpp acs_emf_emfinfo.cpp acs_emf_getopt.c acs_emf_execute.cpp acs_emf_tra.cpp acs_emf_common.cpp acs_emf_TScsiDev.cpp

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 
 

## Object files
#
OBJS =	\
	$(SRCFILES:%.cpp=%.obj) \
	$(SRCFILES:%.c=%.obj)  
	
## Build instructions

EMFCOPY_APNAME = emfcopy
EMFCOPY_APNAME_DBG = emfcopy_dbg

EMFINFO_APNAME = emfinfo
EMFINFO_APNAME_DBG = emfinfo_dbg

#EMFGET_APNAME = emfget

EMFCOPY_APEXE = $(OUTDIR)/$(EMFCOPY_APNAME)

EMFINFO_APEXE = $(OUTDIR)/$(EMFINFO_APNAME)

#EMFGET_APEXE = $(OUTDIR)/$(EMFGET_APNAME)

EMFCOPY_OBJDIR =$(OBJDIR)/acs_emf_aeh.obj  $(OBJDIR)/acs_emf_emfcopy.obj $(OBJDIR)/acs_emf_getopt.obj $(OBJDIR)/acs_emf_tra.obj $(OBJDIR)/acs_emf_execute.obj $(OBJDIR)/acs_emf_common.obj $(OBJDIR)/acs_emf_TScsiDev.obj

EMFINFO_OBJDIR = $(OBJDIR)/acs_emf_aeh.obj $(OBJDIR)/acs_emf_getopt.obj $(OBJDIR)/acs_emf_execute.obj $(OBJDIR)/acs_emf_tra.obj $(OBJDIR)/acs_emf_common.obj $(OBJDIR)/acs_emf_emfinfo.obj $(OBJDIR)/acs_emf_TScsiDev.obj

#EMFGET_OBJDIR = $(OBJDIR)/acs_emf_emfget.obj $(OBJDIR)/acs_emf_getopt.obj $(OBJDIR)/acs_emf_execute.obj  $(OBJDIR)/acs_emf_tra.obj $(OBJDIR)/acs_emf_common.obj

all: $(EMFCOPY_APEXE) \
	 $(EMFINFO_APEXE) 
#	 $(EMFGET_APEXE)

.PHONY: clean 
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)
		
.PHONY: distclean 
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(EMFCOPY_APEXE)
		$(RM) $(OUTDIR)/$(EMFINFO_APEXE)
#		$(RM) $(OUTDIR)/$(EMFGET_APEXE)
		$(RM) $(OUTDIR)/.debug/$(EMFCOPY_APEXE_DBG)
		$(RM) $(OUTDIR)/.debug/$(EMFINFO_APEXE_DBG)

$(EMFCOPY_APEXE): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(EMFCOPY_APEXE) $(EMFCOPY_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(EMFCOPY_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(EMFINFO_APEXE): $(OBJS) $(METRICS)
		$(NEW_LINE)
		$(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
		$(SILENT)$(ECHO) 'Invoking: GCC Linker'
		$(CC) -o $(EMFINFO_APEXE) $(EMFINFO_OBJDIR) $(LIBSDIR) $(LIBS)
		$(call stripp,$(EMFINFO_APNAME))
		$(SILENT)$(ECHO) 'Finished building target: $@'
		$(SEPARATOR_STR)
		$(NEW_LINE)

#$(EMFGET_APEXE): $(OBJS) $(METRICS)
#		$(NEW_LINE)
#		$(SEPARATOR_STR)
#		$(SILENT)$(ECHO) 'Building target: $@'
#		$(SILENT)$(ECHO) 'Invoking: GCC Linker'
#		$(CC) -o $(EMFGET_APEXE) $(EMFGET_OBJDIR) $(LIBSDIR) $(LIBS)
#		$(call stripp,$(EMFGET_APNAME))
#		$(SILENT)$(ECHO) 'Finished building target: $@'
#		$(SEPARATOR_STR)
#		$(NEW_LINE)
	    
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
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)
	    
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

